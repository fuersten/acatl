//
//  mqtt_connect_parser_test.cpp
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

#include <acatl_mqtt/mqtt_connect_parser.h>


TEST(MQTTConnectParserTest, parse)
{
    {
        std::vector<uint8_t> buffer;
        buffer.resize(64);
        
        buffer[0] = 0x00;  // protocol name length msb
        buffer[1] = 0x04;  // protocol name length lsb
        buffer[2] = 'M';   // protocol name
        buffer[3] = 'Q';
        buffer[4] = 'T';
        buffer[5] = 'T';
        buffer[6] = 0x04;  // protocol level
        buffer[7] = 0xCE;  // connect flags
        buffer[8] = 0x00;  // keep alive msb
        buffer[9] = 0x0A;  // keep alive lsb
        buffer[10] = 0x00; // client id length msb
        buffer[11] = 0x05; // client id length lsb
        buffer[12] = 'a';  // client id
        buffer[13] = 'e';
        buffer[14] = 'i';
        buffer[15] = 'o';
        buffer[16] = 'u';
        buffer[17] = 0x00; // will topic length msb
        buffer[18] = 0x04; // will topic length lsb
        buffer[19] = 't';  // will topic
        buffer[20] = 'e';
        buffer[21] = 's';
        buffer[22] = 't';
        buffer[23] = 0x00; // will message length msb
        buffer[24] = 0x08; // will message length lsb
        buffer[25] = 't';  // will message
        buffer[26] = 'e';
        buffer[27] = 's';
        buffer[28] = 't';
        buffer[29] = 'i';
        buffer[30] = 'c';
        buffer[31] = 'l';
        buffer[32] = 'e';
        buffer[33] = 0x00; // username length msb
        buffer[34] = 0x07; // username length lsb
        buffer[35] = 'c';  // username
        buffer[36] = 'h';
        buffer[37] = 'e';
        buffer[38] = 'c';
        buffer[39] = 'k';
        buffer[40] = 'e';
        buffer[41] = 'r';
        buffer[42] = 0x00; // password length msb
        buffer[43] = 0x06; // password length lsb
        buffer[44] = 's';  // password
        buffer[45] = 'e';
        buffer[46] = 'c';
        buffer[47] = 'r';
        buffer[48] = 'e';
        buffer[49] = 't';

        uint32_t index = 0;
        uint32_t length = 50;
        std::error_code ec;
        acatl::mqtt::ConnectParser parser(length);
        acatl::Tribool ret;
        while(ret.isIndeterminate() && index < length) {
            ret = parser.parse(buffer[index++], ec);
        }
        EXPECT_FALSE(ret.isIndeterminate());
        EXPECT_TRUE(ret.isTrue());
        EXPECT_FALSE(ec);

        const acatl::mqtt::ConnectControlPacket& packet = parser.packet();
        EXPECT_TRUE(packet._cleanSession);
        EXPECT_TRUE(packet._willFlag);
        EXPECT_EQ(acatl::mqtt::QoSLevel::AtLeastOnce, packet._willQoSLevel);
        EXPECT_FALSE(packet._willRetain);
        EXPECT_TRUE(packet._passwordFlag);
        EXPECT_TRUE(packet._userNameFlag);
        EXPECT_EQ(10, packet._keepAlive);
        EXPECT_EQ("aeiou", packet._clientId);
        EXPECT_EQ("test", packet._willTopic);
        EXPECT_EQ("testicle", packet._willMessage);
        EXPECT_EQ("checker", packet._userName);
        EXPECT_EQ("secret", packet._password);
    }
    {
        std::vector<uint8_t> buffer;
        buffer.resize(64);
        
        buffer[0] = 0x00;  // protocol name length msb
        buffer[1] = 0x04;  // protocol name length lsb
        buffer[2] = 'M';   // protocol name
        buffer[3] = 'Q';
        buffer[4] = 'T';
        buffer[5] = 'T';
        buffer[6] = 0x04;  // protocol level
        buffer[7] = 0xC2;  // connect flags
        buffer[8] = 0xF0;  // keep alive msb
        buffer[9] = 0x0A;  // keep alive lsb
        buffer[10] = 0x00; // client id length msb
        buffer[11] = 0x05; // client id length lsb
        buffer[12] = 'a';  // client id
        buffer[13] = 'e';
        buffer[14] = 'i';
        buffer[15] = 'o';
        buffer[16] = 'u';
        buffer[17] = 0x00; // username length msb
        buffer[18] = 0x07; // username length lsb
        buffer[19] = 'c';  // username
        buffer[20] = 'h';
        buffer[21] = 'e';
        buffer[22] = 'c';
        buffer[23] = 'k';
        buffer[24] = 'e';
        buffer[25] = 'r';
        buffer[26] = 0x00; // password length msb
        buffer[27] = 0x06; // password length lsb
        buffer[28] = 's';  // password
        buffer[29] = 'e';
        buffer[30] = 'c';
        buffer[31] = 'r';
        buffer[32] = 'e';
        buffer[33] = 't';
        
        uint32_t index = 0;
        uint32_t length = 34;
        std::error_code ec;
        acatl::mqtt::ConnectParser parser(length);
        acatl::Tribool ret;
        while(ret.isIndeterminate() && index < length) {
            ret = parser.parse(buffer[index++], ec);
        }
        EXPECT_FALSE(ret.isIndeterminate());
        EXPECT_TRUE(ret.isTrue());
        EXPECT_FALSE(ec);
        
        const acatl::mqtt::ConnectControlPacket& packet = parser.packet();
        EXPECT_TRUE(packet._cleanSession);
        EXPECT_FALSE(packet._willFlag);
        EXPECT_EQ(acatl::mqtt::QoSLevel::AtMostOnce, packet._willQoSLevel);
        EXPECT_FALSE(packet._willRetain);
        EXPECT_TRUE(packet._passwordFlag);
        EXPECT_TRUE(packet._userNameFlag);
        EXPECT_EQ(61450, packet._keepAlive);
        EXPECT_EQ("aeiou", packet._clientId);
        EXPECT_EQ("", packet._willTopic);
        EXPECT_EQ("", packet._willMessage);
        EXPECT_EQ("checker", packet._userName);
        EXPECT_EQ("secret", packet._password);
    }
}

TEST(MQTTConnectParserTest, emptyClientId)
{
        std::vector<uint8_t> buffer;
        buffer.resize(64);
        
        buffer[0] = 0x00;  // protocol name length msb
        buffer[1] = 0x04;  // protocol name length lsb
        buffer[2] = 'M';   // protocol name
        buffer[3] = 'Q';
        buffer[4] = 'T';
        buffer[5] = 'T';
        buffer[6] = 0x04;  // protocol level
        buffer[7] = 0x02;  // connect flags
        buffer[8] = 0xF0;  // keep alive msb
        buffer[9] = 0x0A;  // keep alive lsb
        buffer[10] = 0x00; // client id length msb
        buffer[11] = 0x00; // client id length lsb
    
        uint32_t index = 0;
        uint32_t length = 12;
        std::error_code ec;
        acatl::mqtt::ConnectParser parser(length);
        acatl::Tribool ret;
        while(ret.isIndeterminate() && index < length) {
            ret = parser.parse(buffer[index++], ec);
        }
        EXPECT_FALSE(ret.isIndeterminate());
        EXPECT_TRUE(ret.isTrue());
        EXPECT_FALSE(ec);
        
        const acatl::mqtt::ConnectControlPacket& packet = parser.packet();
        EXPECT_TRUE(packet._cleanSession);
        EXPECT_FALSE(packet._willFlag);
        EXPECT_EQ(acatl::mqtt::QoSLevel::AtMostOnce, packet._willQoSLevel);
        EXPECT_FALSE(packet._willRetain);
        EXPECT_FALSE(packet._passwordFlag);
        EXPECT_FALSE(packet._userNameFlag);
        EXPECT_EQ(61450, packet._keepAlive);
        EXPECT_EQ(36u, packet._clientId.size());
        EXPECT_EQ("", packet._willTopic);
        EXPECT_EQ("", packet._willMessage);
        EXPECT_EQ("", packet._userName);
        EXPECT_EQ("", packet._password);
}
