//
//  mqtt_subscription_tree_test.cpp
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

#include <acatl_mqtt/mqtt_subscription_tree.h>


class MQTTSubscriptionTreeTest : public acatl::mqtt::SubscriptionHandler, public ::testing::Test
{
public:
    MQTTSubscriptionTreeTest()
    : _session(new acatl::mqtt::Session("session", *this))
    {
    }
    
    virtual void addSubscriptions(const acatl::mqtt::TopicFilters& subscriptions)
    {}
    
    virtual void removeSubscriptions(const acatl::mqtt::TopicFilters& subscriptions)
    {}
    
    void prepareSubscriptions()
    {
        std::error_code ec;
        acatl::mqtt::TopicFilter filter = { "sport/tennis/#" };
        
        _subscriptions.addFilter(filter, _session, ec);
        
        filter = { "sport/tennis/+/player1" };
        _subscriptions.addFilter(filter, _session, ec);
        
        filter = { "/system/check/hardware/#" };
        _subscriptions.addFilter(filter, _session, ec);
        
        filter = { "sport/tennis/wimbledom/player1" };
        _subscriptions.addFilter(filter, _session, ec);
        
        filter = { "sport/tennis/wimbledom/player2" };
        _subscriptions.addFilter(filter, _session, ec);
        
        filter = { "sport/soccer/bundesliga/teams/bayern münchen" };
        _subscriptions.addFilter(filter, _session, ec);
        
        filter = { "sport/soccer/bundesliga/teams/#" };
        _subscriptions.addFilter(filter, _session, ec);
        
        filter = { "sport/tennis/davis cup/player1" };
        _subscriptions.addFilter(filter, _session, ec);
        
        filter = { "sport/tennis/+/player1" };
        _subscriptions.addFilter(filter, _session, ec);
        
        filter = { "/system/check/software/#" };
        _subscriptions.addFilter(filter, _session, ec);
        
        filter = { "/#" };
        _subscriptions.addFilter(filter, _session, ec);
        
        EXPECT_FALSE(ec);
    }
    
protected:
    acatl::mqtt::SubscriptionTree _subscriptions;
    acatl::mqtt::Session::Ptr _session;
};


TEST_F(MQTTSubscriptionTreeTest, treeCreation)
{
    prepareSubscriptions();
    
    std::stringstream ss;
    size_t indent = 0;
    _subscriptions.dump(ss, indent);
    
    std::string dump = R"(""
  "#" -> session,
  "system"
    "check"
      "software"
        "#" -> session,
      "hardware"
        "#" -> session,
"sport"
  "soccer"
    "bundesliga"
      "teams"
        "#" -> session,
        "bayern münchen" -> session,
  "tennis"
    "davis cup"
      "player1" -> session,
    "wimbledom"
      "player2" -> session,
      "player1" -> session,
    "+"
      "player1" -> session,
    "#" -> session,
)";

    EXPECT_EQ(dump, ss.str());
}

TEST_F(MQTTSubscriptionTreeTest, simpleMatching)
{
    acatl::mqtt::Sessions sessions;
    std::error_code ec;
    
    {
        acatl::mqtt::TopicFilter filter = { "sport/tennis/wimbledon/player1" };
        acatl::mqtt::SubscriptionTree subscriptions;
        subscriptions.addFilter(filter, _session, ec);
        
        acatl::mqtt::TopicName topic("sport/tennis/wimbledon/player1");
        EXPECT_TRUE(subscriptions.match(topic, sessions, ec));
    }
    {
        acatl::mqtt::TopicFilter filter = { "sport/tennis/+/player1" };
        acatl::mqtt::SubscriptionTree subscriptions;
        subscriptions.addFilter(filter, _session, ec);
        
        acatl::mqtt::TopicName topic("sport/tennis/wimbledon/player1");
        EXPECT_TRUE(subscriptions.match(topic, sessions, ec));
    }
    {
        acatl::mqtt::TopicFilter filter = { "sport/tennis/#" };
        acatl::mqtt::SubscriptionTree subscriptions;
        subscriptions.addFilter(filter, _session, ec);
        
        acatl::mqtt::TopicName topic("sport/tennis/wimbledon/player1");
        EXPECT_TRUE(subscriptions.match(topic, sessions, ec));
        
        topic = "sport/tennis/davis cup/player1";
        EXPECT_TRUE(subscriptions.match(topic, sessions, ec));

        topic = "sport/tennis/davis cup/player1/ranking";
        EXPECT_TRUE(subscriptions.match(topic, sessions, ec));
    }
}

TEST_F(MQTTSubscriptionTreeTest, simpleNonMatching)
{
    acatl::mqtt::Sessions sessions;
    std::error_code ec;
    
    {
        acatl::mqtt::TopicFilter filter = { "sport/tennis/wimbledon" };
        acatl::mqtt::SubscriptionTree subscriptions;
        subscriptions.addFilter(filter, _session, ec);
        
        acatl::mqtt::TopicName topic("sport/tennis/wimbledon/player1");
        EXPECT_FALSE(subscriptions.match(topic, sessions, ec));
    }
    {
        acatl::mqtt::TopicFilter filter = { "sport/tennis/+/player1" };
        acatl::mqtt::SubscriptionTree subscriptions;
        subscriptions.addFilter(filter, _session, ec);
        
        acatl::mqtt::TopicName topic("sport/soccer/bundesliga");
        EXPECT_FALSE(subscriptions.match(topic, sessions, ec));
    }
    {
        acatl::mqtt::TopicFilter filter = { "sport/tennis/#" };
        acatl::mqtt::SubscriptionTree subscriptions;
        subscriptions.addFilter(filter, _session, ec);
        
        acatl::mqtt::TopicName topic("sport/soccer/bundesliga");
        EXPECT_FALSE(subscriptions.match(topic, sessions, ec));
    }
    {
        acatl::mqtt::TopicFilter filter = { "sport/tennis/+" };
        acatl::mqtt::SubscriptionTree subscriptions;
        subscriptions.addFilter(filter, _session, ec);
        
        acatl::mqtt::TopicName topic("sport/tennis/davis cup/player1");
        EXPECT_FALSE(subscriptions.match(topic, sessions, ec));
    }
    {
        acatl::mqtt::TopicFilter filter = { "sport/tennis/+/player1" };
        acatl::mqtt::SubscriptionTree subscriptions;
        subscriptions.addFilter(filter, _session, ec);
        
        acatl::mqtt::TopicName topic("sport/tennis/wimbledon/ranking/player1");
        EXPECT_FALSE(subscriptions.match(topic, sessions, ec));
    }
}

TEST_F(MQTTSubscriptionTreeTest, sessionMatching)
{
    std::error_code ec;
    acatl::mqtt::SubscriptionTree subscriptions;
    
    acatl::mqtt::Session::Ptr session1(new acatl::mqtt::Session("session1", *this));
    acatl::mqtt::Session::Ptr session2(new acatl::mqtt::Session("session2", *this));
    acatl::mqtt::Session::Ptr session3(new acatl::mqtt::Session("session3", *this));
    
    acatl::mqtt::TopicFilter filter = { "sport/tennis/#" };
    subscriptions.addFilter(filter, session1, ec);
    
    filter = { "sport/tennis/+/player1" };
    subscriptions.addFilter(filter, session2, ec);
    
    filter = { "sport/tennis/wimbledon/player1" };
    subscriptions.addFilter(filter, session3, ec);

    filter = { "sport/tennis/wimbledon/player2" };
    subscriptions.addFilter(filter, session1, ec);
    subscriptions.addFilter(filter, session3, ec);
    
    {
        acatl::mqtt::TopicName topic("sport/tennis/wimbledon/player1");
        acatl::mqtt::Sessions sessions;
        EXPECT_TRUE(subscriptions.match(topic, sessions, ec));
        EXPECT_EQ(3u, sessions.size());
        EXPECT_TRUE(sessions.find(session1) != sessions.end());
        EXPECT_TRUE(sessions.find(session2) != sessions.end());
        EXPECT_TRUE(sessions.find(session3) != sessions.end());
    }
    {
        acatl::mqtt::TopicName topic("sport/tennis/wimbledon/player2");
        acatl::mqtt::Sessions sessions;
        EXPECT_TRUE(subscriptions.match(topic, sessions, ec));
        EXPECT_EQ(2u, sessions.size());
        EXPECT_TRUE(sessions.find(session1) != sessions.end());
        EXPECT_TRUE(sessions.find(session3) != sessions.end());
    }
}
