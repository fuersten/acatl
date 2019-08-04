//
//  http_cookie.h
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

#ifndef acatl_http_cookie_h
#define acatl_http_cookie_h

#include <acatl/exception.h>

#include <string>

namespace acatl
{
  namespace net
  {
    ACATL_DECLARE_EXCEPTION(MalformedCookieException, acatl::Exception);

    enum class CookieType
    {
      eSERVER,
      eCLIENT
    };

    class HttpCookie
    {
    public:
      HttpCookie(const std::string& name, const std::string& value);

      const std::string& name() const
      {
        return _name;
      }

      const std::string& value() const
      {
        return _value;
      }

      const std::string& domain() const
      {
        return _domain;
      }

      const std::string& path() const
      {
        return _path;
      }

      uint32_t maxAge() const
      {
        return _maxAge;
      }

      bool isSecure() const
      {
        return _secure;
      }

      bool isHttpOnly() const
      {
        return _httpOnly;
      }

      void domain(const std::string& domain)
      {
        _domain = domain;
      }

      void path(const std::string& path)
      {
        _path = path;
      }

      void value(const std::string& value)
      {
        _value = value;
      }

      void maxAge(uint32_t age)
      {
        _maxAge = age;
      }

      void secure(bool secure)
      {
        _secure = secure;
      }

      void httpOnly(bool httpOnly)
      {
        _httpOnly = httpOnly;
      }

      static std::vector<HttpCookie> parseCookie(CookieType type, const std::string& cookie);

    private:
      std::string _name;
      std::string _value;
      std::string _domain;
      std::string _path;
      uint32_t _maxAge;
      bool _secure;
      bool _httpOnly;
    };

    template<class CharT, class Traits>
    std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const HttpCookie& cookie)
    {
      os << cookie.name() << "=" << cookie.value() << ";Version=1";
      if(!cookie.domain().empty()) {
        os << ";Domain=" << cookie.domain();
      }
      if(!cookie.path().empty()) {
        os << ";Path=" << cookie.path();
      }
      if(cookie.maxAge() < std::numeric_limits<uint32_t>::max()) {
        os << ";Max-Age=" << std::to_string(cookie.maxAge());
      }
      if(cookie.isSecure()) {
        os << ";Secure";
      }
      if(cookie.isHttpOnly()) {
        os << ";HttpOnly";
      }

      return os;
    }
  }
}

#endif
