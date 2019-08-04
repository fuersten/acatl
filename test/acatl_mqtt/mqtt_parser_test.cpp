//
//  mqtt_parser_test.cpp
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

#include <acatl_mqtt/mqtt_parser.h>


TEST(MQTTParserTest, parseConnect)
{
    std::vector<uint8_t> buffer;
    buffer.resize(64);
    
    buffer[0] = 0x10;  // CONNECT
    buffer[1] = 0x32;  // remaining length
    buffer[2] = 0x00;  // protocol name length msb
    buffer[3] = 0x04;  // protocol name length lsb
    buffer[4] = 'M';   // protocol name
    buffer[5] = 'Q';
    buffer[6] = 'T';
    buffer[7] = 'T';
    buffer[8] = 0x04;  // protocol level
    buffer[9] = 0xCE;  // connect flags
    buffer[10] = 0x00; // keep alive msb
    buffer[11] = 0x0A; // keep alive lsb
    buffer[12] = 0x00; // client id length msb
    buffer[13] = 0x05; // client id length lsb
    buffer[14] = 'a';  // client id
    buffer[15] = 'e';
    buffer[16] = 'i';
    buffer[17] = 'o';
    buffer[18] = 'u';
    buffer[19] = 0x00; // will topic length msb
    buffer[20] = 0x04; // will topic length lsb
    buffer[21] = 't';  // will topic
    buffer[22] = 'e';
    buffer[23] = 's';
    buffer[24] = 't';
    buffer[25] = 0x00; // will message length msb
    buffer[26] = 0x08; // will message length lsb
    buffer[27] = 't';  // will message
    buffer[28] = 'e';
    buffer[29] = 's';
    buffer[30] = 't';
    buffer[31] = 'i';
    buffer[32] = 'c';
    buffer[33] = 'l';
    buffer[34] = 'e';
    buffer[35] = 0x00; // username length msb
    buffer[36] = 0x07; // username length lsb
    buffer[37] = 'c';  // username
    buffer[38] = 'h';
    buffer[39] = 'e';
    buffer[40] = 'c';
    buffer[41] = 'k';
    buffer[42] = 'e';
    buffer[43] = 'r';
    buffer[44] = 0x00; // password length msb
    buffer[45] = 0x06; // password length lsb
    buffer[46] = 's';  // password
    buffer[47] = 'e';
    buffer[48] = 'c';
    buffer[49] = 'r';
    buffer[50] = 'e';
    buffer[51] = 't';
    
    uint32_t index = 0;
    std::error_code ec;
    acatl::mqtt::MQTTParser parser;
    acatl::Tribool ret;
    while(ret.isIndeterminate()) {
        ret = parser.parse(buffer[index++], ec);
    }
    EXPECT_FALSE(ret.isIndeterminate());
    EXPECT_TRUE(ret.isTrue());
    EXPECT_FALSE(ec);
    
    acatl::mqtt::ControlPacket::Ptr packet = parser.consumePacket();
    EXPECT_TRUE(packet);
    const acatl::mqtt::ConnectControlPacket* connect = dynamic_cast<const acatl::mqtt::ConnectControlPacket*>(packet.get());
    ASSERT_TRUE(connect);
    
    EXPECT_TRUE(connect->_cleanSession);
    EXPECT_TRUE(connect->_willFlag);
    EXPECT_EQ(acatl::mqtt::QoSLevel::AtLeastOnce, connect->_willQoSLevel);
    EXPECT_FALSE(connect->_willRetain);
    EXPECT_TRUE(connect->_passwordFlag);
    EXPECT_TRUE(connect->_userNameFlag);
    EXPECT_EQ(10, connect->_keepAlive);
    EXPECT_EQ("aeiou", connect->_clientId);
    EXPECT_EQ("test", connect->_willTopic);
    EXPECT_EQ("testicle", connect->_willMessage);
    EXPECT_EQ("checker", connect->_userName);
    EXPECT_EQ("secret", connect->_password);
}

TEST(MQTTParserTest, parseConnAck)
{
    std::vector<uint8_t> buffer;
    buffer.resize(64);
    
    buffer[0] = 0x20;  // CONNACK
    buffer[1] = 0x02;  // remaining length
    buffer[2] = 0x01;  // connect acknowledge flag
    buffer[3] = 0x00;  // connect return code
    
    uint32_t index = 0;
    std::error_code ec;
    acatl::mqtt::MQTTParser parser;
    acatl::Tribool ret;
    while(ret.isIndeterminate()) {
        ret = parser.parse(buffer[index++], ec);
    }
    EXPECT_FALSE(ret.isIndeterminate());
    EXPECT_TRUE(ret.isTrue());
    EXPECT_FALSE(ec);
    
    acatl::mqtt::ControlPacket::Ptr packet = parser.consumePacket();
    EXPECT_TRUE(packet);
    const acatl::mqtt::ConnAckControlPacket* connack = dynamic_cast<const acatl::mqtt::ConnAckControlPacket*>(packet.get());
    ASSERT_TRUE(connack);
    
    EXPECT_EQ(acatl::mqtt::ConnectAcknowledgeFlags::SessionPresent, connack->_connectAcknowledgeFlag);
    EXPECT_EQ(acatl::mqtt::ConnectReturnCode::ConnectionAccepted, connack->_connectReturnCode);
}

TEST(MQTTParserTest, parsePublish)
{
    std::vector<uint8_t> buffer;
    buffer.resize(64);
    
    buffer[0] = 0x3B;  // PUBLISH
    buffer[1] = 0x1C;  // remaining length
    buffer[2] = 0x00;  // topic name length msb
    buffer[3] = 0x06;  // topic name length lsb
    buffer[4] = 'h';   // topic name
    buffer[5] = 'u';
    buffer[6] = 't';
    buffer[7] = 'z';
    buffer[8] = 'l';
    buffer[9] = 'i';
    buffer[10] = 0x00;  // packet identifier msb
    buffer[11] = 0x0A;  // packet identifier lsb
    buffer[12] = 't'; // payload
    buffer[13] = 'h';
    buffer[14] = 'i';
    buffer[15] = 's';
    buffer[16] = ' ';
    buffer[17] = 'i';
    buffer[18] = 's';
    buffer[19] = ' ';
    buffer[20] = 'm';
    buffer[21] = 'y';
    buffer[22] = ' ';
    buffer[23] = 'p';
    buffer[24] = 'a';
    buffer[25] = 'y';
    buffer[26] = 'l';
    buffer[27] = 'o';
    buffer[28] = 'a';
    buffer[29] = 'd';
    
    uint32_t index = 0;
    std::error_code ec;
    acatl::mqtt::MQTTParser parser;
    acatl::Tribool ret;
    while(ret.isIndeterminate()) {
        ret = parser.parse(buffer[index++], ec);
    }
    EXPECT_FALSE(ret.isIndeterminate());
    EXPECT_TRUE(ret.isTrue());
    EXPECT_FALSE(ec);
    
    acatl::mqtt::ControlPacket::Ptr packet = parser.consumePacket();
    EXPECT_TRUE(packet);
    const acatl::mqtt::PublishControlPacket* publish = dynamic_cast<const acatl::mqtt::PublishControlPacket*>(packet.get());
    ASSERT_TRUE(publish);
    
    EXPECT_EQ("hutzli", publish->_topicName._name);
    EXPECT_EQ(10u, publish->_packetIdentifier);
    EXPECT_EQ(18u, publish->_payload.size());
    EXPECT_EQ("this is my payload", std::string(reinterpret_cast<const char*>(&publish->_payload[0]), publish->_payload.size()));
    
    // reuse parser
    parser.reset();
    index = 0;
    ret = acatl::Tribool();
    while(ret.isIndeterminate()) {
        ret = parser.parse(buffer[index++], ec);
    }
    EXPECT_FALSE(ret.isIndeterminate());
    EXPECT_TRUE(ret.isTrue());
    EXPECT_FALSE(ec);
    
    packet = parser.consumePacket();
    EXPECT_TRUE(packet);
}

TEST(MQTTParserTest, parseSubscribe)
{
    std::vector<uint8_t> buffer;
    buffer.resize(64);
    
    buffer[0] = 0x82;  // SUBSCRIBE
    buffer[1] = 0x1F;  // remaining length
    buffer[2] = 0x00;  // packet identifier msb
    buffer[3] = 0x0A;  // packet identifier msb
    buffer[4] = 0x00;  // topic filter #1 length msb
    buffer[5] = 0x03;  // topic filter #1 length lsb
    buffer[6] = 'a';   // topic filter #1 name
    buffer[7] = '/';
    buffer[8] = 'b';
    buffer[9] = 0x01;  // topic filter #1 qos
    buffer[10] = 0x00; // topic filter #2 length msb
    buffer[11] = 0x03; // topic filter #2 length lsb
    buffer[12] = 'c';  // topic filter #2 name
    buffer[13] = '/';
    buffer[14] = 'd';
    buffer[15] = 0x02; // topic filter #2 qos
    buffer[16] = 0x00; // topic filter #3 length msb
    buffer[17] = 0x0E; // topic filter #3 length lsb
    buffer[18] = 'c';  // topic filter #3 name
    buffer[19] = 'h';
    buffer[20] = 'e';
    buffer[21] = 'c';
    buffer[22] = 'k';
    buffer[23] = '/';
    buffer[24] = 't';
    buffer[25] = 'h';
    buffer[26] = 'i';
    buffer[27] = 's';
    buffer[28] = '/';
    buffer[29] = 'o';
    buffer[30] = 'u';
    buffer[31] = 't';
    buffer[32] = 0x00;  // topic filter #3 qos
    
    uint32_t index = 0;
    std::error_code ec;
    acatl::mqtt::MQTTParser parser;
    acatl::Tribool ret;
    while(ret.isIndeterminate()) {
        ret = parser.parse(buffer[index++], ec);
    }
    EXPECT_FALSE(ret.isIndeterminate());
    EXPECT_TRUE(ret.isTrue());
    EXPECT_FALSE(ec);
    
    acatl::mqtt::ControlPacket::Ptr packet = parser.consumePacket();
    EXPECT_TRUE(packet);
    const acatl::mqtt::SubscribeControlPacket* subscribe = dynamic_cast<const acatl::mqtt::SubscribeControlPacket*>(packet.get());
    ASSERT_TRUE(subscribe);
    
    EXPECT_EQ(10u, subscribe->_packetIdentifier);
    EXPECT_EQ("check/this/out", subscribe->_topicFilters[0]._filter);
    EXPECT_EQ(acatl::mqtt::QoSLevel::AtMostOnce, subscribe->_topicFilters[0]._qos);
    EXPECT_EQ("a/b", subscribe->_topicFilters[1]._filter);
    EXPECT_EQ(acatl::mqtt::QoSLevel::AtLeastOnce, subscribe->_topicFilters[1]._qos);
    EXPECT_EQ("c/d", subscribe->_topicFilters[2]._filter);
    EXPECT_EQ(acatl::mqtt::QoSLevel::ExactlyOnce, subscribe->_topicFilters[2]._qos);
    
    // reuse parser
    index = 0;
    ret = acatl::Tribool();
    parser.reset();
    while(ret.isIndeterminate()) {
        ret = parser.parse(buffer[index++], ec);
    }
    EXPECT_FALSE(ret.isIndeterminate());
    EXPECT_TRUE(ret.isTrue());
    EXPECT_FALSE(ec);
    
    packet = parser.consumePacket();
    EXPECT_TRUE(packet);
    subscribe = dynamic_cast<const acatl::mqtt::SubscribeControlPacket*>(packet.get());
    ASSERT_TRUE(subscribe);
}

TEST(MQTTParserTest, parseSuback)
{
    std::vector<uint8_t> buffer;
    buffer.resize(64);
    
    buffer[0] = 0x90;  // SUBACK
    buffer[1] = 0x05;  // remaining length
    buffer[2] = 0x00;  // packet identifier msb
    buffer[3] = 0x0A;  // packet identifier lsb
    buffer[4] = 0x01;  // QoSLevel
    buffer[5] = 0x00;  // QoSLevel
    buffer[6] = 0x02;  // QoSLevel
    
    uint32_t index = 0;
    std::error_code ec;
    acatl::mqtt::MQTTParser parser;
    acatl::Tribool ret;
    while(ret.isIndeterminate()) {
        ret = parser.parse(buffer[index++], ec);
    }
    EXPECT_FALSE(ret.isIndeterminate());
    EXPECT_TRUE(ret.isTrue());
    EXPECT_FALSE(ec);
    
    acatl::mqtt::ControlPacket::Ptr packet = parser.consumePacket();
    EXPECT_TRUE(packet);
    const acatl::mqtt::SubAckControlPacket* suback = dynamic_cast<const acatl::mqtt::SubAckControlPacket*>(packet.get());
    ASSERT_TRUE(suback);
    
    EXPECT_EQ(10u, suback->_packetIdentifier);
    EXPECT_EQ(3u, suback->_qosLevels.size());
    EXPECT_EQ(acatl::mqtt::QoSLevel::AtLeastOnce, suback->_qosLevels[0]);
    EXPECT_EQ(acatl::mqtt::QoSLevel::AtMostOnce, suback->_qosLevels[1]);
    EXPECT_EQ(acatl::mqtt::QoSLevel::ExactlyOnce, suback->_qosLevels[2]);
}

TEST(MQTTParserTest, parseDisconnect)
{
    std::vector<uint8_t> buffer;
    buffer.resize(64);
    
    buffer[0] = 0xE0;  // DISCONNECT
    buffer[1] = 0x00;
    
    uint32_t index = 0;
    std::error_code ec;
    acatl::mqtt::MQTTParser parser;
    acatl::Tribool ret;
    while(ret.isIndeterminate()) {
        ret = parser.parse(buffer[index++], ec);
    }
    EXPECT_FALSE(ret.isIndeterminate());
    EXPECT_TRUE(ret.isTrue());
    EXPECT_FALSE(ec);
    
    acatl::mqtt::ControlPacket::Ptr packet = parser.consumePacket();
    EXPECT_TRUE(packet);
    const acatl::mqtt::DisconnectControlPacket* disconnect = dynamic_cast<const acatl::mqtt::DisconnectControlPacket*>(packet.get());
    ASSERT_TRUE(disconnect);
    
    EXPECT_EQ(0u, disconnect->_header._flags);
    EXPECT_EQ(0u, disconnect->_header._length);
}

TEST(MQTTParserTest, parsePingReq)
{
    std::vector<uint8_t> buffer;
    buffer.resize(64);
    
    buffer[0] = 0xC0;  // PINGREQ
    buffer[1] = 0x00;  // remaining length
    
    uint32_t index = 0;
    std::error_code ec;
    acatl::mqtt::MQTTParser parser;
    acatl::Tribool ret;
    while(ret.isIndeterminate()) {
        ret = parser.parse(buffer[index++], ec);
    }
    EXPECT_FALSE(ret.isIndeterminate());
    EXPECT_TRUE(ret.isTrue());
    EXPECT_FALSE(ec);
    
    acatl::mqtt::ControlPacket::Ptr packet = parser.consumePacket();
    EXPECT_TRUE(packet);
    const acatl::mqtt::PingReqControlPacket* pingreq = dynamic_cast<const acatl::mqtt::PingReqControlPacket*>(packet.get());
    ASSERT_TRUE(pingreq);
    
    EXPECT_EQ(0u, pingreq->_header._flags);
    EXPECT_EQ(0u, pingreq->_header._length);
}

TEST(MQTTParserTest, parsePingResp)
{
    std::vector<uint8_t> buffer;
    buffer.resize(64);
    
    buffer[0] = 0xD0;  // PINGRESP
    buffer[1] = 0x00;  // remaining length
    
    uint32_t index = 0;
    std::error_code ec;
    acatl::mqtt::MQTTParser parser;
    acatl::Tribool ret;
    while(ret.isIndeterminate()) {
        ret = parser.parse(buffer[index++], ec);
    }
    EXPECT_FALSE(ret.isIndeterminate());
    EXPECT_TRUE(ret.isTrue());
    EXPECT_FALSE(ec);
    
    acatl::mqtt::ControlPacket::Ptr packet = parser.consumePacket();
    EXPECT_TRUE(packet);
    const acatl::mqtt::PingRespControlPacket* pingresp = dynamic_cast<const acatl::mqtt::PingRespControlPacket*>(packet.get());
    ASSERT_TRUE(pingresp);
    
    EXPECT_EQ(0u, pingresp->_header._flags);
    EXPECT_EQ(0u, pingresp->_header._length);
}
