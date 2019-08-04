//
//  mqtt_control_packets.h
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

#ifndef acatl_mqtt_control_packets_h
#define acatl_mqtt_control_packets_h

#include <acatl_mqtt/mqtt_topic.h>


namespace acatl
{
    namespace mqtt
    {
        
        struct FixedHeader
        {
            FixedHeader(ControlPacketType controlPacketType)
            : _controlPacketType(controlPacketType)
            , _flags(0)
            , _length(0)
            {}
            
            FixedHeader()
            : _controlPacketType(ControlPacketType::None)
            , _flags(0)
            , _length(0)
            {}

            bool validate(std::error_code& ec) {
                switch(_controlPacketType) {
                    case ControlPacketType::Connect:
                    case ControlPacketType::Connack:
                    case ControlPacketType::Puback:
                    case ControlPacketType::Pubrec:
                    case ControlPacketType::Pubcomp:
                    case ControlPacketType::Suback:
                    case ControlPacketType::Unsuback:
                    case ControlPacketType::Pingreq:
                    case ControlPacketType::Pingresp:
                    case ControlPacketType::Disconnect:
                        if(_flags != 0) {
                            ec = mqtt_error::connect_flag_protocol_violation;
                            return false;
                        }
                        break;
                    case ControlPacketType::Publish:
                        if((_flags & 0x06) == 0 && _flags & 0x08) {
                            // DUP flag has to be 0 if QoS is AtMostOnce
                            ec = mqtt_error::dup_flag_violation;
                            return false;
                        } else if((_flags & 0x06) == 0x06) {
                            ec = mqtt_error::invalid_qos_level;
                            return false;
                        }
                        break;
                    case ControlPacketType::Pubrel:
                    case ControlPacketType::Subscribe:
                    case ControlPacketType::Unsubscribe:
                        if(_flags != 0x02) {
                            ec = mqtt_error::connect_flag_protocol_violation;
                            return false;
                        }
                        break;
                    default:
                        ec = mqtt_error::invalid_control_packet_type;
                        return false;
                }
                return true;
            }
            
            ControlPacketType _controlPacketType;
            HeaderFlags _flags;
            uint32_t _length;
        };
        
              
        struct ControlPacket
        {
            typedef std::unique_ptr<ControlPacket> Ptr;
            
            ControlPacket(ControlPacketType controlPacketType)
            : _header(controlPacketType)
            {}
            
            virtual ~ControlPacket()
            {}
            
            FixedHeader _header;
        };
        
        struct ConnectControlPacket : public ControlPacket
        {
            typedef std::unique_ptr<ConnectControlPacket> Ptr;
            
            ConnectControlPacket()
            : ControlPacket(ControlPacketType::Connect)
            , _protocolLevel(0)
            , _cleanSession(false)
            , _willFlag(false)
            , _willQoSLevel(QoSLevel::AtMostOnce)
            , _willRetain(false)
            , _passwordFlag(false)
            , _userNameFlag(false)
            {}
            
            ConnectControlPacket(const ConnectControlPacket& rhs) = default;
            
            uint8_t _protocolLevel;
            bool _cleanSession;
            bool _willFlag;
            QoSLevel _willQoSLevel;
            bool _willRetain;
            bool _passwordFlag;
            bool _userNameFlag;
            uint16_t _keepAlive;
            
            std::string _clientId;
            std::string _willTopic;
            std::string _willMessage;
            std::string _userName;
            std::string _password;
        };
        
        struct ConnAckControlPacket : public ControlPacket
        {
            typedef std::unique_ptr<ConnAckControlPacket> Ptr;
            
            ConnAckControlPacket()
            : ControlPacket(ControlPacketType::Connack)
            , _connectAcknowledgeFlag(ConnectAcknowledgeFlags::None)
            , _connectReturnCode(ConnectReturnCode::ConnectionAccepted)
            {}
            
            ConnectAcknowledgeFlags _connectAcknowledgeFlag;
            ConnectReturnCode _connectReturnCode;
        };
        
        struct PingReqControlPacket : public ControlPacket
        {
            typedef std::unique_ptr<PingReqControlPacket> Ptr;
            
            PingReqControlPacket()
            : ControlPacket(ControlPacketType::Pingreq)
            {}
        };

        struct PingRespControlPacket : public ControlPacket
        {
            typedef std::unique_ptr<PingRespControlPacket> Ptr;
            
            PingRespControlPacket()
            : ControlPacket(ControlPacketType::Pingresp)
            {}
        };

        struct PublishControlPacket : public ControlPacket
        {
            typedef std::unique_ptr<PublishControlPacket> Ptr;
            
            PublishControlPacket()
            : ControlPacket(ControlPacketType::Publish)
            {}
            
            TopicName _topicName;
            PacketIdentifier _packetIdentifier;
            std::vector<uint8_t> _payload;
        };
        
        struct SubscribeControlPacket : public ControlPacket
        {
            typedef std::unique_ptr<SubscribeControlPacket> Ptr;
            
            SubscribeControlPacket()
            : ControlPacket(ControlPacketType::Subscribe)
            {}
            
            PacketIdentifier _packetIdentifier;
            TopicFilters _topicFilters;
        };
        
        struct SubAckControlPacket : public ControlPacket
        {
            typedef std::unique_ptr<SubAckControlPacket> Ptr;
            
            SubAckControlPacket()
            : ControlPacket(ControlPacketType::Suback)
            {}
            
            PacketIdentifier _packetIdentifier;
            QoSLevels _qosLevels;
        };
        
        struct DisconnectControlPacket : public ControlPacket
        {
            typedef std::unique_ptr<DisconnectControlPacket> Ptr;
            
            DisconnectControlPacket()
            : ControlPacket(ControlPacketType::Disconnect)
            {}
        };
    }
}

#endif
