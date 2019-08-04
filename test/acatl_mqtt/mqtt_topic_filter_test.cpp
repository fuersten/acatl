//
//  mqtt_topic_filter_test.cpp
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

#include <acatl_mqtt/mqtt_topic.h>


TEST(MQTTTopicFilterTest, validatePositive)
{
    std::error_code ec;
    acatl::mqtt::TopicFilter filter = {"a/b", acatl::mqtt::QoSLevel::AtMostOnce};
    
    EXPECT_TRUE(filter.validate(ec));
    
    filter = {"sport", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_TRUE(filter.validate(ec));
    
    filter = {"/sport", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_TRUE(filter.validate(ec));

    filter = {"sport/tennis", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_TRUE(filter.validate(ec));
    
    filter = {"/sport/tennis", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_TRUE(filter.validate(ec));

    filter = {"+", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_TRUE(filter.validate(ec));
    
    filter = {"sport/+/player1", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_TRUE(filter.validate(ec));
    
    filter = {"/+", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_TRUE(filter.validate(ec));
    
    filter = {"+/+", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_TRUE(filter.validate(ec));

    filter = {"sport/#", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_TRUE(filter.validate(ec));
    
    filter = {"#", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_TRUE(filter.validate(ec));
    
    filter = {"sport/tennis/#", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_TRUE(filter.validate(ec));
    
    filter = {"+/sport/#", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_TRUE(filter.validate(ec));
    
    filter = {"/+/sport/#", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_TRUE(filter.validate(ec));
    
    filter = {"/#", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_TRUE(filter.validate(ec));
}

TEST(MQTTTopicFilterTest, validateNegative)
{
    std::error_code ec;
    acatl::mqtt::TopicFilter filter = {"sport+", acatl::mqtt::QoSLevel::AtMostOnce};
    
    EXPECT_FALSE(filter.validate(ec));
    EXPECT_EQ(acatl::mqtt::mqtt_error::invalid_topic_filter, ec);
    
    filter = {"+sport", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_FALSE(filter.validate(ec));
    
    filter = {"sp+ort", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_FALSE(filter.validate(ec));
    
    filter = {"/+sport", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_FALSE(filter.validate(ec));
    
    filter = {"sport+/", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_FALSE(filter.validate(ec));
    
    filter = {"sport/tennis#", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_FALSE(filter.validate(ec));
    
    filter = {"sport/tennis/#/ranking", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_FALSE(filter.validate(ec));
    
    filter = {"sport/tennis#/ranking", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_FALSE(filter.validate(ec));
    
    filter = {"#test", acatl::mqtt::QoSLevel::AtMostOnce};
    EXPECT_FALSE(filter.validate(ec));
}

TEST(MQTTTopicFilterTest, iterator)
{
    acatl::mqtt::TopicFilter filter = {"sport/tennis", acatl::mqtt::QoSLevel::AtMostOnce};
    
    std::vector<std::string> topics;
    
    for(const auto& val : filter) {
        topics.push_back(val);
    }
    ASSERT_EQ(2u, topics.size());
    EXPECT_EQ("sport", topics[0]);
    EXPECT_EQ("tennis", topics[1]);
    
    topics.clear();
    filter = {"+", acatl::mqtt::QoSLevel::AtMostOnce};
    std::copy(filter.begin(), filter.end(), std::back_inserter(topics));
    ASSERT_EQ(1u, topics.size());
    EXPECT_EQ("+", topics[0]);

    topics.clear();
    filter = {"/#", acatl::mqtt::QoSLevel::AtMostOnce};
    std::copy(filter.begin(), filter.end(), std::back_inserter(topics));
    ASSERT_EQ(2u, topics.size());
    EXPECT_EQ("", topics[0]);
    EXPECT_EQ("#", topics[1]);

    topics.clear();
    filter = {"sport/+/player1", acatl::mqtt::QoSLevel::AtMostOnce};
    std::copy(filter.begin(), filter.end(), std::back_inserter(topics));
    ASSERT_EQ(3u, topics.size());
    EXPECT_EQ("sport", topics[0]);
    EXPECT_EQ("+", topics[1]);
    EXPECT_EQ("player1", topics[2]);

    topics.clear();
    filter = {"sport/tennis/#", acatl::mqtt::QoSLevel::AtMostOnce};
    std::copy(filter.begin(), filter.end(), std::back_inserter(topics));
    ASSERT_EQ(3u, topics.size());
    EXPECT_EQ("sport", topics[0]);
    EXPECT_EQ("tennis", topics[1]);
    EXPECT_EQ("#", topics[2]);
    
    topics.clear();
    filter = {"/sport/tennis", acatl::mqtt::QoSLevel::AtMostOnce};
    std::copy(filter.begin(), filter.end(), std::back_inserter(topics));
    ASSERT_EQ(3u, topics.size());
    EXPECT_EQ("", topics[0]);
    EXPECT_EQ("sport", topics[1]);
    EXPECT_EQ("tennis", topics[2]);

    topics.clear();
    filter = {"/sport/tennis/", acatl::mqtt::QoSLevel::AtMostOnce};
    std::copy(filter.begin(), filter.end(), std::back_inserter(topics));
    ASSERT_EQ(3u, topics.size());
    EXPECT_EQ("", topics[0]);
    EXPECT_EQ("sport", topics[1]);
    EXPECT_EQ("tennis", topics[2]);

    topics.clear();
    filter = {"+/sport/#", acatl::mqtt::QoSLevel::AtMostOnce};
    std::copy(filter.begin(), filter.end(), std::back_inserter(topics));
    ASSERT_EQ(3u, topics.size());
    EXPECT_EQ("+", topics[0]);
    EXPECT_EQ("sport", topics[1]);
    EXPECT_EQ("#", topics[2]);
}

TEST(MQTTTopicFilterTest, construct)
{
    acatl::mqtt::TopicFilter filter1 = {"a/b", acatl::mqtt::QoSLevel::AtMostOnce};
    acatl::mqtt::TopicFilter filter2 = {"c/d", acatl::mqtt::QoSLevel::AtMostOnce};
    acatl::mqtt::TopicFilter filter3 = {"e/f", acatl::mqtt::QoSLevel::AtMostOnce};
    
    EXPECT_NE(filter1, filter2);
    EXPECT_EQ(filter1, filter1);
    EXPECT_LT(filter1, filter2);
    
    filter1 = {"a/b", acatl::mqtt::QoSLevel::AtLeastOnce};
    EXPECT_LT(filter1, filter2);
}

TEST(MQTTTopicFilterTest, stream)
{
    acatl::mqtt::TopicFilter filter1 = {"a/b", acatl::mqtt::QoSLevel::AtMostOnce};
    acatl::mqtt::TopicFilter filter2 = {"c/d", acatl::mqtt::QoSLevel::AtLeastOnce};
    
    std::stringstream ss;
    ss << filter1;
    EXPECT_EQ("\"a/b\", at most once", ss.str());
    
    ss.str("");
    ss.clear();
    ss << filter2;
    EXPECT_EQ("\"c/d\", at least once", ss.str());
}

TEST(MQTTTopicFilterTest, difference)
{
    acatl::mqtt::TopicFilters toAddTopics({
        {"a/b", acatl::mqtt::QoSLevel::AtLeastOnce},
        {"c/d", acatl::mqtt::QoSLevel::AtMostOnce},
        {"e/f", acatl::mqtt::QoSLevel::AtLeastOnce}
    });
    acatl::mqtt::TopicFilters currentTopics({
        {"c/d", acatl::mqtt::QoSLevel::AtMostOnce}
    });
    
    acatl::mqtt::TopicFilters result = acatl::mqtt::TopicFilterHelper::findDifference(toAddTopics, currentTopics);
    EXPECT_EQ(2u, result.size());
    EXPECT_EQ(acatl::mqtt::TopicFilter("a/b", acatl::mqtt::QoSLevel::AtLeastOnce), result[0]);
    EXPECT_EQ(acatl::mqtt::TopicFilter("e/f", acatl::mqtt::QoSLevel::AtLeastOnce), result[1]);
}

TEST(MQTTTopicTest, validate)
{
    std::error_code ec;
    acatl::mqtt::TopicName topic("check");
    
    EXPECT_TRUE(topic.validate(ec));
    
    topic = "/do/check/my/topic/#";
    EXPECT_FALSE(topic.validate(ec));
    EXPECT_EQ(acatl::mqtt::mqtt_error::invalid_wildcard_in_topic, ec);

    topic = "/do/check/+/topic/id";
    EXPECT_FALSE(topic.validate(ec));
    EXPECT_EQ(acatl::mqtt::mqtt_error::invalid_wildcard_in_topic, ec);

    topic = "/do/check/topic/id";
    EXPECT_TRUE(topic.validate(ec));
}

TEST(MQTTTopicTest, print)
{
    std::stringstream ss;
    acatl::mqtt::TopicName topic("check/the/topic");
    ss << topic;
    EXPECT_EQ("\"check/the/topic\"", ss.str());
}

TEST(MQTTTopicTest, iterator)
{
    acatl::mqtt::TopicName topic("check/the/topic");
    
    std::vector<std::string> topics;
    
    for(const auto& val : topic) {
        topics.push_back(val);
    }
    ASSERT_EQ(3u, topics.size());
    EXPECT_EQ("check", topics[0]);
    EXPECT_EQ("the", topics[1]);
    EXPECT_EQ("topic", topics[2]);

    topics.clear();
    topic = "/a/cool/topic/";
    std::copy(topic.begin(), topic.end(), std::back_inserter(topics));
    ASSERT_EQ(4u, topics.size());
    EXPECT_EQ("", topics[0]);
    EXPECT_EQ("a", topics[1]);
    EXPECT_EQ("cool", topics[2]);
    EXPECT_EQ("topic", topics[3]);
}
