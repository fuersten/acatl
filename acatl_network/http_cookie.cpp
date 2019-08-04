//
//  http_cookie.cpp
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

#include "http_cookie.h"

#include <acatl/numeric_cast.h>
#include <acatl/string_helper.h>

namespace acatl
{
  namespace net
  {
    HttpCookie::HttpCookie(const std::string& name, const std::string& value)
    : _name(name)
    , _value(value)
    , _maxAge(std::numeric_limits<uint32_t>::max())
    , _secure(true)
    , _httpOnly(true)
    {
      if(name[0] == '$') {
        ACATL_THROW(MalformedCookieException, "Cookie names may not start with a '$'");
      }
    }

    static std::string getNextPart(std::string::size_type& pos, const std::string& s)
    {
      auto p = s.find_first_of(";,", pos);
      if(p == std::string::npos) {
        auto tmp{pos};
        pos = p;
        return s.substr(tmp);
      }
      auto tmp{pos};
      pos = p + 1;
      return s.substr(tmp, p - tmp);
    }

    static std::pair<std::string, std::string> splitPart(const std::string& part)
    {
      auto p = part.find_first_of("=");
      if(p == std::string::npos) {
        return std::make_pair(acatl::trim_right(acatl::trim_left(part)), "");
      }
      auto val = acatl::trim_right(acatl::trim_left(part.substr(p + 1)));
      if(val[0] == '\"') {
        val = val.substr(1, val.length() - 2);
      }
      return std::make_pair(acatl::tolower_copy(acatl::trim_right(acatl::trim_left(part.substr(0, p)))), val);
    }

    static std::vector<HttpCookie> parseServerCookies(const std::string& cookie)
    {
      std::vector<HttpCookie> cookies;

      std::vector<std::string> cookie_strings;
      acatl::split(cookie, ',', cookie_strings);
      for(const auto& tmp : cookie_strings) {
        std::vector<std::string> parts;
        acatl::split(tmp, ';', parts);

        auto n = parts[0].find_first_of("=");
        if(n == std::string::npos) {
          ACATL_THROW(MalformedCookieException, "Malformed cookie: " << tmp);
        }
        std::string name = acatl::trim_right(acatl::trim_left(parts[0].substr(0, n)));
        std::string value = acatl::trim_right(acatl::trim_left(parts[0].substr(n + 1)));

        if(value[0] == '\"') {
          value = value.substr(1, value.length() - 2);
        }
        cookies.push_back(HttpCookie(name, value));

        parts.erase(parts.begin());

        for(const auto& part : parts) {
          std::string attribute;
          std::string val;

          auto p = part.find_first_of("=");
          if(p != std::string::npos) {
            attribute = acatl::trim_right(acatl::trim_left(part.substr(0, p)));
            acatl::tolower(attribute);
            val = acatl::trim_right(acatl::trim_left(part.substr(p + 1)));
            if(val[0] == '\"') {
              val = val.substr(1, val.length() - 2);
            }
          } else {
            attribute = acatl::tolower_copy(part);
          }
          if(attribute == "domain") {
            cookies.front().domain(val);
          } else if(attribute == "path") {
            cookies.front().path(val);
          } else if(attribute == "max-age") {
            cookies.front().maxAge(acatl::numeric_cast<uint32_t>(std::stoul(val)));
          } else if(attribute == "version") {
            if(val != "1") {
              ACATL_THROW(MalformedCookieException, "Cookie has wrong version: " << part);
            }
          } else if(attribute == "secure") {
            cookies.front().secure(true);
          } else {
            ACATL_THROW(MalformedCookieException, "Malformed cookie, unknown attribute: " << part);
          }
        }
      }
      return cookies;
    }

    static std::vector<HttpCookie> parseClientCookies(const std::string& cookie)
    {
      std::vector<HttpCookie> cookies;

      std::string::size_type pos = 0;
      do {
        auto part = getNextPart(pos, cookie);
        auto split = splitPart(part);
        if(split.second.empty()) {
          ACATL_THROW(MalformedCookieException, "Malformed client cookie: " << part);
        }

        if(split.first == "$version") {
          if(!cookies.empty()) {
            ACATL_THROW(MalformedCookieException, "Malformed client cookie: $Version has to be the first attribute");
          }
          if(split.second != "1") {
            ACATL_THROW(MalformedCookieException, "Cookie has wrong version: " << part);
          }
        } else if(split.first == "$domain") {
          if(cookies.empty()) {
            ACATL_THROW(MalformedCookieException, "Malformed client cookie: no name=value set");
          }
          cookies.back().domain(split.second);
        } else if(split.first == "$path") {
          if(cookies.empty()) {
            ACATL_THROW(MalformedCookieException, "Malformed client cookie: no name=value set");
          }
          cookies.back().path(split.second);
        } else {
          cookies.push_back(HttpCookie(split.first, split.second));
        }
      } while(pos != std::string::npos);

      return cookies;
    }

    std::vector<HttpCookie> HttpCookie::parseCookie(CookieType type, const std::string& cookie)
    {
      switch(type) {
        case CookieType::eSERVER: {
          return parseServerCookies(cookie);
        }
        case CookieType::eCLIENT: {
          return parseClientCookies(cookie);
        }
      }
    }
  }
}
