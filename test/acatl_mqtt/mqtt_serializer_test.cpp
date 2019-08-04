//
//  mqtt_serializer_test.cpp
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

#include <gtest/gtest.h>

#include <acatl_mqtt/mqtt_serializer.h>


TEST(MQTTSerializerTest, serializeConnect)
{
    acatl::mqtt::Serializer serializer;
    
    acatl::mqtt::ConnectControlPacket::Ptr connect = std::make_unique<acatl::mqtt::ConnectControlPacket>();
    connect->_protocolLevel = 0x04;
    connect->_cleanSession = true;
    connect->_willFlag = true;
    connect->_willQoSLevel = acatl::mqtt::QoSLevel::AtLeastOnce;
    connect->_willRetain = false;
    connect->_passwordFlag = true;
    connect->_userNameFlag = true;
    connect->_keepAlive = 10;
    connect->_clientId = "hutzli";
    connect->_userName = "test";
    connect->_password = "secret";
    connect->_willTopic = "topic";
    connect->_willMessage = "this is my last will";
    
    std::error_code ec;
    size_t length = 0;
    std::vector<uint8_t> buffer;
    
    EXPECT_TRUE(serializer.serialize(std::move(connect), buffer, length, ec));
    EXPECT_FALSE(ec);
    EXPECT_EQ(63u, length);
    
    EXPECT_EQ(0x10, buffer[0]);
    EXPECT_EQ(0x3D, buffer[1]);
    EXPECT_EQ(0x00, buffer[2]);
    EXPECT_EQ(0x04, buffer[3]);
    EXPECT_EQ('M', buffer[4]);
    EXPECT_EQ('Q', buffer[5]);
    EXPECT_EQ('T', buffer[6]);
    EXPECT_EQ('T', buffer[7]);
    EXPECT_EQ(0x04, buffer[8]);
    EXPECT_EQ(0xCE, buffer[9]);
    EXPECT_EQ(0x00, buffer[10]);
    EXPECT_EQ(0x0A, buffer[11]);
    EXPECT_EQ(0x00, buffer[12]);
    EXPECT_EQ(0x06, buffer[13]);
    EXPECT_EQ('h', buffer[14]);
    EXPECT_EQ('i', buffer[19]);
    EXPECT_EQ('s', buffer[57]);
    EXPECT_EQ('t', buffer[62]);
}

TEST(MQTTSerializerTest, serializeConnAck)
{
    acatl::mqtt::Serializer serializer;
    
    acatl::mqtt::ConnAckControlPacket::Ptr connack = std::make_unique<acatl::mqtt::ConnAckControlPacket>();
    connack->_connectAcknowledgeFlag = acatl::mqtt::ConnectAcknowledgeFlags::None;
    connack->_connectReturnCode = acatl::mqtt::ConnectReturnCode::ConnectionAccepted;
    
    std::error_code ec;
    size_t length = 0;
    std::vector<uint8_t> buffer;
    buffer.resize(64);
    
    EXPECT_TRUE(serializer.serialize(std::move(connack), buffer, length, ec));
    EXPECT_FALSE(ec);
    EXPECT_EQ(4u, length);
    
    EXPECT_EQ(0x20, buffer[0]);
    EXPECT_EQ(0x02, buffer[1]);
    EXPECT_EQ(0x00, buffer[2]);
    EXPECT_EQ(0x00, buffer[3]);
}

TEST(MQTTSerializerTest, serializePingResp)
{
    acatl::mqtt::Serializer serializer;
    
    acatl::mqtt::PingRespControlPacket::Ptr pingresp = std::make_unique<acatl::mqtt::PingRespControlPacket>();
    
    std::error_code ec;
    size_t length = 0;
    std::vector<uint8_t> buffer;
    buffer.resize(64);
    
    EXPECT_TRUE(serializer.serialize(std::move(pingresp), buffer, length, ec));
    EXPECT_FALSE(ec);
    EXPECT_EQ(2u, length);
    
    EXPECT_EQ(0xD0, buffer[0]);
    EXPECT_EQ(0x00, buffer[1]);
}

TEST(MQTTSerializerTest, Publish)
{
    acatl::mqtt::Serializer serializer;
    
    acatl::mqtt::PublishControlPacket::Ptr pub = std::make_unique<acatl::mqtt::PublishControlPacket>();
    
    pub->_packetIdentifier = 0;
    pub->_topicName = "sheldon/bazinga";
    pub->_payload = { 'c', 'o', 'o', 'l', '!' };

    std::error_code ec;
    size_t length = 0;
    std::vector<uint8_t> buffer;
    
    EXPECT_TRUE(serializer.serialize(std::move(pub), buffer, length, ec));
    EXPECT_FALSE(ec);
    EXPECT_EQ(24u, length);
    EXPECT_EQ(24u, buffer.size());
    
    EXPECT_EQ(0x30, buffer[0]);
    EXPECT_EQ(0x16, buffer[1]);
    EXPECT_EQ(0x00, buffer[2]);
    EXPECT_EQ(0x0F, buffer[3]);
    EXPECT_EQ('s', buffer[4]);
    EXPECT_EQ('h', buffer[5]);
    EXPECT_EQ('e', buffer[6]);
    EXPECT_EQ('l', buffer[7]);
    EXPECT_EQ('d', buffer[8]);
    EXPECT_EQ('o', buffer[9]);
    EXPECT_EQ('n', buffer[10]);
    EXPECT_EQ('/', buffer[11]);
    EXPECT_EQ('b', buffer[12]);
    EXPECT_EQ('a', buffer[13]);
    EXPECT_EQ('z', buffer[14]);
    EXPECT_EQ('i', buffer[15]);
    EXPECT_EQ('n', buffer[16]);
    EXPECT_EQ('g', buffer[17]);
    EXPECT_EQ('a', buffer[18]);
    EXPECT_EQ('c', buffer[19]);
    EXPECT_EQ('o', buffer[20]);
    EXPECT_EQ('o', buffer[21]);
    EXPECT_EQ('l', buffer[22]);
    EXPECT_EQ('!', buffer[23]);
}

TEST(MQTTSerializerTest, serializeSubscribe)
{
    acatl::mqtt::Serializer serializer;
    
    acatl::mqtt::SubscribeControlPacket::Ptr subscribe = std::make_unique<acatl::mqtt::SubscribeControlPacket>();
    subscribe->_packetIdentifier = 15;
    subscribe->_topicFilters.push_back({ "check/this/out/#", acatl::mqtt::QoSLevel::AtLeastOnce });
    subscribe->_topicFilters.push_back({ "another/cool/topic", acatl::mqtt::QoSLevel::AtMostOnce });
    subscribe->_topicFilters.push_back({ "/best/in/+/class", acatl::mqtt::QoSLevel::ExactlyOnce });
    
    std::error_code ec;
    size_t length = 0;
    std::vector<uint8_t> buffer;
    
    EXPECT_TRUE(serializer.serialize(std::move(subscribe), buffer, length, ec));
    EXPECT_FALSE(ec);
    EXPECT_EQ(63u, length);
    
    EXPECT_EQ(0x82, buffer[0]);
    EXPECT_EQ(61u, buffer[1]);
    EXPECT_EQ(0x00, buffer[2]);
    EXPECT_EQ(0x0F, buffer[3]);
    EXPECT_EQ(0x00, buffer[4]);
    EXPECT_EQ(0x10, buffer[5]);
    EXPECT_EQ('c', buffer[6]);
    EXPECT_EQ('k', buffer[10]);
    EXPECT_EQ('/', buffer[16]);
    EXPECT_EQ('o', buffer[17]);
    EXPECT_EQ('#', buffer[21]);
    EXPECT_EQ(0x01, buffer[22]);
    EXPECT_EQ(0x00, buffer[23]);
    EXPECT_EQ(0x12, buffer[24]);
    EXPECT_EQ('a', buffer[25]);
    EXPECT_EQ('c', buffer[42]);
    EXPECT_EQ(0x00, buffer[43]);
    EXPECT_EQ(0x00, buffer[44]);
    EXPECT_EQ(0x10, buffer[45]);
    EXPECT_EQ('/', buffer[46]);
    EXPECT_EQ('+', buffer[55]);
    EXPECT_EQ('s', buffer[61]);
    EXPECT_EQ(0x02, buffer[62]);
}

TEST(MQTTSerializerTest, serializeSuback)
{
    acatl::mqtt::Serializer serializer;
    
    acatl::mqtt::SubAckControlPacket::Ptr suback = std::make_unique<acatl::mqtt::SubAckControlPacket>();
    suback->_packetIdentifier = 15;
    suback->_qosLevels.push_back(acatl::mqtt::QoSLevel::AtMostOnce);
    suback->_qosLevels.push_back(acatl::mqtt::QoSLevel::AtLeastOnce);
    suback->_qosLevels.push_back(acatl::mqtt::QoSLevel::ExactlyOnce);
    
    std::error_code ec;
    size_t length = 0;
    std::vector<uint8_t> buffer;
    buffer.resize(64);
    
    EXPECT_TRUE(serializer.serialize(std::move(suback), buffer, length, ec));
    EXPECT_FALSE(ec);
    EXPECT_EQ(7u, length);
    
    EXPECT_EQ(0x90, buffer[0]);
}

TEST(MQTTSerializerTest, serializeDisco)
{
    acatl::mqtt::Serializer serializer;
    
    acatl::mqtt::DisconnectControlPacket::Ptr disco = std::make_unique<acatl::mqtt::DisconnectControlPacket>();
    
    std::error_code ec;
    size_t length = 0;
    std::vector<uint8_t> buffer;
    buffer.resize(64);
    
    EXPECT_TRUE(serializer.serialize(std::move(disco), buffer, length, ec));
    EXPECT_FALSE(ec);
    EXPECT_EQ(2u, length);
    
    EXPECT_EQ(0xE0, buffer[0]);
    EXPECT_EQ(0x00, buffer[1]);
}
