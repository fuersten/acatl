//
//  http_parser.cpp
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

#include <acatl_network/http_parser.h>

#include <http_parser.h>

#include <sstream>

namespace acatl
{
  namespace net
  {
    class HttpParser::Impl
    {
    public:
      enum class State
      {
        Start,
        Status,
        Url,
        HeaderField,
        HeaderValue,
        Body,
        Complete
      };

      Impl(HttpCallbacks& callbacks, http_parser_type type)
      : _type(type)
      , _state(State::Start)
      , _callbacks(callbacks)
      {
        http_parser_init(&_parser, type);
        _parser.data = this;

        http_parser_settings_init(&_settings);
        _settings.on_message_begin = Impl::onMessageBegin;
        _settings.on_status = Impl::onStatus;
        _settings.on_url = Impl::onUrl;
        _settings.on_header_field = Impl::onHeaderField;
        _settings.on_header_value = Impl::onHeaderValue;
        _settings.on_headers_complete = Impl::onHeaderComplete;
        _settings.on_body = Impl::onBody;
        _settings.on_message_complete = Impl::onMessageComplete;
      }

      Tribool parse(const std::vector<char>& buffer, size_t length, std::error_code& ec)
      {
        if(_state == State::Complete) {
          ec = http_error::parsing_complete;
          return false;
        }
        if(buffer.size() < length) {
          ec = http_error::buffer_overflow;
          return false;
        }
        size_t nread = http_parser_execute(&_parser, &_settings, buffer.data(), length);
        if(nread != length) {
          ec.assign(HTTP_PARSER_ERRNO(&_parser), http_error_category());
          return false;
        }
        ec.clear();
        return _state == State::Complete ? true : Tribool{};
      }

      void reset()
      {
        _state = State::Start;
        _data.str("");
        _data.clear();
      }

      static int onMessageBegin(http_parser* parser)
      {
        Impl* pObj = reinterpret_cast<Impl*>(parser->data);
        HttpMethod method;
        switch(parser->method) {
          case HTTP_GET:
            method = HttpMethod::Get;
            break;
          case HTTP_POST:
            method = HttpMethod::Post;
            break;
          case HTTP_PUT:
            method = HttpMethod::Put;
            break;
          case HTTP_DELETE:
            method = HttpMethod::Delete;
            break;
          case HTTP_HEAD:
            method = HttpMethod::Head;
            break;
          default:
            return 1;
        }
        return pObj->_callbacks.onMessageBegin(method);
      }

      static int onStatus(http_parser* parser, const char* at, size_t length)
      {
        Impl* pObj = reinterpret_cast<Impl*>(parser->data);
        pObj->_state = State::Status;
        pObj->_statusCode = static_cast<HttpStatus>(parser->status_code);
        pObj->_data << std::string(at, length);
        return 0;
      }

      static int onUrl(http_parser* parser, const char* at, size_t length)
      {
        Impl* pObj = reinterpret_cast<Impl*>(parser->data);
        pObj->_state = State::Url;
        pObj->_data << std::string(at, length);
        return 0;
      }

      static int onHeaderField(http_parser* parser, const char* at, size_t length)
      {
        int ret = 0;
        Impl* pObj = reinterpret_cast<Impl*>(parser->data);
        if(pObj->_state != State::HeaderField) {
          ret = pObj->callCallback(pObj->_data.str());
          pObj->_data.str("");
          pObj->_data.clear();
          pObj->_state = State::HeaderField;
        }
        pObj->_data << std::string(at, length);
        return 0;
      }

      static int onHeaderValue(http_parser* parser, const char* at, size_t length)
      {
        int ret = 0;
        Impl* pObj = reinterpret_cast<Impl*>(parser->data);
        if(pObj->_state != State::HeaderValue) {
          ret = pObj->callCallback(pObj->_data.str());
          pObj->_data.str("");
          pObj->_data.clear();
          pObj->_state = State::HeaderValue;
        }
        pObj->_data << std::string(at, length);
        return ret;
      }

      static int onHeaderComplete(http_parser* parser)
      {
        int ret = 0;
        Impl* pObj = reinterpret_cast<Impl*>(parser->data);
        ret = pObj->callCallback(pObj->_data.str());
        pObj->_data.str("");
        pObj->_data.clear();
        HttpVersion version{HttpVersion::HTTP_1_1};
        if(parser->http_major == 1 && parser->http_minor == 0) {
          version = HttpVersion::HTTP_1_0;
        } else if(parser->http_major == 1 && parser->http_minor == 1) {
          version = HttpVersion::HTTP_1_1;
        } else {
          return 1;
        }
        pObj->_callbacks.onHeaderComplete(version);
        pObj->_state = State::Body;
        return ret;
      }

      static int onBody(http_parser* parser, const char* at, size_t length)
      {
        Impl* pObj = reinterpret_cast<Impl*>(parser->data);
        return pObj->_callbacks.onBody(at, length);
      }

      static int onMessageComplete(http_parser* parser)
      {
        int ret = 0;
        Impl* pObj = reinterpret_cast<Impl*>(parser->data);
        ret = pObj->_callbacks.onMessageComplete(http_should_keep_alive(parser) == 0);
        pObj->_state = State::Complete;
        return ret;
      }

      int callCallback(const std::string& data)
      {
        switch(_state) {
          case State::Url:
            return _callbacks.onUrl(data);
          case State::HeaderField:
            return _callbacks.onHeaderField(data);
          case State::HeaderValue:
            return _callbacks.onHeaderValue(data);
          case State::Body:
          case State::Complete:
          case State::Status:
            return _callbacks.onStatus(_statusCode, data);
          case State::Start:
            return 1;
        }

        return 1;
      }

      http_parser_type _type;
      State _state;
      HttpStatus _statusCode;
      http_parser _parser;
      http_parser_settings _settings;
      std::stringstream _data;

      HttpCallbacks& _callbacks;
    };

    HttpParser::HttpParser(HttpCallbacks& callbacks, Type type)
    : _impl(new Impl{callbacks, type == REQUEST ? http_parser_type::HTTP_REQUEST : http_parser_type::HTTP_RESPONSE})
    {
    }

    HttpParser::~HttpParser()
    {
    }

    Tribool HttpParser::parse(const std::vector<char>& buffer, size_t length, std::error_code& ec)
    {
      return _impl->parse(buffer, length, ec);
    }

    void HttpParser::reset()
    {
      _impl->reset();
    }

    HttpRequestParser::HttpRequestParser(HttpCallbacks& callbacks)
    : HttpParser(callbacks, REQUEST)
    {
    }

    HttpResponseParser::HttpResponseParser(HttpCallbacks& callbacks)
    : HttpParser(callbacks, RESPONSE)
    {
    }
  }
}
