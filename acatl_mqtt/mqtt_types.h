//
//  mqtt_types.h
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

#ifndef acatl_mqtt_types_h
#define acatl_mqtt_types_h

#include <acatl_mqtt/mqtt_error.h>

#include <memory>
#include <ostream>


namespace acatl
{
    namespace mqtt
    {
        
        enum class ConnectionState
        {
            Keep,
            Close
        };
        
        enum class ControlPacketType : uint8_t
        {
            None = 0,
            Connect = 0x10,
            Connack = 0x20,
            Publish = 0x30,
            Puback = 0x40,
            Pubrec = 0x50,
            Pubrel = 0x60,
            Pubcomp = 0x70,
            Subscribe = 0x80,
            Suback = 0x90,
            Unsubscribe = 0xA0,
            Unsuback = 0xB0,
            Pingreq = 0xC0,
            Pingresp = 0xD0,
            Disconnect = 0xE0,
            Reserved = 0xF0
        };
        
        template<class CharT, class Traits>
        std::basic_ostream<CharT,Traits>&
        operator<<(std::basic_ostream<CharT,Traits>& os, const ControlPacketType& p)
        {
            switch(p) {
                case ControlPacketType::None:
                    os << "uninitialized";
                    break;
                case ControlPacketType::Connect:
                    os << "connect";
                    break;
                case ControlPacketType::Connack:
                    os << "connack";
                    break;
                case ControlPacketType::Publish:
                    os << "publish";
                    break;
                case ControlPacketType::Puback:
                    os << "puback";
                    break;
                case ControlPacketType::Pubrec:
                    os << "pubrec";
                    break;
                case ControlPacketType::Pubrel:
                    os << "pubrel";
                    break;
                case ControlPacketType::Pubcomp:
                    os << "pubcomp";
                    break;
                case ControlPacketType::Subscribe:
                    os << "subscribe";
                    break;
                case ControlPacketType::Suback:
                    os << "suback";
                    break;
                case ControlPacketType::Unsubscribe:
                    os << "unsubscribe";
                    break;
                case ControlPacketType::Unsuback:
                    os << "unsuback";
                    break;
                case ControlPacketType::Pingreq:
                    os << "pingreq";
                    break;
                case ControlPacketType::Pingresp:
                    os << "pingresp";
                    break;
                case ControlPacketType::Disconnect:
                    os << "disconnect";
                    break;
                case ControlPacketType::Reserved:
                    os << "reserved";
                    break;
            }
            return os;
        }
        
        
        enum class ConnectAcknowledgeFlags
        {
            None = 0x00,
            SessionPresent = 0x01
        };
        
        
        enum class ConnectReturnCode
        {
            ConnectionAccepted = 0x00,
            UnacceptableProtocolVersion = 0x01,
            IdentifierRejected = 0x02,
            ServerUnavailable = 0x03,
            BadUserNameOrPassword = 0x04
        };
        
        
        enum class QoSLevel
        {
            AtMostOnce = 0x00,
            AtLeastOnce = 0x01,
            ExactlyOnce = 0x02,
            Error = 0x80
        };

        template<class CharT, class Traits>
        std::basic_ostream<CharT,Traits>&
        operator<<(std::basic_ostream<CharT,Traits>& os, const QoSLevel& level)
        {
            switch(level) {
                case QoSLevel::AtMostOnce:
                    os << "at most once";
                    break;
                case QoSLevel::AtLeastOnce:
                    os << "at least once";
                    break;
                case QoSLevel::ExactlyOnce:
                    os << "exactly once";
                    break;
                case QoSLevel::Error:
                    os << "error";
                    break;
            }
            return os;
        }

        typedef std::vector<QoSLevel> QoSLevels;
        
        typedef uint8_t HeaderFlags;
        
        typedef uint16_t PacketIdentifier;
        
    }
}

#endif
