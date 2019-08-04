//
//  http_url_parser.cpp
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

#include "http_url_parser.h"

#include <acatl/numeric_cast.h>

#include <acatl_network/http_errors.h>

#include <http_parser.h>


namespace acatl
{
  namespace net
  {
    HttpUrl::HttpUrl()
    : _port(0)
    {
    }

    const std::string& HttpUrl::schema() const
    {
      return _schema;
    }

    const std::string& HttpUrl::userInfo() const
    {
      return _userInfo;
    }

    const std::string& HttpUrl::host() const
    {
      return _host;
    }

    Port HttpUrl::port() const
    {
      return _port;
    }

    const std::string& HttpUrl::path() const
    {
      return _path;
    }

    const std::string& HttpUrl::query() const
    {
      return _query;
    }

    const std::string& HttpUrl::fragment() const
    {
      return _fragment;
    }

    struct HttpUrlParser::Impl
    {
    public:
      Impl()
      {
        ::http_parser_url_init(&_url);
      }

      struct http_parser_url _url;
    };

    HttpUrlParser::HttpUrlParser()
    : _impl(new Impl)
    {
    }

    HttpUrlParser::~HttpUrlParser()
    {
    }

    HttpUrl HttpUrlParser::parse(const std::string& url, std::error_code& ec)
    {
      if(http_parser_parse_url(url.c_str(), url.size(), false, &_impl->_url) != 0) {
        ec = http_error::invalid_url;
        return HttpUrl();
      }

      HttpUrl httpUrl;

      if(_impl->_url.field_set & (1 << UF_SCHEMA)) {
        std::string s(url.c_str() + _impl->_url.field_data[UF_SCHEMA].off, _impl->_url.field_data[UF_SCHEMA].len);

        httpUrl._schema = std::move(s);
      }

      if(_impl->_url.field_set & (1 << UF_HOST)) {
        std::string s(url.c_str() + _impl->_url.field_data[UF_HOST].off, _impl->_url.field_data[UF_HOST].len);

        httpUrl._host = std::move(s);
      }

      if(_impl->_url.field_set & (1 << UF_PORT)) {
        std::string s(url.c_str() + _impl->_url.field_data[UF_PORT].off, _impl->_url.field_data[UF_PORT].len);

        httpUrl._port = numeric_cast<Port>(std::stoul(s));
      }

      if(_impl->_url.field_set & (1 << UF_PATH)) {
        std::string s(url.c_str() + _impl->_url.field_data[UF_PATH].off, _impl->_url.field_data[UF_PATH].len);

        httpUrl._path = std::move(s);
      }

      if(_impl->_url.field_set & (1 << UF_QUERY)) {
        std::string s(url.c_str() + _impl->_url.field_data[UF_QUERY].off, _impl->_url.field_data[UF_QUERY].len);

        httpUrl._query = std::move(s);
      }

      if(_impl->_url.field_set & (1 << UF_FRAGMENT)) {
        std::string s(url.c_str() + _impl->_url.field_data[UF_FRAGMENT].off, _impl->_url.field_data[UF_FRAGMENT].len);

        httpUrl._fragment = std::move(s);
      }

      if(_impl->_url.field_set & (1 << UF_USERINFO)) {
        std::string s(url.c_str() + _impl->_url.field_data[UF_USERINFO].off, _impl->_url.field_data[UF_USERINFO].len);

        httpUrl._userInfo = std::move(s);
      }

      return httpUrl;
    }
  }
}
