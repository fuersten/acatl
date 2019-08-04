//
//  mqtt_serializer.h
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

#ifndef acatl_mqtt_serializer_h
#define acatl_mqtt_serializer_h

#include <acatl_mqtt/mqtt_control_packets.h>
#include <acatl_mqtt/mqtt_error.h>
#include <acatl_mqtt/mqtt_utils.h>


namespace acatl
{
    namespace mqtt
    {
        
        class Serializer
        {
        public:
            bool serialize(ControlPacket::Ptr packet, std::vector<uint8_t>& buffer, size_t& length, std::error_code& ec)
            {
                switch(packet->_header._controlPacketType) {
                    case ControlPacketType::Connect:
                        return doSerialize(*dynamic_cast<const ConnectControlPacket*>(packet.get()), buffer, length, ec);
                    case ControlPacketType::Connack:
                        return doSerialize(*dynamic_cast<const ConnAckControlPacket*>(packet.get()), buffer, length, ec);
                    case ControlPacketType::Subscribe:
                        return doSerialize(*dynamic_cast<const SubscribeControlPacket*>(packet.get()), buffer, length, ec);
                    case ControlPacketType::Unsubscribe:
                        break;
                    case ControlPacketType::Suback:
                        return doSerialize(*dynamic_cast<const SubAckControlPacket*>(packet.get()), buffer, length, ec);
                    case ControlPacketType::Unsuback:
                    case ControlPacketType::Pingreq:
                        break;
                    case ControlPacketType::Pingresp:
                        return doSerialize(*dynamic_cast<const PingRespControlPacket*>(packet.get()), buffer, length, ec);
                    case ControlPacketType::Disconnect:
                        return doSerialize(*dynamic_cast<const DisconnectControlPacket*>(packet.get()), buffer, length, ec);
                    case ControlPacketType::Publish:
                        return doSerialize(*dynamic_cast<const PublishControlPacket*>(packet.get()), buffer, length, ec);
                    case ControlPacketType::Puback:
                    case ControlPacketType::Pubrec:
                    case ControlPacketType::Pubcomp:
                    case ControlPacketType::Pubrel:
                        break;
                    default:
                        ec = mqtt_error::invalid_control_packet_type;
                        return false;
                }
                ec = mqtt_error::feature_not_implemented;
                return false;
            }
            
        private:
            bool doSerialize(const ConnectControlPacket& connect, std::vector<uint8_t>& buffer, size_t& length, std::error_code& ec)
            {
                uint32_t dataLength = 10 + 2 + static_cast<uint32_t>(connect._clientId.size());
                if(connect._willFlag) {
                    dataLength += 2 + connect._willTopic.size();
                    dataLength += 2 + connect._willMessage.size();
                }
                if(connect._userNameFlag) {
                    dataLength += 2 + connect._userName.size();
                }
                if(connect._passwordFlag) {
                    dataLength += 2 + connect._password.size();
                }
                
                if(buffer.size() < dataLength+2) {
                    buffer.resize(dataLength+2);
                }
                
                length = 0;
                // Connect type
                buffer[length++] = static_cast<uint8_t>(acatl::mqtt::ControlPacketType::Connect);
                // Remaining length
                _lengthEncoder.encode(dataLength, buffer, length);
                // MQTT protocol name
                buffer[length++] = 0x00;
                buffer[length++] = 0x04;
                buffer[length++] = 'M';
                buffer[length++] = 'Q';
                buffer[length++] = 'T';
                buffer[length++] = 'T';
                // Protocol level
                buffer[length++] = 0x04;
                // Connect flag
                buffer[length] = 0;
                if(connect._userNameFlag) {
                    buffer[length] |= 0x80;
                }
                if(connect._passwordFlag) {
                    buffer[length] |= 0x40;
                }
                if(connect._willRetain) {
                    buffer[length] |= 0x20;
                }
                buffer[length] |= 0x18 & (static_cast<uint8_t>(connect._willQoSLevel) << 3);
                if(connect._willFlag) {
                    buffer[length] |= 0x04;
                }
                if(connect._cleanSession) {
                    buffer[length] |= 0x02;
                }
                length++;
                buffer[length++] = (connect._keepAlive & 0xFF00) >> 8;
                buffer[length++] = (connect._keepAlive & 0x00FF);
                _stringEncoder.encode(connect._clientId, buffer, length);
                if(connect._willFlag) {
                    _stringEncoder.encode(connect._willTopic, buffer, length);
                    _stringEncoder.encode(connect._willMessage, buffer, length);
                }
                if(connect._userNameFlag) {
                    _stringEncoder.encode(connect._userName, buffer, length);
                }
                if(connect._passwordFlag) {
                    _stringEncoder.encode(connect._password, buffer, length);
                }
                
                return true;
            }
            
            bool doSerialize(const ConnAckControlPacket& connack, std::vector<uint8_t>& buffer, size_t& length, std::error_code& ec)
            {
                if(buffer.size() < 4) {
                    buffer.resize(4);
                }
                
                buffer[0] = static_cast<uint8_t>(acatl::mqtt::ControlPacketType::Connack);
                buffer[1] = 0x02;
                buffer[2] = static_cast<uint8_t>(connack._connectAcknowledgeFlag);
                buffer[3] = static_cast<uint8_t>(connack._connectReturnCode);
                
                length = 4;
                return true;
            }
            
            bool doSerialize(const DisconnectControlPacket& disco, std::vector<uint8_t>& buffer, size_t& length, std::error_code& ec)
            {
                if(buffer.size() < 2) {
                    buffer.resize(2);
                }
                
                buffer[0] = static_cast<uint8_t>(acatl::mqtt::ControlPacketType::Disconnect);
                buffer[1] = 0x00;
                
                length = 2;
                return true;
            }
            
            bool doSerialize(const PublishControlPacket& publish, std::vector<uint8_t>& buffer, size_t& length, std::error_code& ec)
            {
                uint32_t dataLength = 2 + static_cast<uint32_t>(publish._topicName._name.size() + publish._payload.size());
                // if QoS > 0, add length of packetidentifier
                
                if(buffer.size() < dataLength+2) {
                    buffer.resize(dataLength+2);
                }

                length = 0;
                
                buffer[length] = static_cast<uint8_t>(acatl::mqtt::ControlPacketType::Publish);
                // TODO set DUP, QoS level and RETAIN flags
                ++length;
                _lengthEncoder.encode(dataLength, buffer, length);
                _stringEncoder.encode(publish._topicName._name, buffer, length);
                // if QoS > 0, add packetidentifier
                std::copy(std::begin(publish._payload), std::end(publish._payload), &buffer[length]);
                length += publish._payload.size();
                
                return true;
            }
            
            bool doSerialize(const SubscribeControlPacket& subscribe, std::vector<uint8_t>& buffer, size_t& length, std::error_code& ec)
            {
                uint32_t dataLength = 2;
                for(const auto& filter : subscribe._topicFilters) {
                    dataLength += 3;
                    dataLength += filter._filter.size();
                }
                
                if(buffer.size() < dataLength+2) {
                    buffer.resize(dataLength+2);
                }

                length = 0;
                
                buffer[length] = static_cast<uint8_t>(acatl::mqtt::ControlPacketType::Subscribe);
                buffer[length] |= 0x02;
                ++length;
                _lengthEncoder.encode(dataLength, buffer, length);
                buffer[length++] = (0xFF00 & subscribe._packetIdentifier) >> 8;
                buffer[length++] = (0x00FF & subscribe._packetIdentifier);
                for(const auto& filter : subscribe._topicFilters) {
                    _stringEncoder.encode(filter._filter, buffer, length);
                    buffer[length++] = static_cast<uint8_t>(filter._qos);
                }
                
                return true;
            }

            bool doSerialize(const SubAckControlPacket& suback, std::vector<uint8_t>& buffer, size_t& length, std::error_code& ec)
            {
                if(buffer.size() < suback._qosLevels.size()+2) {
                    buffer.resize(suback._qosLevels.size()+2);
                }

                length = 0;
                
                buffer[length++] = static_cast<uint8_t>(acatl::mqtt::ControlPacketType::Suback);
                buffer[length++] = 0x02 + static_cast<uint8_t>(suback._qosLevels.size());
                buffer[length++] = (0xFF00 & suback._packetIdentifier) >> 8;
                buffer[length++] = (0x00FF & suback._packetIdentifier);
                for(const auto& level : suback._qosLevels) {
                    buffer[length++] = static_cast<uint8_t>(level);
                }
                
                return true;
            }
            
            bool doSerialize(const PingRespControlPacket& pingresp, std::vector<uint8_t>& buffer, size_t& length, std::error_code& ec)
            {
                if(buffer.size() < 2) {
                    buffer.resize(2);
                }

                buffer[0] = static_cast<uint8_t>(acatl::mqtt::ControlPacketType::Pingresp);
                buffer[1] = 0x00;
                length = 2;
                return true;
            }
            
            acatl::mqtt::RunlengthEncoder _lengthEncoder;
            acatl::mqtt::StringEncoder _stringEncoder;
        };
        
    }
}

#endif
