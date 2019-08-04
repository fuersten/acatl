//
//  connection.h
//  acatl
//
//  BSD 3-Clause License
//  Copyright (c) 2017, Lars-Christian Fürstenberg
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

#ifndef acatl_http_server_connection_h
#define acatl_http_server_connection_h

#include "mime_types.h"
#include "types.h"

#include <acatl/logging.h>
#include <acatl/string_helper.h>
#include <acatl_network/http_url_parser.h>
#include <acatl_network/socket_type.h>

#include <memory>

template<typename Socket>
class Connection
: public acatl::net::HttpCallbacks
, public std::enable_shared_from_this<Connection<Socket>>
{
public:
  typedef Socket SocketType;

  Connection(SocketType&& socket, const HttpConfig& config)
  : _parser(*this)
  , _config(config)
  , _socket(std::move(socket))
  {
    _readBuf.resize(bufSize);
    _writeBuf.resize(bufSize);
  }

  ~Connection()
  {
    ACATL_CLASSLOG(Connection, 1, "Terminating connection");
  }

  void start()
  {
    ACATL_CLASSLOG(Connection, 1, "Start connection");
    do_read();
  }

  void sendResponseHeader(const HttpResponse& response)
  {
    std::stringstream ss;
    ss << response;
    auto r = ss.str();
    if(_writeBuf.size() < r.size()) {
      _writeBuf.resize(r.size());
    }
    memcpy(&_writeBuf[0], r.c_str(), r.length());

    do_write(r.length());
  }

private:
  int onMessageBegin(acatl::net::HttpMethod method) override
  {
    if(method != acatl::net::HttpMethod::Get && method != acatl::net::HttpMethod::Post) {
      ACATL_ERRORLOG("We only support http GET or POST requests");
      return 1;
    }
    _request._method = method;

    return 0;
  }

  int onStatus(acatl::net::HttpStatus, const std::string&) override
  {
    ACATL_ERRORLOG("Should not have been called");
    return 0;
  }

  int onUrl(const std::string& url) override
  {
    ACATL_CLASSLOG(Connection, 2, "Request for URL: " << url);
    _request._url = url;
    return 0;
  }

  int onHeaderField(const std::string& field) override
  {
    _iter = _request._header.emplace(field, "").first;
    return 0;
  }

  int onHeaderValue(const std::string& value) override
  {
    _iter->second = value;
    return 0;
  }

  int onHeaderComplete(acatl::net::HttpVersion version) override
  {
    _request._version = version;
    return 0;
  }

  int onBody(const char*, size_t) override
  {
    return 0;
  }

  int onMessageComplete(bool closeConnection) override
  {
    ACATL_CLASSLOG(Connection, 2, "Request completed");

    HttpResponse response;
    response._version = _request._version;
    if(!closeConnection) {
      response._header = {{"Connection", "keep-alive"}};
    } else {
      response._header = {{"Connection", "close"}};
    }
    std::stringstream ss;

    std::error_code ec;
    acatl::net::HttpUrlParser urlParser;
    acatl::net::HttpUrl url = urlParser.parse(_request._url, ec);
    if(ec) {
      response._status = acatl::net::HttpStatus::BAD_REQUEST;
    } else {
      fs::path path{url.path()};
      auto filePath = fs::canonical(fs::path(path).relative_path(), _config._rootPath, ec);

      if(ec || !fs::exists(filePath, ec)) {
        response._status = acatl::net::HttpStatus::NOT_FOUND;
        ss << "File not found!";
      } else {
        ss << acatl::load_file(filePath, ec);
        if(ec) {
          ss.str("");
          ss.clear();
          response._status = acatl::net::HttpStatus::NOT_FOUND;
          ss << "File not found!";
        } else {
          response._status = acatl::net::HttpStatus::OK;
          response._header["Content-Type"] = extension_to_type(filePath.extension());
        }
      }
    }
    auto r = ss.str();
    response._header["Content-Length"] = std::to_string(r.size());
    sendResponseHeader(response);

    if(_writeBuf.size() < r.size()) {
      _writeBuf.resize(r.size());
    }
    memcpy(&_writeBuf[0], r.c_str(), r.length());

    do_write(r.length());

    return 0;
  }

  void do_read()
  {
    auto self(this->shared_from_this());
    _socket().async_read_some(asio::buffer(_readBuf, bufSize), [self](std::error_code ec, std::size_t length) {
      if(!ec) {
        self->handle_read(length);
      } else {
        ACATL_ERRORLOG("Read error: " << ec.message());
      }
    });
  }

  void handle_read(std::size_t length)
  {
    std::error_code ec;
    auto ret = _parser.parse(_readBuf, length, ec);
    if(ret.isFalse()) {
      ACATL_ERRORLOG("Http parser error: " << ec.message());
      return;
    }
    if(ret.isIndeterminate()) {
      do_read();
    } else {
      _parser.reset();
    }
  }

  void do_write(std::size_t length)
  {
    if(length > 0) {
      auto self(this->shared_from_this());
      asio::async_write(_socket(), asio::buffer(_writeBuf, length), [self](std::error_code ec, std::size_t /*length*/) {
        if(!ec) {
          self->do_read();
        } else {
          ACATL_ERRORLOG("Write error: " << ec.message());
        }
      });
    }
  }

  static const size_t bufSize = 1024;

  std::vector<char> _readBuf;
  std::vector<char> _writeBuf;
  acatl::net::HttpRequestParser _parser;
  const HttpConfig& _config;
  HttpRequest _request;
  std::unordered_map<std::string, std::string>::iterator _iter;

  SocketType _socket;
};

#endif
