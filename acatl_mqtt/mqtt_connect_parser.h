//
//  mqtt_connect_parser.h
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

#ifndef acatl_mqtt_connect_parser_h
#define acatl_mqtt_connect_parser_h

#include <acatl_mqtt/mqtt_control_packets.h>
#include <acatl_mqtt/mqtt_string_parser.h>

#include <acatl/tribool.h>
#include <acatl/uuid.h>


namespace acatl
{
    namespace mqtt
    {
        
        class ConnectParser
        {
        public:
            enum class Status
            {
                ProtocolName,
                ProtocolLevel,
                ConnectFlags,
                KeepAliveMsb,
                KeepAliveLsb,
                ClientId,
                WillTopic,
                WillMessage,
                Username,
                Password,
                Ready
            };

            ConnectParser(uint32_t length)
            : _status(Status::ProtocolName)
            , _length(length)
            {}
            
            void reset(uint32_t length)
            {
                _ret = acatl::Tribool();
                _status = Status::ProtocolName;
                _length = length;
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
                    case Status::ProtocolName: {
                        acatl::Tribool ret = _stringParser.parse(byte, ec);
                        if(!ret.isIndeterminate()) {
                            if(_ret.isFalse() || ec) {
                                _status = Status::Ready;
                                _ret.set(false);
                            } else if(_stringParser.string() != "MQTT") {
                                ec = mqtt_error::protocol_name_violation;
                                _status = Status::Ready;
                                _ret.set(false);
                            } else {
                                _status = Status::ProtocolLevel;
                                _stringParser.reset();
                            }
                        }
                        break;
                    }
                    case Status::ProtocolLevel:
                        if(byte != 0x04) {
                            ec = mqtt_error::unacceptable_protocol_level;
                            _status = Status::Ready;
                            _ret.set(false);
                        } else {
                            _packet._protocolLevel = byte;
                            _status = Status::ConnectFlags;
                        }
                        break;
                    case Status::ConnectFlags:
                        if(byte & 0x01) {
                            // reserved flag not set to 0
                            ec = mqtt_error::connect_flag_protocol_violation;
                            _status = Status::Ready;
                            _ret.set(false);
                        } else {
                            _packet._cleanSession = byte & 0x02;
                            _packet._willFlag = byte & 0x04;
                            _packet._willQoSLevel = QoSLevel(static_cast<uint8_t>(((byte & 0x10) << 8) | (byte & 0x08)) >> 3);
                            _packet._willRetain = byte & 0x20;
                            _packet._passwordFlag = byte & 0x40;
                            _packet._userNameFlag = byte & 0x80;
                            _status = Status::KeepAliveMsb;
                        }
                        break;
                    case Status::KeepAliveMsb:
                        _packet._keepAlive = static_cast<uint16_t>(byte << 8);
                        _status = Status::KeepAliveLsb;
                        break;
                    case Status::KeepAliveLsb:
                        _packet._keepAlive |= static_cast<uint16_t>(byte);
                        _status = Status::ClientId;
                        break;
                    case Status::ClientId: {
                        acatl::Tribool ret = _stringParser.parse(byte, ec);
                        if(!ret.isIndeterminate()) {
                            if(_ret.isFalse() || ec) {
                                _status = Status::Ready;
                                _ret.set(false);
                            } else {
                                _packet._clientId = _stringParser.string();
                                if(_packet._clientId.empty()) {
                                    if(!_packet._cleanSession) {
                                        ec = mqtt_error::clean_session_not_set_for_empty_client_id;
                                        _status = Status::Ready;
                                        _ret.set(false);
                                    }
                                    _packet._clientId = acatl::UuidGenerator::generate().toString();
                                }
                                if(_packet._willFlag) {
                                    _status = Status::WillTopic;
                                } else if(_packet._userNameFlag) {
                                    _status = Status::Username;
                                } else {
                                    _status = Status::Ready;
                                    _ret.set(true);
                                }
                                _stringParser.reset();
                            }
                        }
                        break;
                    }
                    case Status::WillTopic: {
                        acatl::Tribool ret = _stringParser.parse(byte, ec);
                        if(!ret.isIndeterminate()) {
                            if(_ret.isFalse() || ec) {
                                _status = Status::Ready;
                                _ret.set(false);
                            } else {
                                _packet._willTopic = _stringParser.string();
                                _status = Status::WillMessage;
                                _stringParser.reset();
                            }
                        }
                        break;
                    }
                    case Status::WillMessage: {
                        acatl::Tribool ret = _stringParser.parse(byte, ec);
                        if(!ret.isIndeterminate()) {
                            if(_ret.isFalse() || ec) {
                                _status = Status::Ready;
                                _ret.set(false);
                            } else {
                                _packet._willMessage = _stringParser.string();
                                if(_packet._userNameFlag) {
                                    _status = Status::Username;
                                } else {
                                    _status = Status::Ready;
                                    _ret.set(true);
                                }
                                _stringParser.reset();
                            }
                        }
                        break;
                    }
                    case Status::Username: {
                        acatl::Tribool ret = _stringParser.parse(byte, ec);
                        if(!ret.isIndeterminate()) {
                            if(_ret.isFalse() || ec) {
                                _status = Status::Ready;
                                _ret.set(false);
                            } else {
                                _packet._userName = _stringParser.string();
                                _status = Status::Password;
                                _stringParser.reset();
                            }
                        }
                        break;
                    }
                    case Status::Password: {
                        acatl::Tribool ret = _stringParser.parse(byte, ec);
                        if(!ret.isIndeterminate()) {
                            if(_ret.isFalse() || ec) {
                                _status = Status::Ready;
                                _ret.set(false);
                            } else {
                                _packet._password = _stringParser.string();
                                _status = Status::Ready;
                                _ret.set(true);
                            }
                        }
                        break;
                    }
                    case Status::Ready:
                        ec = mqtt_error::connect_protocol_violation;
                        break;
                }
                
                if(_ret && _length != 0) {
                    _ret.set(false);
                    ec = mqtt_error::control_packet_length;
                }
                
                return _ret;
            }

            const ConnectControlPacket& packet() const {
                return _packet;
            }
            
        private:
            Status _status;
            ConnectControlPacket _packet;
            acatl::Tribool _ret;
            uint32_t _length;
            StringParser _stringParser;
        };
        
    }
}

#endif
