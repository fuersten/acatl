//
//  mqtt_connack_parser.h
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

#ifndef acatl_mqtt_connack_parser_h
#define acatl_mqtt_connack_parser_h

#include <acatl_mqtt/mqtt_control_packets.h>

#include <acatl/tribool.h>


namespace acatl
{
    namespace mqtt
    {
        
        class ConnectAckParser
        {
        public:
            enum class Status
            {
                ConnectAcknowledgeFlag,
                ConnectReturnCode,
                Ready
            };

            ConnectAckParser()
            : _status(Status::ConnectAcknowledgeFlag)
            {}
            
            void reset()
            {
                _status = Status::ConnectAcknowledgeFlag;
            }
            
            acatl::Tribool parse(uint8_t byte, std::error_code& ec)
            {
                switch(_status) {
                    case Status::ConnectAcknowledgeFlag:
                        if(byte & 0xFE) {
                            ec = mqtt_error::connect_acknowledge_flag_violation;
                            _status = Status::Ready;
                            return acatl::Tribool(false);
                        }
                        _packet._connectAcknowledgeFlag = ConnectAcknowledgeFlags(byte & 0x01);
                        _status = Status::ConnectReturnCode;
                        break;
                    case Status::ConnectReturnCode:
                        if(byte > 5) {
                            ec = mqtt_error::invalid_connect_return_code;
                            _status = Status::Ready;
                            return acatl::Tribool(false);
                        }
                        _packet._connectReturnCode = ConnectReturnCode(byte);
                        _status = Status::Ready;
                        return acatl::Tribool(true);
                    case Status::Ready:
                        ec = mqtt_error::packet_identifier_length_violation;
                        return acatl::Tribool(false);
                }
                
                return acatl::Tribool();
            }

            const ConnAckControlPacket& packet() const {
                return _packet;
            }
            
        private:
            Status _status;
            ConnAckControlPacket _packet;
        };
        
    }
}

#endif
