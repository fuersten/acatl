//
//  mqtt_subscribe_parser.h
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

#ifndef acatl_mqtt_subscribe_parser_h
#define acatl_mqtt_subscribe_parser_h

#include <acatl_mqtt/mqtt_control_packets.h>
#include <acatl_mqtt/mqtt_packet_identifier_parser.h>
#include <acatl_mqtt/mqtt_string_parser.h>

#include <acatl/tribool.h>


namespace acatl
{
    namespace mqtt
    {
        
        class SubscribeParser
        {
        public:
            enum class Status
            {
                PacketIdentifier,
                TopicFilter,
                QoS,
                Ready
            };
            
            SubscribeParser(uint32_t length)
            : _status(Status::PacketIdentifier)
            , _length(length)
            , _topicFilter("")
            {}
            
            void reset(uint32_t length)
            {
                _ret = acatl::Tribool();
                _status = Status::PacketIdentifier;
                _length = length;
                _identifierParser.reset();
                _stringParser.reset();
            }
            
            acatl::Tribool parse(uint8_t byte, std::error_code& ec)
            {
                if(_length == 0) {
                    ec = mqtt_error::control_packet_length;
                    _ret.set(false);
                    return _ret;
                }
                
                --_length;
                switch(_status) {
                    case Status::PacketIdentifier: {
                        acatl::Tribool ret = _identifierParser.parse(byte, ec);
                        if(ret.isFalse() || ec) {
                            _status = Status::Ready;
                            _ret.set(false);
                        } else if(ret.isTrue()){
                            _packet._packetIdentifier = _identifierParser.packetIdentifier();
                            _status = Status::TopicFilter;
                            _stringParser.reset();
                        }
                        break;
                    }
                    case Status::TopicFilter:{
                        acatl::Tribool ret = _stringParser.parse(byte, ec);
                        if(ret.isFalse() || ec) {
                            _status = Status::Ready;
                            _ret.set(false);
                        } else if(ret.isTrue()){
                            _topicFilter = TopicFilter(_stringParser.string());
                            _status = Status::QoS;
                            _stringParser.reset();
                        }
                        break;
                    }
                    case Status::QoS:
                        if(byte & 0xFC || (byte & 0x03) > 2) {
                            // bits from 2 to 7 have to be set to 0
                            // and the qos has to be in the range [0..2]
                            ec = mqtt_error::invalid_qos_level;
                            _status = Status::Ready;
                            _ret.set(false);
                        } else {
                            _topicFilter._qos = QoSLevel(byte & 0x03);
                            if(!_topicFilter.validate(ec)) {
                                ec = mqtt_error::invalid_topic_filter;
                                _status = Status::Ready;
                                _ret.set(false);
                            } else {
                                _packet._topicFilters.push_back(_topicFilter);
                                if(_length > 0) {
                                    _status = Status::TopicFilter;
                                } else {
                                    // remove duplicate topics
                                    std::sort(_packet._topicFilters.begin(), _packet._topicFilters.end());
                                    _packet._topicFilters.erase(std::unique(_packet._topicFilters.begin(), _packet._topicFilters.end()), _packet._topicFilters.end());
                                    _status = Status::Ready;
                                    _ret.set(true);
                                }
                            }
                        }
                        break;
                    case Status::Ready:
                        ec = mqtt_error::subscribe_protocol_violation;
                        break;
                }
                
                if(_ret && _length != 0) {
                    _status = Status::Ready;
                    _ret.set(false);
                    ec = mqtt_error::control_packet_length;
                }
                
                return _ret;
            }
            
            const SubscribeControlPacket& packet() const {
                return _packet;
            }
            
        private:
            Status _status;
            SubscribeControlPacket _packet;
            acatl::Tribool _ret;
            uint32_t _length;
            PacketIdentifierParser _identifierParser;
            StringParser _stringParser;
            TopicFilter _topicFilter;
        };
        
    }
}

#endif
