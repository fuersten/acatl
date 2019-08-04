//
//  mqtt_fixed_header_parser_test.cpp
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

#include <acatl_mqtt/mqtt_fixed_header_parser.h>


TEST(MQTTFixedHeaderParserTest, parse)
{
    {
        std::vector<uint8_t> buffer;
        buffer.resize(64);
        
        buffer[0] = 0x10; // CONNECT
        buffer[1] = 0x2C; // 44 remaining length
        
        std::error_code ec;
        acatl::mqtt::FixedHeaderParser parser;
        acatl::Tribool ret = parser.parse(buffer[0], ec);
        EXPECT_TRUE(ret.isIndeterminate());
        EXPECT_FALSE(ec);
        ret = parser.parse(buffer[1], ec);
        EXPECT_FALSE(ret.isIndeterminate());
        EXPECT_TRUE(ret.isTrue());
        EXPECT_FALSE(ec);
        
        EXPECT_EQ(acatl::mqtt::ControlPacketType::Connect, parser.header()._controlPacketType);
        EXPECT_EQ(0u, parser.header()._flags);
        EXPECT_EQ(44u, parser.header()._length);
    }
    {
        std::vector<uint8_t> buffer;
        buffer.resize(64);
        
        buffer[0] = 0x3B; // PUBLISH
        buffer[1] = 0x2C; // 44 remaining length
        
        std::error_code ec;
        acatl::mqtt::FixedHeaderParser parser;
        acatl::Tribool ret = parser.parse(buffer[0], ec);
        EXPECT_TRUE(ret.isIndeterminate());
        EXPECT_FALSE(ec);
        ret = parser.parse(buffer[1], ec);
        EXPECT_FALSE(ret.isIndeterminate());
        EXPECT_TRUE(ret.isTrue());
        EXPECT_FALSE(ec);
        
        EXPECT_EQ(acatl::mqtt::ControlPacketType::Publish, parser.header()._controlPacketType);
        EXPECT_EQ(0x0B, parser.header()._flags);
        EXPECT_EQ(44u, parser.header()._length);
    }
}

TEST(MQTTFixedHeaderParserTest, invalidControlPacketType)
{
    {
        std::vector<uint8_t> buffer;
        buffer.resize(64);
        
        buffer[0] = 0xF0; // invalid packet type
        buffer[1] = 0x2C; // 44 remaining length
        
        std::error_code ec;
        acatl::mqtt::FixedHeaderParser parser;
        acatl::Tribool ret = parser.parse(buffer[0], ec);
        EXPECT_FALSE(ret.isIndeterminate());
        EXPECT_TRUE(ret.isFalse());
        EXPECT_TRUE(ec);
        EXPECT_EQ(acatl::mqtt::mqtt_error::invalid_control_packet_type, ec);
    }
    {
        std::vector<uint8_t> buffer;
        buffer.resize(64);
        
        buffer[0] = 0x00; // invalid packet type
        buffer[1] = 0x2C; // 44 remaining length
        
        std::error_code ec;
        acatl::mqtt::FixedHeaderParser parser;
        acatl::Tribool ret = parser.parse(buffer[0], ec);
        EXPECT_FALSE(ret.isIndeterminate());
        EXPECT_TRUE(ret.isFalse());
        EXPECT_TRUE(ec);
        EXPECT_EQ(acatl::mqtt::mqtt_error::invalid_control_packet_type, ec);
    }
}

TEST(MQTTFixedHeaderParserTest, wrongLengthEncoding)
{
    std::vector<uint8_t> buffer;
    buffer.resize(64);
    
    buffer[0] = 0x20; // CONNACK
    buffer[1] = 0xFF; // wrong remaining length
    buffer[2] = 0xFF;
    buffer[3] = 0xFF;
    buffer[4] = 0xFF;
    buffer[5] = 0x7F;
    
    uint32_t index = 0;
    std::error_code ec;
    acatl::mqtt::FixedHeaderParser parser;
    acatl::Tribool ret;
    while(ret.isIndeterminate() && index < 6) {
        ret = parser.parse(buffer[index++], ec);
    }
    EXPECT_FALSE(ret.isIndeterminate());
    EXPECT_TRUE(ret.isFalse());
    EXPECT_TRUE(ec);
    EXPECT_EQ(acatl::mqtt::mqtt_error::malformed_remaining_length, ec);
}

TEST(MQTTFixedHeaderParserTest, invalidControlPacketFlags)
{
    {
        std::vector<uint8_t> buffer;
        buffer.resize(64);
        
        buffer[0] = 0x11; // invalid packet flags
        buffer[1] = 0x2C; // 44 remaining length
        
        std::error_code ec;
        acatl::mqtt::FixedHeaderParser parser;
        acatl::Tribool ret = parser.parse(buffer[0], ec);
        EXPECT_FALSE(ret.isIndeterminate());
        EXPECT_TRUE(ret.isFalse());
        EXPECT_TRUE(ec);
        EXPECT_EQ(acatl::mqtt::mqtt_error::connect_flag_protocol_violation, ec);
    }
    {
        std::vector<uint8_t> buffer;
        buffer.resize(64);
        
        buffer[0] = 0x60; // invalid packet flags
        buffer[1] = 0x2C; // 44 remaining length
        
        std::error_code ec;
        acatl::mqtt::FixedHeaderParser parser;
        acatl::Tribool ret = parser.parse(buffer[0], ec);
        EXPECT_FALSE(ret.isIndeterminate());
        EXPECT_TRUE(ret.isFalse());
        EXPECT_TRUE(ec);
        EXPECT_EQ(acatl::mqtt::mqtt_error::connect_flag_protocol_violation, ec);
    }
}
