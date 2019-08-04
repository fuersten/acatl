//
//  mqtt_suback_parser.h
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

#ifndef acatl_mqtt_suback_parser_h
#define acatl_mqtt_suback_parser_h

#include <acatl_mqtt/mqtt_control_packets.h>
#include <acatl_mqtt/mqtt_packet_identifier_parser.h>

#include <acatl/tribool.h>


namespace acatl
{
    namespace mqtt
    {
        
        class SubAckParser
        {
        public:
            enum class Status
            {
                PacketIdentifier,
                QoS,
                Ready
            };

            SubAckParser(uint32_t length)
            : _status(Status::PacketIdentifier)
            , _length(length)
            {}
            
            void reset(uint32_t length)
            {
                _ret = acatl::Tribool();
                _status = Status::PacketIdentifier;
                _length = length;
                _identifierParser.reset();
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
                            if(_length) {
                                _status = Status::QoS;
                            } else {
                                _status = Status::Ready;
                                _ret.set(true);
                            }
                        }
                        break;
                    }
                    case Status::QoS:
                        if(byte > 0x02) {
                            _status = Status::Ready;
                            ec = mqtt_error::invalid_qos_level;
                            _ret.set(false);
                        } else {
                            _packet._qosLevels.push_back(QoSLevel(byte));
                            if(_length == 0) {
                                _status = Status::Ready;
                                _ret.set(true);
                            }
                        }
                        break;
                    case Status::Ready:
                        ec = mqtt_error::publish_protocol_violation;
                        break;
                }
                
                if(_ret && _length != 0) {
                    _ret.set(false);
                    ec = mqtt_error::control_packet_length;
                }
                
                return _ret;
            }

            const SubAckControlPacket& packet() const {
                return _packet;
            }
            
        private:
            Status _status;
            SubAckControlPacket _packet;
            acatl::Tribool _ret;
            uint32_t _length;
            PacketIdentifierParser _identifierParser;
        };
        
    }
}

#endif
