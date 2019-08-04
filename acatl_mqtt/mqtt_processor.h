//
//  mqtt_processor.h
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

#ifndef acatl_mqtt_processor_h
#define acatl_mqtt_processor_h

#include <acatl/logging.h>

#include <acatl_mqtt/mqtt_packet_sender.h>
#include <acatl_mqtt/mqtt_session_manager.h>
#include <acatl_mqtt/mqtt_subscription_tree_manager.h>
#include <acatl_mqtt/mqtt_types.h>


namespace acatl
{
    namespace mqtt
    {
        
        class Processor : SubscriptionHandler
        {
        public:
            Processor(SubscriptionTreeManager& subcriptionTreeManager, SessionManager& sessionManager)
            : _status(Status::None)
            , _currentSession(nullptr)
            , _subcriptionTreeManager(subcriptionTreeManager)
            , _sessionManager(sessionManager)
            {}
            
            ~Processor()
            {
                std::error_code ec;
                _sessionManager.returnSession(_currentSession, ec);
            }
            
            // TODO this is a bit bizar. The addSubscriptions will be called by the Session, but we use here the
            //      _currentSession, which should always be the calling Session. Would be better to supply the
            //      Session smart pointer in the first place
            virtual void addSubscriptions(const TopicFilters& subscriptions)
            {
                acatl::mqtt::SubscriptionTreeManager::WritableTree writableTree = _subcriptionTreeManager.getWritableTree();
                std::for_each(subscriptions.cbegin(), subscriptions.cend(), [this,&writableTree](const TopicFilter& filter) {
                    std::error_code ec;
                    writableTree.tree()->addFilter(filter, _currentSession, ec);
                });
            }
            
            virtual void removeSubscriptions(const TopicFilters& subscriptions)
            {
            }

            void setPacketSender(PacketSender::WeakPtr packetSender)
            {
                _packetSender = packetSender;
            }
            
            std::tuple<ConnectionState, ControlPacket::Ptr> processPacket(ControlPacket::Ptr packet, std::error_code& ec)
            {
                if(_packetSender.expired()) {
                    ec = mqtt_error::no_packet_sender;
                    return std::make_tuple(ConnectionState::Close, ControlPacket::Ptr());
                }
                
                ACATL_CLASSLOG(Processor, 2, "Received " << packet->_header._controlPacketType << " packet");
                
                switch(_status) {
                    case Status::None:
                        if(packet->_header._controlPacketType != ControlPacketType::Connect) {
                            ec = mqtt_error::not_connected;
                            return std::make_tuple(ConnectionState::Close, ControlPacket::Ptr());
                        }
                        break;
                    case Status::Connected:
                        if(packet->_header._controlPacketType == ControlPacketType::Connect) {
                            ec = mqtt_error::duplicate_connect_protocol_violation;
                            return std::make_tuple(ConnectionState::Close, ControlPacket::Ptr());
                        }
                        break;
                    case Status::Disconnected:
                        ec = mqtt_error::not_connected;
                        return std::make_tuple(ConnectionState::Close, ControlPacket::Ptr());
                }
                
                switch(packet->_header._controlPacketType) {
                    case ControlPacketType::Connack:
                    case ControlPacketType::Pingresp:
                    case ControlPacketType::Suback:
                    case ControlPacketType::Unsuback:
                        ec = mqtt_error::control_packet_not_allowed;
                        break;
                    case ControlPacketType::Connect:
                        _status = Status::Connected;
                        return doProcess(dynamic_cast<const ConnectControlPacket&>(*packet), ec);
                    case ControlPacketType::Pingreq:
                        return doProcess(dynamic_cast<const PingReqControlPacket&>(*packet), ec);
                    case ControlPacketType::Disconnect:
                        _status = Status::Disconnected;
                        return doProcess(dynamic_cast<const DisconnectControlPacket&>(*packet), ec);
                    case ControlPacketType::Subscribe:
                        return doProcess(dynamic_cast<const SubscribeControlPacket&>(*packet), ec);
                    case ControlPacketType::Publish:
                        return doProcess(dynamic_cast<const PublishControlPacket&>(*packet), ec);
                    case ControlPacketType::Puback:
                    case ControlPacketType::Pubrec:
                    case ControlPacketType::Pubcomp:
                    case ControlPacketType::Pubrel:
                    case ControlPacketType::Unsubscribe:
                        ec = mqtt_error::feature_not_implemented;
                        break;
                    default:
                        ec = mqtt_error::invalid_control_packet_type;
                        return std::make_tuple(ConnectionState::Close, ControlPacket::Ptr());
                }
                
                return std::make_tuple(ConnectionState::Close, ControlPacket::Ptr());
            }
            
        private:
            enum class Status
            {
                None,
                Connected,
                Disconnected
            };
            
            std::tuple<ConnectionState, ControlPacket::Ptr> doProcess(const ConnectControlPacket& connect, std::error_code& ec)
            {
                ACATL_CLASSLOG(Processor, 3, "Connect length is " << connect._header._length);
                ACATL_CLASSLOG(Processor, 3, "Connect protocol level " << std::to_string(connect._protocolLevel));
                ACATL_CLASSLOG(Processor, 3, "Connect keep alive " << connect._keepAlive << " seconds");
                ACATL_CLASSLOG(Processor, 1, "Connect client ID " << connect._clientId);

                _currentSession = _sessionManager.getSession(connect._clientId, _packetSender, *this, ec);
                
                ConnAckControlPacket::Ptr connack = std::make_unique<ConnAckControlPacket>();
                connack->_connectAcknowledgeFlag = ConnectAcknowledgeFlags::None;
                connack->_connectReturnCode = ConnectReturnCode::ConnectionAccepted;
                return std::make_tuple(ConnectionState::Keep, std::move(connack));
            }
            
            std::tuple<ConnectionState, ControlPacket::Ptr> doProcess(const PublishControlPacket& pub, std::error_code& ec)
            {
                ACATL_CLASSLOG(Processor, 2, "Published on topic '" << pub._topicName._name << "'");
                
                SubscriptionTree::ConstPtr tree = _subcriptionTreeManager.getCurrentSubscriptionTree();
                Sessions sessions;
                if(tree->match(pub._topicName, sessions, ec)) {
                    ACATL_CLASSLOG(Processor, 2, "Found a match for topic '" << pub._topicName._name << "'");
                    std::for_each(sessions.begin(), sessions.end(), [&pub](Session::Ptr session) {
                        PacketSender::Ptr sender = session->currentSender();
                        if(sender) {
                            ACATL_CLASSLOG(Processor, 2, "Sending for session '" << session->clientId() << "'");
                            sender->addSendPacket(PublishControlPacket::Ptr(new PublishControlPacket(pub)));
                        }
                    });
                }
                
                return std::make_tuple(ConnectionState::Keep, ControlPacket::Ptr());
            }
            
            std::tuple<ConnectionState, ControlPacket::Ptr> doProcess(const SubscribeControlPacket& subs, std::error_code& ec)
            {
                _currentSession->addSubscriptions(subs._topicFilters);
                
                SubAckControlPacket::Ptr suback = std::make_unique<SubAckControlPacket>();
                suback->_packetIdentifier = subs._packetIdentifier;
                for(const auto& filter : subs._topicFilters) {
                    suback->_qosLevels.push_back(filter._qos);
                }
                
                return std::make_tuple(ConnectionState::Keep, std::move(suback));
            }
            
            std::tuple<ConnectionState, ControlPacket::Ptr> doProcess(const PingReqControlPacket& pingreq, std::error_code& ec)
            {
                return std::make_tuple(ConnectionState::Keep, std::make_unique<PingRespControlPacket>());
            }
            
            std::tuple<ConnectionState, ControlPacket::Ptr> doProcess(const DisconnectControlPacket& disconnect, std::error_code& ec)
            {
                std::error_code errc;
                _sessionManager.returnSession(_currentSession, errc);
                _currentSession.reset();

                return std::make_tuple(ConnectionState::Close, ControlPacket::Ptr());
            }
            
            Status _status;
            Session::Ptr _currentSession;
            SubscriptionTreeManager& _subcriptionTreeManager;
            SessionManager& _sessionManager;
            PacketSender::WeakPtr _packetSender;
        };
        
    }
}

#endif
