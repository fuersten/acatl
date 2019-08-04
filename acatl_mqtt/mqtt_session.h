//
//  mqtt_session_manager.h
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

#ifndef acatl_mqtt_session_h
#define acatl_mqtt_session_h

#include <acatl_mqtt/mqtt_error.h>
#include <acatl_mqtt/mqtt_packet_sender.h>
#include <acatl_mqtt/mqtt_subscription_handler.h>

#include <mutex>


namespace acatl
{
    namespace mqtt
    {
        
        class Session
        {
        public:
            typedef std::shared_ptr<Session> Ptr;
            
            Session(const std::string& clientId, SubscriptionHandler& subscriptionHandler)
            : _clientId(clientId)
            , _subscriptionHandler(subscriptionHandler)
            {}
            
            Session(Session&& rhs)
            : _clientId(std::move(rhs._clientId))
            , _subscriptionHandler(rhs._subscriptionHandler)
            , _sender(std::move(rhs._sender))
            , _subscriptions(std::move(rhs._subscriptions))
            {
            }
            
            const std::string& clientId() const
            {
                return _clientId;
            }
            
            void setSender(PacketSender::WeakPtr sender)
            {
                _sender = sender;
            }
            
            PacketSender::Ptr currentSender()
            {
                return _sender.lock();
            }
            
            void addSubscriptions(const TopicFilters& subscriptions)
            {
                std::unique_lock<std::mutex> guard(_subscriptionMutex);
                
                acatl::mqtt::TopicFilters result = acatl::mqtt::TopicFilterHelper::findDifference(subscriptions, _subscriptions);
                _subscriptions.insert(std::end(_subscriptions), std::begin(result), std::end(result));
                
                _subscriptionHandler.addSubscriptions(result);
            }
            
        private:
            std::string _clientId;
            SubscriptionHandler& _subscriptionHandler;
            PacketSender::WeakPtr _sender;
            std::mutex _subscriptionMutex;
            TopicFilters _subscriptions;
        };

    }
}

#endif 
