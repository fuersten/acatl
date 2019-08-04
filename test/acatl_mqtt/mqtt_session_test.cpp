//
//  mqtt_session_test.cpp
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

#include <acatl_mqtt/mqtt_session_manager.h>


namespace
{
    class NullSender : public acatl::mqtt::PacketSender
    {
    public:
        virtual void addSendPacket(acatl::mqtt::ControlPacket::Ptr packet)
        {
        }
    };
}


class MQTTSessionTest : public acatl::mqtt::SubscriptionHandler, public ::testing::Test
{
public:
    MQTTSessionTest()
    : _sender(new NullSender)
    {
    }
    
    virtual void addSubscriptions(const acatl::mqtt::TopicFilters& subscriptions)
    {}
    
    virtual void removeSubscriptions(const acatl::mqtt::TopicFilters& subscriptions)
    {}

protected:
    acatl::mqtt::SessionManager _sessionManager;
    std::shared_ptr<NullSender> _sender;
};


TEST_F(MQTTSessionTest, createSession)
{
    acatl::mqtt::SessionManager manager;
    
    std::error_code ec;
    acatl::mqtt::Session::Ptr session = manager.getSession("hutzli0815", _sender, *this, ec);
    EXPECT_TRUE(session.get() != nullptr);
    EXPECT_EQ(1u, manager.count());
    EXPECT_EQ(_sender, session->currentSender());
    
    EXPECT_TRUE(manager.returnSession(session, ec));
    EXPECT_FALSE(ec);
    EXPECT_EQ(1u, manager.count());
    
    EXPECT_NE(_sender, session->currentSender());
    EXPECT_FALSE(session->currentSender());
}

TEST_F(MQTTSessionTest, duplSession)
{
    acatl::mqtt::SessionManager manager;
    
    std::error_code ec;
    acatl::mqtt::Session::Ptr session = manager.getSession("hutzli0815", _sender, *this, ec);
    EXPECT_TRUE(session.get() != nullptr);
    EXPECT_FALSE(ec);
    
    acatl::mqtt::Session::Ptr nextSession = manager.getSession("hutzli0815", _sender, *this, ec);
    EXPECT_TRUE(nextSession.get() == nullptr);
    EXPECT_TRUE(ec);
    
    EXPECT_TRUE(manager.returnSession(session, ec));
    EXPECT_FALSE(ec);
    
    nextSession = manager.getSession("hutzli0815", _sender, *this, ec);
    EXPECT_TRUE(nextSession != nullptr);
    EXPECT_FALSE(ec);
}

TEST_F(MQTTSessionTest, removeSession)
{
    acatl::mqtt::SessionManager manager;
    
    std::error_code ec;
    acatl::mqtt::Session::Ptr session = manager.getSession("hutzli0815", _sender, *this, ec);
    EXPECT_TRUE(session.get() != nullptr);
    EXPECT_EQ(1u, manager.count());
    
    EXPECT_FALSE(manager.removeSession("hutzli0815", ec));
    EXPECT_TRUE(ec);
    EXPECT_EQ(acatl::mqtt::mqtt_error::session_in_use, ec);
    EXPECT_EQ(1u, manager.count());
    
    EXPECT_TRUE(manager.returnSession(session, ec));
    
    EXPECT_TRUE(manager.removeSession("hutzli0815", ec));
    EXPECT_FALSE(ec);
    EXPECT_EQ(0u, manager.count());
    
    EXPECT_FALSE(manager.removeSession("hutzli0815", ec));
    EXPECT_TRUE(ec);
    EXPECT_EQ(acatl::mqtt::mqtt_error::session_not_found, ec);
}

