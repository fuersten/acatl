//
//  http_session.cpp
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

#include "http_session.h"

namespace acatl
{
  namespace net
  {
    HttpSession::HttpSession(std::string id, bool secure)
    : _id(id)
    , _isNew(true)
    , _isValid(true)
    , _isSecure(secure)
    , _timestamp(std::chrono::system_clock::now())
    {
    }

    HttpSession::~HttpSession()
    {
    }

    const std::string& HttpSession::id() const
    {
      return _id;
    }

    bool HttpSession::isNew() const
    {
      return _isNew;
    }

    bool HttpSession::isValid() const
    {
      return !_id.empty() && _isValid;
    }

    const std::string& HttpSession::value(const std::string& key) const
    {
      static std::string s_empty;

      auto iter = _values.find(key);
      if(iter != _values.end()) {
        return iter->second;
      }

      return s_empty;
    }

    void HttpSession::value(const std::string& key, const std::string& value)
    {
      _values.insert(std::make_pair(key, value));
    }

    bool HttpSession::hasValue(const std::string& key) const
    {
      auto iter = _values.find(key);
      if(iter != _values.end()) {
        return true;
      }

      return false;
    }

    bool HttpSession::isSecure() const
    {
      return _isSecure;
    }

    void HttpSession::invalidate()
    {
      _values.clear();
      _isValid = false;
    }
  }
}
