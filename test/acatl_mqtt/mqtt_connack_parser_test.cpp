//
//  mqtt_connack_parser_test.cpp
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

#include <acatl_mqtt/mqtt_connack_parser.h>


TEST(MQTTConnAckParserTest, parse)
{
    {
        std::vector<uint8_t> buffer;
        buffer.resize(64);
        
        buffer[0] = 0x01;  // connect acknowledge flag
        buffer[1] = 0x00;  // connect return code

        uint32_t index = 0;
        uint32_t length = 2;
        std::error_code ec;
        acatl::mqtt::ConnectAckParser parser;
        acatl::Tribool ret;
        while(ret.isIndeterminate() && index < length) {
            ret = parser.parse(buffer[index++], ec);
        }
        EXPECT_FALSE(ret.isIndeterminate());
        EXPECT_TRUE(ret.isTrue());
        EXPECT_FALSE(ec);

        const acatl::mqtt::ConnAckControlPacket& packet = parser.packet();
        EXPECT_EQ(acatl::mqtt::ConnectAcknowledgeFlags::SessionPresent, packet._connectAcknowledgeFlag);
        EXPECT_EQ(acatl::mqtt::ConnectReturnCode::ConnectionAccepted, packet._connectReturnCode);
    }
    {
        std::vector<uint8_t> buffer;
        buffer.resize(64);
        
        buffer[0] = 0x00;  // connect acknowledge flag
        buffer[1] = 0xEE;  // connect return code
        
        uint32_t index = 0;
        uint32_t length = 2;
        std::error_code ec;
        acatl::mqtt::ConnectAckParser parser;
        acatl::Tribool ret;
        while(ret.isIndeterminate() && index < length) {
            ret = parser.parse(buffer[index++], ec);
        }
        EXPECT_FALSE(ret.isIndeterminate());
        EXPECT_TRUE(ret.isFalse());
        EXPECT_TRUE(ec);
        EXPECT_EQ(acatl::mqtt::mqtt_error::invalid_connect_return_code, ec);
    }
}
