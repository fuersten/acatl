//
//  mqtt_utils_test.cpp
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

#include <acatl_mqtt/mqtt_utils.h>


TEST(MQTTUtilsTest, runlengthdecoder)
{
    acatl::mqtt::RunlengthDecoder decoder;
    
    std::error_code ec;
    auto ret = decoder.decode(0x40, ec);
    EXPECT_EQ(1, std::get<0>(ret));
    EXPECT_FALSE(ec);
    EXPECT_EQ(64u, std::get<1>(ret));
    
    decoder.reset();
    ret = decoder.decode(0xC1, ec);
    EXPECT_EQ(0, std::get<0>(ret));
    EXPECT_FALSE(ec);
    ret = decoder.decode(0x02, ec);
    EXPECT_EQ(1, std::get<0>(ret));
    EXPECT_FALSE(ec);
    EXPECT_EQ(321u, std::get<1>(ret));

    decoder.reset();
    ret = decoder.decode(0xFF, ec);
    EXPECT_EQ(0, std::get<0>(ret));
    EXPECT_FALSE(ec);
    ret = decoder.decode(0xFF, ec);
    EXPECT_EQ(0, std::get<0>(ret));
    EXPECT_FALSE(ec);
    ret = decoder.decode(0xFF, ec);
    EXPECT_EQ(0, std::get<0>(ret));
    EXPECT_FALSE(ec);
    ret = decoder.decode(0x7F, ec);
    EXPECT_EQ(1, std::get<0>(ret));
    EXPECT_FALSE(ec);
    EXPECT_EQ(268435455u, std::get<1>(ret));
    
    decoder.reset();
    ret = decoder.decode(0xFF, ec);
    ret = decoder.decode(0xFF, ec);
    ret = decoder.decode(0xFF, ec);
    ret = decoder.decode(0xFF, ec);
    ret = decoder.decode(0x7F, ec);
    EXPECT_EQ(-1, std::get<0>(ret));
    EXPECT_TRUE(ec);
    EXPECT_EQ(0u, std::get<1>(ret));
}

TEST(MQTTUtilsTest, runlengthencoder)
{
    std::vector<uint8_t> buffer;
    buffer.resize(4);
    
    acatl::mqtt::RunlengthEncoder encoder;
    size_t index = 0;
    encoder.encode(64, buffer, index);
    EXPECT_EQ(1u, index);
    EXPECT_EQ(0x40, buffer[0]);
    
    index = 0;
    encoder.encode(321, buffer, index);
    EXPECT_EQ(2u, index);
    EXPECT_EQ(0xC1, buffer[0]);
    EXPECT_EQ(0x02, buffer[1]);
    
    index = 0;
    encoder.encode(268435455, buffer, index);
    EXPECT_EQ(4u, index);
    EXPECT_EQ(0xFF, buffer[0]);
    EXPECT_EQ(0xFF, buffer[1]);
    EXPECT_EQ(0xFF, buffer[2]);
    EXPECT_EQ(0x7F, buffer[3]);
}
