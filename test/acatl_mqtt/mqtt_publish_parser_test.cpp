//
//  mqtt_publish_parser_test.cpp
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

#include <acatl_mqtt/mqtt_publish_parser.h>


TEST(MQTTPublishParserTest, parse)
{
    std::vector<uint8_t> buffer;
    buffer.resize(64);
    
    buffer[0] = 0x00;  // topic name length msb
    buffer[1] = 0x06;  // topic name length lsb
    buffer[2] = 'h';   // topic name
    buffer[3] = 'u';
    buffer[4] = 't';
    buffer[5] = 'z';
    buffer[6] = 'l';
    buffer[7] = 'i';
    buffer[8] = 0x00;  // packet identifier msb
    buffer[9] = 0x0A;  // packet identifier lsb
    buffer[10] = 't'; // payload
    buffer[11] = 'h';
    buffer[12] = 'i';
    buffer[13] = 's';
    buffer[14] = ' ';
    buffer[15] = 'i';
    buffer[16] = 's';
    buffer[17] = ' ';
    buffer[18] = 'm';
    buffer[19] = 'y';
    buffer[20] = ' ';
    buffer[21] = 'p';
    buffer[22] = 'a';
    buffer[23] = 'y';
    buffer[24] = 'l';
    buffer[25] = 'o';
    buffer[26] = 'a';
    buffer[27] = 'd';
    
    uint32_t index = 0;
    uint32_t length = 28;
    std::error_code ec;
    acatl::mqtt::PublishParser parser(acatl::mqtt::QoSLevel::AtLeastOnce, length);
    acatl::Tribool ret;
    while(ret.isIndeterminate() && index < length) {
        ret = parser.parse(buffer[index++], ec);
    }
    EXPECT_FALSE(ret.isIndeterminate());
    EXPECT_TRUE(ret.isTrue());
    EXPECT_FALSE(ec);
    
    const acatl::mqtt::PublishControlPacket& packet = parser.packet();
    EXPECT_EQ("hutzli", packet._topicName._name);
    EXPECT_EQ(10u, packet._packetIdentifier);
    EXPECT_EQ(18u, packet._payload.size());
    EXPECT_EQ("this is my payload", std::string(reinterpret_cast<const char*>(&packet._payload[0]), packet._payload.size()));
    
    index = 0;
    length = 28;
    ret = acatl::Tribool();
    parser.reset(acatl::mqtt::QoSLevel::AtLeastOnce, length);
    while(ret.isIndeterminate() && index < length) {
        ret = parser.parse(buffer[index++], ec);
    }
    EXPECT_FALSE(ret.isIndeterminate());
    EXPECT_TRUE(ret.isTrue());
    EXPECT_FALSE(ec);
    
    const acatl::mqtt::PublishControlPacket& packet2 = parser.packet();
    EXPECT_EQ("hutzli", packet2._topicName._name);
    EXPECT_EQ(10u, packet2._packetIdentifier);
    EXPECT_EQ(18u, packet2._payload.size());
    EXPECT_EQ("this is my payload", std::string(reinterpret_cast<const char*>(&packet2._payload[0]), packet2._payload.size()));
}

TEST(MQTTPublishParserTest, parseZeroPayload)
{
    std::vector<uint8_t> buffer;
    buffer.resize(64);
    
    buffer[0] = 0x00;  // topic name length msb
    buffer[1] = 0x06;  // topic name length lsb
    buffer[2] = 'h';   // topic name
    buffer[3] = 'u';
    buffer[4] = 't';
    buffer[5] = 'z';
    buffer[6] = 'l';
    buffer[7] = 'i';
    buffer[8] = 0x00;  // packet identifier msb
    buffer[9] = 0x0F;  // packet identifier lsb
    
    uint32_t index = 0;
    uint32_t length = 10;
    std::error_code ec;
    acatl::mqtt::PublishParser parser(acatl::mqtt::QoSLevel::AtLeastOnce, length);
    acatl::Tribool ret;
    while(ret.isIndeterminate() && index < length) {
        ret = parser.parse(buffer[index++], ec);
    }
    EXPECT_FALSE(ret.isIndeterminate());
    EXPECT_TRUE(ret.isTrue());
    EXPECT_FALSE(ec);
    
    const acatl::mqtt::PublishControlPacket& packet = parser.packet();
    EXPECT_EQ("hutzli", packet._topicName._name);
    EXPECT_EQ(15u, packet._packetIdentifier);
    EXPECT_EQ(0u, packet._payload.size());
}

TEST(MQTTPublishParserTest, wildcardInTopicError)
{
    std::vector<uint8_t> buffer;
    buffer.resize(64);
    
    buffer[0] = 0x00;  // topic name length msb
    buffer[1] = 0x06;  // topic name length lsb
    buffer[2] = 'h';   // topic name
    buffer[3] = 'u';
    buffer[4] = 't';
    buffer[5] = '+';
    buffer[6] = 'l';
    buffer[7] = 'i';
    buffer[8] = 0x00;  // packet identifier msb
    buffer[9] = 0x0F;  // packet identifier lsb
    
    uint32_t index = 0;
    uint32_t length = 10;
    std::error_code ec;
    acatl::mqtt::PublishParser parser(acatl::mqtt::QoSLevel::AtLeastOnce, length);
    acatl::Tribool ret;
    while(ret.isIndeterminate() && index < length) {
        ret = parser.parse(buffer[index++], ec);
    }
    EXPECT_FALSE(ret.isIndeterminate());
    EXPECT_TRUE(ret.isFalse());
    EXPECT_TRUE(ec);
    EXPECT_EQ(acatl::mqtt::mqtt_error::invalid_wildcard_in_topic, ec);
}
