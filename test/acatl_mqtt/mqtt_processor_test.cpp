//
//  mqtt_processor_test.cpp
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

#include <acatl_mqtt/mqtt_processor.h>

namespace
{
    class NullSender : public acatl::mqtt::PacketSender
    {
    public:
        virtual void addSendPacket(acatl::mqtt::ControlPacket::Ptr packet)
        {
            _sendPackets.push_back(std::move(packet));
        }
        
        std::vector<acatl::mqtt::ControlPacket::Ptr> _sendPackets;
    };
    
    class MySubscriptionHandler : public acatl::mqtt::SubscriptionHandler
    {
    public:
        MySubscriptionHandler(acatl::mqtt::SubscriptionTreeManager& manager)
        : _manager(manager)
        {}
        
        void setSession(acatl::mqtt::Session::Ptr session)
        {
            _session = session;
        }
        
        virtual void addSubscriptions(const acatl::mqtt::TopicFilters& subscriptions)
        {
            acatl::mqtt::SubscriptionTreeManager::WritableTree writableTree = _manager.getWritableTree();
            std::for_each(subscriptions.cbegin(), subscriptions.cend(), [&](const acatl::mqtt::TopicFilter& filter) {
                std::error_code ec;
                acatl::mqtt::SubscriptionTree* tree = writableTree.tree();
                tree->addFilter(filter, _session, ec);
            });

        }
        
        virtual void removeSubscriptions(const acatl::mqtt::TopicFilters& subscriptions)
        {
        }
        
    private:
        acatl::mqtt::Session::Ptr _session;
        acatl::mqtt::SubscriptionTreeManager& _manager;
    };
}


class MQTTProcessorTest : public ::testing::Test
{
public:
    MQTTProcessorTest()
    : _mqttProcessor(_subscriptionTreeManager, _sessionManager)
    , _sender(new NullSender)
    {
        _mqttProcessor.setPacketSender(_sender);
    }

    void connect()
    {
        std::error_code ec;
        std::tuple<acatl::mqtt::ConnectionState, acatl::mqtt::ControlPacket::Ptr> result = _mqttProcessor.processPacket(makeConnectPacket(), ec);
        EXPECT_FALSE(ec);
        EXPECT_TRUE(std::get<1>(result));
    }
    
    acatl::mqtt::ConnectControlPacket::Ptr makeConnectPacket()
    {
        acatl::mqtt::ConnectControlPacket::Ptr ctrl = std::make_unique<acatl::mqtt::ConnectControlPacket>();
        ctrl->_header._flags = 0;
        ctrl->_header._length = 0;
        ctrl->_protocolLevel = 0x04;
        ctrl->_cleanSession = false;
        ctrl->_willFlag = false;
        ctrl->_willQoSLevel = acatl::mqtt::QoSLevel::AtMostOnce;
        ctrl->_willRetain = false;
        ctrl->_passwordFlag = false;
        ctrl->_userNameFlag = false;
        ctrl->_keepAlive = 60;
        ctrl->_clientId = "hutzli0815";

        return ctrl;
    }
    
    
    acatl::mqtt::SubscriptionTreeManager _subscriptionTreeManager;
    acatl::mqtt::SessionManager _sessionManager;
    acatl::mqtt::Processor _mqttProcessor;
    std::shared_ptr<NullSender> _sender;
};


TEST_F(MQTTProcessorTest, disconnect)
{
    connect();
    
    acatl::mqtt::ControlPacket::Ptr disconnect = std::make_unique<acatl::mqtt::DisconnectControlPacket>();
    
    std::error_code ec;
    std::tuple<acatl::mqtt::ConnectionState, acatl::mqtt::ControlPacket::Ptr> result = _mqttProcessor.processPacket(std::move(disconnect), ec);
    EXPECT_FALSE(std::get<1>(result));
    EXPECT_EQ(acatl::mqtt::ConnectionState::Close, std::get<0>(result));
    
    result = _mqttProcessor.processPacket(std::make_unique<acatl::mqtt::PingReqControlPacket>(), ec);
    EXPECT_FALSE(std::get<1>(result));
    EXPECT_TRUE(ec);
    EXPECT_EQ(acatl::mqtt::mqtt_error::not_connected, ec);
}

TEST_F(MQTTProcessorTest, wrongPacket)
{
    connect();
    
    acatl::mqtt::ControlPacket::Ptr req = std::make_unique<acatl::mqtt::PingRespControlPacket>();
    
    std::error_code ec;
    std::tuple<acatl::mqtt::ConnectionState, acatl::mqtt::ControlPacket::Ptr> result = _mqttProcessor.processPacket(std::move(req), ec);
    EXPECT_FALSE(std::get<1>(result));
    EXPECT_TRUE(ec);
    EXPECT_EQ(acatl::mqtt::mqtt_error::control_packet_not_allowed, ec);
}

TEST_F(MQTTProcessorTest, duplConnect)
{
    connect();
    
    acatl::mqtt::ControlPacket::Ptr req = makeConnectPacket();
    
    std::error_code ec;
    std::tuple<acatl::mqtt::ConnectionState, acatl::mqtt::ControlPacket::Ptr> result = _mqttProcessor.processPacket(std::move(req), ec);
    EXPECT_FALSE(std::get<1>(result));
    EXPECT_TRUE(ec);
    EXPECT_EQ(acatl::mqtt::mqtt_error::duplicate_connect_protocol_violation, ec);
}

TEST_F(MQTTProcessorTest, connect)
{
    acatl::mqtt::ControlPacket::Ptr req = makeConnectPacket();
    
    std::error_code ec;
    std::tuple<acatl::mqtt::ConnectionState, acatl::mqtt::ControlPacket::Ptr> result = _mqttProcessor.processPacket(std::move(req), ec);
    EXPECT_TRUE(std::get<1>(result));
    EXPECT_FALSE(ec);
    EXPECT_EQ(acatl::mqtt::ControlPacketType::Connack, std::get<1>(result)->_header._controlPacketType);
}

TEST_F(MQTTProcessorTest, subscribe)
{
    connect();
    
    acatl::mqtt::SubscribeControlPacket::Ptr req = std::make_unique<acatl::mqtt::SubscribeControlPacket>();
    req->_packetIdentifier = 15;
    req->_topicFilters.push_back(acatl::mqtt::TopicFilter("check", acatl::mqtt::QoSLevel::AtMostOnce));
    req->_topicFilters.push_back(acatl::mqtt::TopicFilter("hutzli", acatl::mqtt::QoSLevel::AtLeastOnce));
    req->_topicFilters.push_back(acatl::mqtt::TopicFilter("/foo/bar/#", acatl::mqtt::QoSLevel::AtMostOnce));
    
    std::error_code ec;
    std::tuple<acatl::mqtt::ConnectionState, acatl::mqtt::ControlPacket::Ptr> result = _mqttProcessor.processPacket(std::move(req), ec);
    EXPECT_TRUE(std::get<1>(result));
    EXPECT_FALSE(ec);
    EXPECT_EQ(acatl::mqtt::ControlPacketType::Suback, std::get<1>(result)->_header._controlPacketType);
}

TEST_F(MQTTProcessorTest, ping)
{
    connect();
    
    acatl::mqtt::ControlPacket::Ptr req = std::make_unique<acatl::mqtt::PingReqControlPacket>();
    
    std::error_code ec;
    std::tuple<acatl::mqtt::ConnectionState, acatl::mqtt::ControlPacket::Ptr> result = _mqttProcessor.processPacket(std::move(req), ec);
    EXPECT_TRUE(std::get<1>(result));
    EXPECT_FALSE(ec);
    EXPECT_EQ(acatl::mqtt::ControlPacketType::Pingresp, std::get<1>(result)->_header._controlPacketType);
}

TEST_F(MQTTProcessorTest, subscribeAndPublish)
{
    std::error_code ec;
    std::shared_ptr<NullSender> sender(new NullSender);
    MySubscriptionHandler handler(_subscriptionTreeManager);
    
    acatl::mqtt::Session::Ptr session = _sessionManager.getSession("0815-session", sender, handler, ec);
    handler.setSession(session);
    acatl::mqtt::TopicFilters filters;
    filters.push_back(acatl::mqtt::TopicFilter("sheldon/bazinga", acatl::mqtt::QoSLevel::AtMostOnce));
    session->addSubscriptions(filters);
    
    connect();
    
    acatl::mqtt::SubscribeControlPacket::Ptr req = std::make_unique<acatl::mqtt::SubscribeControlPacket>();
    req->_packetIdentifier = 15;
    req->_topicFilters.push_back(acatl::mqtt::TopicFilter("check", acatl::mqtt::QoSLevel::AtMostOnce));
    
    std::tuple<acatl::mqtt::ConnectionState, acatl::mqtt::ControlPacket::Ptr> result = _mqttProcessor.processPacket(std::move(req), ec);
    EXPECT_TRUE(std::get<1>(result));
    EXPECT_FALSE(ec);
    EXPECT_EQ(acatl::mqtt::ControlPacketType::Suback, std::get<1>(result)->_header._controlPacketType);
    
    acatl::mqtt::PublishControlPacket::Ptr pub = std::make_unique<acatl::mqtt::PublishControlPacket>();
    pub->_packetIdentifier = 53263;
    pub->_topicName = "sheldon/bazinga";
    pub->_payload = { 'c', 'o', 'o', 'l', '!' };
    
    result = _mqttProcessor.processPacket(std::move(pub), ec);
    // no control package, as there is no response for QoS 0 publish requests
    EXPECT_TRUE(!std::get<1>(result));
    EXPECT_FALSE(ec);
    EXPECT_EQ(1u, sender->_sendPackets.size());
    const acatl::mqtt::PublishControlPacket* p = static_cast<const acatl::mqtt::PublishControlPacket*>(sender->_sendPackets[0].get());
    EXPECT_EQ("cool!", std::string(reinterpret_cast<const char*>(&p->_payload[0]), p->_payload.size()));
}
