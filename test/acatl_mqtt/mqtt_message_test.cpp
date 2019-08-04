//
//  mqtt_message_test.cpp
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

#include <acatl_mqtt/mqtt_types.h>


TEST(MQTTMessageTest, controlPacketTypeTest)
{
    std::stringstream ss;
    
    ss << acatl::mqtt::ControlPacketType::Connect;
    EXPECT_EQ("connect", ss.str());
    ss.str("");
    ss.clear();

    ss << acatl::mqtt::ControlPacketType::Subscribe;
    EXPECT_EQ("subscribe", ss.str());
    ss.str("");
    ss.clear();
    
    ss << acatl::mqtt::ControlPacketType::Publish;
    EXPECT_EQ("publish", ss.str());
}

TEST(MQTTMessageTest, qosLevelTest)
{
    std::stringstream ss;
    
    ss << acatl::mqtt::QoSLevel::AtMostOnce;
    EXPECT_EQ("at most once", ss.str());
    ss.str("");
    ss.clear();

    ss << acatl::mqtt::QoSLevel::AtLeastOnce;
    EXPECT_EQ("at least once", ss.str());
    ss.str("");
    ss.clear();
    
    ss << acatl::mqtt::QoSLevel::ExactlyOnce;
    EXPECT_EQ("exactly once", ss.str());
    ss.str("");
    ss.clear();
    
    ss << acatl::mqtt::QoSLevel::Error;
    EXPECT_EQ("error", ss.str());
}
