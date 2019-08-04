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

#include <acatl_mqtt/mqtt_subscription_tree_manager.h>


namespace
{
    class MySubscriptionHandler : public acatl::mqtt::SubscriptionHandler
    {
    public:
        virtual void addSubscriptions(const acatl::mqtt::TopicFilters& subscriptions)
        {}
        
        virtual void removeSubscriptions(const acatl::mqtt::TopicFilters& subscriptions)
        {}
    };
}

TEST(MQTTSubscriptionTreeManagerTest, treeSet)
{
    MySubscriptionHandler handler;
    acatl::mqtt::SubscriptionTreeManager manager;
    
    acatl::mqtt::SubscriptionTree::ConstPtr tree = manager.getCurrentSubscriptionTree();
    
    acatl::mqtt::Sessions sessions;
    std::error_code ec;
    EXPECT_FALSE(tree->match({ "sport/tennis/wimbledon/player1" }, sessions, ec));
    
    acatl::mqtt::Session::Ptr session(new acatl::mqtt::Session("hutzli", handler));
    
    {
        acatl::mqtt::SubscriptionTreeManager::WritableTree writableTree = manager.getWritableTree();
        writableTree.tree()->addFilter({ "sport/tennis/#" }, session, ec);
        
        tree = manager.getCurrentSubscriptionTree();
        EXPECT_FALSE(tree->match({ "sport/tennis/wimbledon/player1" }, sessions, ec));
    }
    
    tree = manager.getCurrentSubscriptionTree();
    EXPECT_TRUE(tree->match({ "sport/tennis/wimbledon/player1" }, sessions, ec));
    EXPECT_EQ(1u, sessions.size());
    EXPECT_TRUE(sessions.find(session) != sessions.end());
}
