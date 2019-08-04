//
//  http_session.h
//  acatl
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

#ifndef acatl_http_session_h
#define acatl_http_session_h

#include <chrono>
#include <string>
#include <unordered_map>

namespace acatl
{
  namespace net
  {
    class HttpSession
    {
    public:
      /// Not copyable
      HttpSession(const HttpSession&) = delete;
      HttpSession& operator=(const HttpSession&) = delete;

      /// Movable
      HttpSession(HttpSession&&) = default;
      HttpSession& operator=(HttpSession&&) = default;

      ~HttpSession();

      const std::string& id() const;

      bool isNew() const;

      bool isValid() const;

      bool isSecure() const;

      void invalidate();

      const std::string& value(const std::string& key) const;

      void value(const std::string& key, const std::string& value);

      bool hasValue(const std::string& key) const;

    private:
      HttpSession(std::string id, bool secure);

      std::string _id;
      bool _isNew;
      bool _isValid;
      bool _isSecure;
      std::chrono::system_clock::time_point _timestamp;
      std::unordered_map<std::string, std::string> _values;

      friend class HttpSessionManager;
    };
  }
}

#endif
