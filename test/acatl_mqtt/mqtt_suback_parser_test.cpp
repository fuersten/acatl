//
//  mqtt_suback_parser_test.cpp
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

#include <acatl_mqtt/mqtt_suback_parser.h>


TEST(MQTTSubAckParserTest, parse)
{
    std::vector<uint8_t> buffer;
    buffer.resize(64);
    
    buffer[0] = 0x00;  // packet identifier msb
    buffer[1] = 0x0A;  // packet identifier lsb
    buffer[2] = 0x01; // QoSLevel
    buffer[3] = 0x00; // QoSLevel
    buffer[4] = 0x02; // QoSLevel
    
    uint32_t index = 0;
    uint32_t length = 5;
    std::error_code ec;
    acatl::mqtt::SubAckParser parser(length);
    acatl::Tribool ret;
    while(ret.isIndeterminate() && index < length) {
        ret = parser.parse(buffer[index++], ec);
    }
    EXPECT_FALSE(ret.isIndeterminate());
    EXPECT_TRUE(ret.isTrue());
    EXPECT_FALSE(ec);
    
    const acatl::mqtt::SubAckControlPacket& packet = parser.packet();
    EXPECT_EQ(10u, packet._packetIdentifier);
    EXPECT_EQ(3u, packet._qosLevels.size());
    EXPECT_EQ(acatl::mqtt::QoSLevel::AtLeastOnce, packet._qosLevels[0]);
    EXPECT_EQ(acatl::mqtt::QoSLevel::AtMostOnce, packet._qosLevels[1]);
    EXPECT_EQ(acatl::mqtt::QoSLevel::ExactlyOnce, packet._qosLevels[2]);
}
