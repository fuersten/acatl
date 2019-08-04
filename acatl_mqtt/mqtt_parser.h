//
//  mqtt_parser.h
//  acatl_mqtt
//
//  BSD 3-Clause License
//  Copyright (c) 2019, Lars-Christian Fürstenberg
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted
//  provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//  conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice, this list of
//  conditions and the following disclaimer in the documentation and/or other materials provided
//  with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its contributors may be used to
//  endorse or promote products derived from this software without specific prior written
//  permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
//  AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//

#ifndef acatl_mqtt_parser_h
#define acatl_mqtt_parser_h

#include <acatl_mqtt/mqtt_connack_parser.h>
#include <acatl_mqtt/mqtt_connect_parser.h>
#include <acatl_mqtt/mqtt_fixed_header_parser.h>
#include <acatl_mqtt/mqtt_publish_parser.h>
#include <acatl_mqtt/mqtt_subscribe_parser.h>
#include <acatl_mqtt/mqtt_suback_parser.h>


namespace acatl
{
    namespace mqtt
    {
        
        class MQTTParser
        {
        public:
            enum class Status
            {
                Start,
                Connect,
                ConnAck,
                Publish,
                Subscribe,
                SubAck,
                Ready
            };
            
            MQTTParser()
            : _status(Status::Start)
            , _connectParser(0)
            , _publishParser(QoSLevel::AtMostOnce, 0)
            , _subscribeParser(0)
            , _subAckParser(0)
            {}
            
            void reset()
            {
                _status = Status::Start;
                _fixedHeaderParser.reset();
            }
            
            acatl::Tribool parse(uint8_t byte, std::error_code& ec)
            {
                switch(_status) {
                    case Status::Start: {
                        acatl::Tribool ret = _fixedHeaderParser.parse(byte, ec);
                        if(ret.isFalse()) {
                            return acatl::Tribool(false);
                        } else if(ret.isTrue()) {
                            switch(_fixedHeaderParser.header()._controlPacketType) {
                                case ControlPacketType::None:
                                    ec = mqtt_error::invalid_control_packet_type;
                                    return acatl::Tribool(false);
                                case ControlPacketType::Connect:
                                    _connectParser.reset(_fixedHeaderParser.header()._length);
                                    _status = Status::Connect;
                                    break;
                                case ControlPacketType::Connack:
                                    _connAckParser.reset();
                                    _status = Status::ConnAck;
                                    break;
                                case ControlPacketType::Publish: {
                                    QoSLevel qos = QoSLevel((_fixedHeaderParser.header()._flags & 0x06) >> 1);
                                    _publishParser.reset(qos, _fixedHeaderParser.header()._length);
                                    _status = Status::Publish;
                                    break;
                                }
                                case ControlPacketType::Puback:
                                case ControlPacketType::Pubrec:
                                case ControlPacketType::Pubrel:
                                case ControlPacketType::Pubcomp:
                                    ec = mqtt_error::feature_not_implemented;
                                    return acatl::Tribool(false);
                                case ControlPacketType::Subscribe:
                                    _subscribeParser.reset(_fixedHeaderParser.header()._length);
                                    _status = Status::Subscribe;
                                    break;
                                case ControlPacketType::Suback:
                                    _subAckParser.reset(_fixedHeaderParser.header()._length);
                                    _status = Status::SubAck;
                                    break;
                                case ControlPacketType::Unsubscribe:
                                case ControlPacketType::Unsuback:
                                    ec = mqtt_error::feature_not_implemented;
                                    return acatl::Tribool(false);
                                case ControlPacketType::Pingreq:
                                    _packet.reset(new PingReqControlPacket);
                                    _packet->_header = _fixedHeaderParser.header();
                                    _status = Status::Ready;
                                    return acatl::Tribool(true);
                                case ControlPacketType::Pingresp:
                                    _packet.reset(new PingRespControlPacket);
                                    _packet->_header = _fixedHeaderParser.header();
                                    _status = Status::Ready;
                                    return acatl::Tribool(true);
                                case ControlPacketType::Disconnect:
                                    _packet.reset(new DisconnectControlPacket);
                                    _packet->_header = _fixedHeaderParser.header();
                                    _status = Status::Ready;
                                    return acatl::Tribool(true);
                                case ControlPacketType::Reserved:
                                    ec = mqtt_error::invalid_control_packet_type;
                                    return acatl::Tribool(false);
                            }
                        }
                        break;
                    }
                    case Status::Connect: {
                        acatl::Tribool ret = _connectParser.parse(byte, ec);
                        if(ret.isFalse()) {
                            return acatl::Tribool(false);
                        } else if(ret.isTrue()) {
                            _packet.reset(new ConnectControlPacket(_connectParser.packet()));
                            _packet->_header = _fixedHeaderParser.header();
                            _status = Status::Ready;
                            return acatl::Tribool(true);
                        }
                        break;
                    }
                    case Status::ConnAck: {
                        acatl::Tribool ret = _connAckParser.parse(byte, ec);
                        if(ret.isFalse()) {
                            return acatl::Tribool(false);
                        } else if(ret.isTrue()) {
                            _packet.reset(new ConnAckControlPacket(_connAckParser.packet()));
                            _packet->_header = _fixedHeaderParser.header();
                            _status = Status::Ready;
                            return acatl::Tribool(true);
                        }
                        break;
                    }
                    case Status::Publish: {
                        acatl::Tribool ret = _publishParser.parse(byte, ec);
                        if(ret.isFalse()) {
                            return acatl::Tribool(false);
                        } else if(ret.isTrue()) {
                            _packet.reset(new PublishControlPacket(_publishParser.packet()));
                            _packet->_header = _fixedHeaderParser.header();
                            _status = Status::Ready;
                            return acatl::Tribool(true);
                        }
                        break;
                    }
                    case Status::Subscribe:{
                        acatl::Tribool ret = _subscribeParser.parse(byte, ec);
                        if(ret.isFalse()) {
                            return acatl::Tribool(false);
                        } else if(ret.isTrue()) {
                            _packet.reset(new SubscribeControlPacket(_subscribeParser.packet()));
                            _packet->_header = _fixedHeaderParser.header();
                            _status = Status::Ready;
                            return acatl::Tribool(true);
                        }
                        break;
                    }
                    case Status::SubAck: {
                        acatl::Tribool ret = _subAckParser.parse(byte, ec);
                        if(ret.isFalse()) {
                            return acatl::Tribool(false);
                        } else if(ret.isTrue()) {
                            _packet.reset(new SubAckControlPacket(_subAckParser.packet()));
                            _packet->_header = _fixedHeaderParser.header();
                            _status = Status::Ready;
                            return acatl::Tribool(true);
                        }
                        break;
                    }
                    case Status::Ready:
                        return acatl::Tribool(false);
                }
                
                return acatl::Tribool();
            }
            
            ControlPacket::Ptr consumePacket()
            {
                reset();
                return std::move(_packet);
            }
            
        private:
            Status _status;
            FixedHeaderParser _fixedHeaderParser;
            ConnectParser _connectParser;
            ConnectAckParser _connAckParser;
            PublishParser _publishParser;
            SubscribeParser _subscribeParser;
            SubAckParser _subAckParser;
            ControlPacket::Ptr _packet;
        };
        
    }
}

#endif
