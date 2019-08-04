//
//  mqtt_subscribe_parser_test.cpp
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

#include <acatl_mqtt/mqtt_subscribe_parser.h>


TEST(MQTTSubscribeParserTest, parse)
{
    std::vector<uint8_t> buffer;
    buffer.resize(64);
    
    buffer[0] = 0x00;  // packet identifier msb
    buffer[1] = 0x0A;  // packet identifier msb
    buffer[2] = 0x00;  // topic filter #1 length msb
    buffer[3] = 0x03;  // topic filter #1 length lsb
    buffer[4] = 'a';   // topic filter #1 name
    buffer[5] = '/';
    buffer[6] = 'b';
    buffer[7] = 0x01;  // topic filter #1 qos
    buffer[8] = 0x00;  // topic filter #2 length msb
    buffer[9] = 0x03;  // topic filter #2 length lsb
    buffer[10] = 'c';  // topic filter #2 name
    buffer[11] = '/';
    buffer[12] = 'd';
    buffer[13] = 0x02; // topic filter #2 qos
    buffer[14] = 0x00; // topic filter #3 length msb
    buffer[15] = 0x0E; // topic filter #3 length lsb
    buffer[16] = 'c';  // topic filter #3 name
    buffer[17] = 'h';
    buffer[18] = 'e';
    buffer[19] = 'c';
    buffer[20] = 'k';
    buffer[21] = '/';
    buffer[22] = 't';
    buffer[23] = 'h';
    buffer[24] = 'i';
    buffer[25] = 's';
    buffer[26] = '/';
    buffer[27] = 'o';
    buffer[28] = 'u';
    buffer[29] = 't';
    buffer[30] = 0x00;  // topic filter #3 qos
    
    uint32_t index = 0;
    uint32_t length = 31;
    std::error_code ec;
    acatl::mqtt::SubscribeParser parser(length);
    acatl::Tribool ret;
    while(ret.isIndeterminate() && index < length) {
        ret = parser.parse(buffer[index++], ec);
    }
    EXPECT_FALSE(ret.isIndeterminate());
    EXPECT_TRUE(ret.isTrue());
    EXPECT_FALSE(ec);
    
    const acatl::mqtt::SubscribeControlPacket& packet = parser.packet();
    EXPECT_EQ(10u, packet._packetIdentifier);
    EXPECT_EQ("check/this/out", packet._topicFilters[0]._filter);
    EXPECT_EQ(acatl::mqtt::QoSLevel::AtMostOnce, packet._topicFilters[0]._qos);
    EXPECT_EQ("a/b", packet._topicFilters[1]._filter);
    EXPECT_EQ(acatl::mqtt::QoSLevel::AtLeastOnce, packet._topicFilters[1]._qos);
    EXPECT_EQ("c/d", packet._topicFilters[2]._filter);
    EXPECT_EQ(acatl::mqtt::QoSLevel::ExactlyOnce, packet._topicFilters[2]._qos);
}

TEST(MQTTSubscribeParserTest, error)
{
    std::vector<uint8_t> buffer;
    buffer.resize(64);
    
    buffer[0] = 0x00;  // packet identifier msb
    buffer[1] = 0x0A;  // packet identifier msb
    buffer[2] = 0x00;  // topic filter #1 length msb
    buffer[3] = 0x03;  // topic filter #1 length lsb
    buffer[4] = 'a';   // topic filter #1 name
    buffer[5] = '/';
    buffer[6] = 'b';
    buffer[7] = 0x01;  // topic filter #1 qos
    buffer[8] = 0x00;  // topic filter #2 length msb
    buffer[9] = 0x03;  // topic filter #2 length lsb
    buffer[10] = 'c';  // topic filter #2 name
    buffer[11] = '/';
    buffer[12] = 'd';
    buffer[13] = 0x07; // topic filter #2 qos
    buffer[14] = 0x00; // topic filter #3 length msb
    buffer[15] = 0x0E; // topic filter #3 length lsb
    buffer[16] = 'c';  // topic filter #3 name
    buffer[17] = 'h';
    buffer[18] = 'e';
    buffer[19] = 'c';
    buffer[20] = 'k';
    buffer[21] = '/';
    buffer[22] = 't';
    buffer[23] = 'h';
    buffer[24] = 'i';
    buffer[25] = 's';
    buffer[26] = '/';
    buffer[27] = 'o';
    buffer[28] = 'u';
    buffer[29] = 't';
    buffer[30] = 0x00;  // topic filter #3 qos
    
    uint32_t index = 0;
    uint32_t length = 31;
    std::error_code ec;
    acatl::mqtt::SubscribeParser parser(length);
    acatl::Tribool ret;
    while(ret.isIndeterminate() && index < length) {
        ret = parser.parse(buffer[index++], ec);
    }
    EXPECT_FALSE(ret.isIndeterminate());
    EXPECT_TRUE(ret.isFalse());
    EXPECT_TRUE(ec);
    EXPECT_EQ(acatl::mqtt::mqtt_error::invalid_qos_level, ec);
}

TEST(MQTTSubscribeParserTest, topicFilterError)
{
    std::vector<uint8_t> buffer;
    buffer.resize(64);
    
    buffer[0] = 0x00;  // packet identifier msb
    buffer[1] = 0x0A;  // packet identifier msb
    buffer[2] = 0x00;  // topic filter #1 length msb
    buffer[3] = 0x03;  // topic filter #1 length lsb
    buffer[4] = 'a';   // topic filter #1 name
    buffer[5] = '/';
    buffer[6] = 'b';
    buffer[7] = 0x01;  // topic filter #1 qos
    buffer[8] = 0x00;  // topic filter #2 length msb
    buffer[9] = 0x03;  // topic filter #2 length lsb
    buffer[10] = 'c';  // topic filter #2 name
    buffer[11] = '/';
    buffer[12] = 'd';
    buffer[13] = 0x02; // topic filter #2 qos
    buffer[14] = 0x00; // topic filter #3 length msb
    buffer[15] = 0x0E; // topic filter #3 length lsb
    buffer[16] = 'c';  // topic filter #3 name
    buffer[17] = 'h';
    buffer[18] = 'e';
    buffer[19] = 'c';
    buffer[20] = 'k';
    buffer[21] = '/';
    buffer[22] = 't';
    buffer[23] = '+';
    buffer[24] = 'i';
    buffer[25] = 's';
    buffer[26] = '/';
    buffer[27] = 'o';
    buffer[28] = 'u';
    buffer[29] = 't';
    buffer[30] = 0x00;  // topic filter #3 qos
    
    uint32_t index = 0;
    uint32_t length = 31;
    std::error_code ec;
    acatl::mqtt::SubscribeParser parser(length);
    acatl::Tribool ret;
    while(ret.isIndeterminate() && index < length) {
        ret = parser.parse(buffer[index++], ec);
    }
    EXPECT_FALSE(ret.isIndeterminate());
    EXPECT_TRUE(ret.isFalse());
    EXPECT_TRUE(ec);
    EXPECT_EQ(acatl::mqtt::mqtt_error::invalid_topic_filter, ec);
}
