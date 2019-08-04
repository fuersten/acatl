//
//  http_session_manager.cpp
//  acatl
//
//  BSD 3-Clause License
//  Copyright (c) 2018, Lars-Christian Fürstenberg
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

#include "http_session_manager.h"

#include <acatl/uuid.h>

namespace acatl
{
  namespace net
  {
    HttpSessionManager::HttpSessionManager(std::chrono::seconds sessionTimeout)
    : _sessionTimeout(sessionTimeout)
    {
    }

    std::string HttpSessionManager::createId()
    {
      return UuidGenerator::generate().toString();
    }

    void HttpSessionManager::invalidate(HttpSession& session)
    {
      std::unique_lock<std::mutex> guard(_mutex);

      auto iter = _sessions.find(session.id());
      if(iter != _sessions.end()) {
        session.invalidate();
      }
    }

    HttpSession& HttpSessionManager::createInvalidSession(std::string id)
    {
      static HttpSession s_session(id, false);
      s_session._isValid = false;
      return s_session;
    }

    HttpSession& HttpSessionManager::sessionForId(const std::string& id)
    {
      std::unique_lock<std::mutex> guard(_mutex);
      static HttpSession& invalidSession{createInvalidSession(createId())};

      auto iter = _sessions.find(id);
      if(iter != _sessions.end()) {
        HttpSession& session = iter->second;
        if(session.isValid()) {
          if(std::chrono::system_clock::now() < session._timestamp + _sessionTimeout) {
            session._isNew = false;
            session._timestamp = std::chrono::system_clock::now();
            return session;
          } else {
            _sessions.erase(id);
          }
        } else {
          _sessions.erase(id);
        }
      }
      return invalidSession;
    }

    HttpSession& HttpSessionManager::createSession(bool secure)
    {
      std::unique_lock<std::mutex> guard(_mutex);

      HttpSession session(createId(), secure);
      return _sessions.insert(std::make_pair(session._id, std::move(session))).first->second;
    }
  }
}
