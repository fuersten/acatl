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

#ifndef acatl_mqtt_session_manager_h
#define acatl_mqtt_session_manager_h

#include <acatl_mqtt/mqtt_packet_sender.h>
#include <acatl_mqtt/mqtt_session.h>

#include <unordered_map>


namespace acatl
{
    namespace mqtt
    {
        
        class SessionManager
        {
        public:
            SessionManager()
            {}
            
            Session::Ptr getSession(const std::string& clientId,
                                    PacketSender::WeakPtr sender,
                                    SubscriptionHandler& subscriptionHandler,
                                    std::error_code& ec)
            {
                std::unique_lock<std::mutex> guard(_sessionsMutex);
                
                return unlockedGetSession(clientId, sender, subscriptionHandler, ec);
            }
            
            bool returnSession(const Session::Ptr session, std::error_code& ec)
            {
                if(!session) {
                    ec = mqtt_error::session_not_found;
                    return false;
                }
                
                std::unique_lock<std::mutex> guard(_sessionsMutex);
                auto iter = _sessions.find(session->clientId());
                if(iter == _sessions.end()) {
                    ec = mqtt_error::session_not_found;
                    return false;
                }
                iter->second._session->setSender(PacketSender::Ptr());
                iter->second._inUse = false;
                ec.clear();
                return true;
            }
            
            bool removeSession(const std::string& clientId, std::error_code& ec)
            {
                std::unique_lock<std::mutex> guard(_sessionsMutex);
                auto iter = _sessions.find(clientId);
                if(iter == _sessions.end()) {
                    ec = mqtt_error::session_not_found;
                    return false;
                }
                if(iter->second._inUse) {
                    ec = mqtt_error::session_in_use;
                    return false;
                }
                _sessions.erase(iter);
                
                return true;
            }
            
            size_t count() const
            {
                std::unique_lock<std::mutex> guard(_sessionsMutex);
                return _sessions.size();
            }
            
        private:
            Session::Ptr unlockedGetSession(const std::string& clientId,
                                            PacketSender::WeakPtr sender,
                                            SubscriptionHandler& subscriptionHandler,
                                            std::error_code& ec)
            {
                auto iter = _sessions.find(clientId);
                if(iter != _sessions.end()) {
                    if(iter->second._inUse) {
                        ec = mqtt_error::session_in_use;
                        return nullptr;
                    }
                    iter->second._session->setSender(sender);
                    iter->second._inUse = true;
                    ec.clear();
                    return iter->second._session;
                }
                auto result = _sessions.emplace(clientId, SessionWrapper(clientId, subscriptionHandler));
                result.first->second._session->setSender(sender);
                result.first->second._inUse = true;
                ec.clear();
                return result.first->second._session;
            }
            
            struct SessionWrapper
            {
                SessionWrapper(const std::string& clientId, SubscriptionHandler& subscriptionHandler)
                : _session(new Session(clientId, subscriptionHandler))
                , _inUse(false)
                {}
                
                Session::Ptr _session;
                bool _inUse;
            };
            
            mutable std::mutex _sessionsMutex;
            std::unordered_map<std::string, SessionWrapper> _sessions;
        };

    }
}

#endif 
