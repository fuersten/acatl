//
//  http_parser_test.cpp
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

#include <gtest/gtest.h>

#include <acatl_network/http_parser.h>

class MyCallbackHandler : public acatl::net::HttpCallbacks
{
public:
  MyCallbackHandler(int ret = 0)
  : _ret(ret)
  {
  }

  int onMessageBegin(acatl::net::HttpMethod method) override
  {
    _method = method;
    return _ret;
  }

  int onStatus(acatl::net::HttpStatus statusCode, const std::string& statusText) override
  {
    _statusCode = statusCode;
    _statusText = statusText;
    return _ret;
  }

  int onUrl(const std::string& url) override
  {
    _url = url;
    return _ret;
  }

  int onHeaderField(const std::string& field) override
  {
    _header.push_back(std::make_tuple(field, ""));
    return _ret;
  }

  int onHeaderValue(const std::string& value) override
  {
    std::get<1>(_header.back()) = value;
    return _ret;
  }

  int onHeaderComplete(acatl::net::HttpVersion version) override
  {
    _version = version;
    return _ret;
  }

  int onBody(const char* data, size_t length) override
  {
    _body = std::string(data, length);
    return _ret;
  }

  int onMessageComplete(bool closeConnection) override
  {
    _shallClose = closeConnection;
    return _ret;
  }

  acatl::net::HttpMethod _method;
  acatl::net::HttpVersion _version;
  acatl::net::HttpStatus _statusCode;
  std::string _statusText;
  std::string _url;
  std::vector<std::tuple<std::string, std::string>> _header;
  std::string _body;
  bool _shallClose;
  bool _ret;
};

TEST(HttpRequestParserTest, basic)
{
  std::error_code ec;
  MyCallbackHandler handler;

  acatl::net::HttpRequestParser parser(handler);

  std::string raw = "POST /mqtt/status?q=connection HTTP/1.1\r\n"
                    "Accept: */*\r\n"
                    "Transfer-Encoding: identity\r\n"
                    "Content-Length: 36\r\n"
                    "Connection: keep-alive\r\n"
                    "\r\n"
                    "ClientId=9849384h9efz9ehfw9eh298g28g";
  std::vector<char> req(raw.begin(), raw.end());

  EXPECT_TRUE(parser.parse(req, req.size(), ec));

  EXPECT_EQ(acatl::net::HttpMethod::Post, handler._method);
  EXPECT_EQ(acatl::net::HttpVersion::HTTP_1_1, handler._version);
  EXPECT_EQ("/mqtt/status?q=connection", handler._url);
  EXPECT_EQ(4u, handler._header.size());
  EXPECT_EQ("Accept", std::get<0>(handler._header[0]));
  EXPECT_EQ("*/*", std::get<1>(handler._header[0]));
  EXPECT_EQ("Transfer-Encoding", std::get<0>(handler._header[1]));
  EXPECT_EQ("identity", std::get<1>(handler._header[1]));
  EXPECT_EQ("Content-Length", std::get<0>(handler._header[2]));
  EXPECT_EQ("36", std::get<1>(handler._header[2]));
  EXPECT_EQ("Connection", std::get<0>(handler._header[3]));
  EXPECT_EQ("keep-alive", std::get<1>(handler._header[3]));
  EXPECT_FALSE(handler._shallClose);
}

TEST(HttpRequestParserTest, reset)
{
  std::error_code ec;
  MyCallbackHandler handler;

  acatl::net::HttpRequestParser parser(handler);

  std::string raw = "POST /mqtt/status?q=connection HTTP/1.1\r\n"
                    "Accept: */*\r\n"
                    "Transfer-Encoding: identity\r\n"
                    "Content-Length: 36\r\n"
                    "Connection: keep-alive\r\n"
                    "\r\n"
                    "ClientId=9849384h9efz9ehfw9eh298g28g";
  std::vector<char> req(raw.begin(), raw.end());

  EXPECT_TRUE(parser.parse(req, req.size(), ec));

  EXPECT_FALSE(parser.parse(req, req.size(), ec));
  EXPECT_EQ(acatl::net::http_error::parsing_complete, ec);
  parser.reset();
  EXPECT_TRUE(parser.parse(req, req.size(), ec));
  EXPECT_FALSE(ec);
}

TEST(HttpRequestParserTest, multiBuffer)
{
  std::error_code ec;
  MyCallbackHandler handler;

  acatl::net::HttpRequestParser parser(handler);

  std::string raw1 = "POST /mqtt/status?q=connection HTTP/1.0\r\n"
                     "Accept: */*\r\n"
                     "Transfer-Encoding: identity\r\n"
                     "Content-";

  std::string raw2 = "Length: 36\r\n"
                     "Connection: close\r\n"
                     "\r\n"
                     "ClientId=9849384h9efz9ehfw9eh298g28g";

  std::vector<char> req1(raw1.begin(), raw1.end());
  std::vector<char> req2(raw2.begin(), raw2.end());

  EXPECT_TRUE(parser.parse(req1, req1.size(), ec).isIndeterminate());
  EXPECT_TRUE(parser.parse(req2, req2.size(), ec));

  EXPECT_EQ(acatl::net::HttpMethod::Post, handler._method);
  EXPECT_EQ(acatl::net::HttpVersion::HTTP_1_0, handler._version);
  EXPECT_EQ("/mqtt/status?q=connection", handler._url);
  EXPECT_EQ(4u, handler._header.size());
  EXPECT_EQ("Accept", std::get<0>(handler._header[0]));
  EXPECT_EQ("*/*", std::get<1>(handler._header[0]));
  EXPECT_EQ("Transfer-Encoding", std::get<0>(handler._header[1]));
  EXPECT_EQ("identity", std::get<1>(handler._header[1]));
  EXPECT_EQ("Content-Length", std::get<0>(handler._header[2]));
  EXPECT_EQ("36", std::get<1>(handler._header[2]));
  EXPECT_EQ("Connection", std::get<0>(handler._header[3]));
  EXPECT_EQ("close", std::get<1>(handler._header[3]));
  EXPECT_EQ("ClientId=9849384h9efz9ehfw9eh298g28g", handler._body);
  EXPECT_TRUE(handler._shallClose);
}

TEST(HttpRequestParserTest, parseResponse)
{
  std::error_code ec;
  MyCallbackHandler handler;

  acatl::net::HttpRequestParser parser(handler);

  std::string response = "HTTP/1.1 200 OK\r\n"
                         "Date: Mon, 27 Jul 2009 12:28:53 GMT\r\n"
                         "Server: acatl1.0.0\r\n"
                         "Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n"
                         "Content-Length: 52\r\n"
                         "Content-Type: text/html\r\n"
                         "Connection: Closed\r\n\r\n"
                         "<html>\n"
                         "<body>\n"
                         "<h1>Hello, World!</h1>\n"
                         "</body>\n"
                         "</html>";

  std::vector<char> resp(response.begin(), response.end());

  EXPECT_FALSE(parser.parse(resp, resp.size(), ec));
  EXPECT_EQ("invalid HTTP method", ec.message());
  EXPECT_TRUE(ec == acatl::net::http_error::invalid_method);
}

TEST(HttpRequestParserTest, parseError)
{
  std::error_code ec;
  MyCallbackHandler handler;

  acatl::net::HttpRequestParser parser(handler);

  std::string raw = "POST /mqtt/status?q=connection HTTP/1.1\r\n"
                    "Accept: */*\r\n"
                    "Transfer-Encoding\r\n"
                    "Content-Length: 36\r\n"
                    "Connection: keep-alive\r\n"
                    "\r\n"
                    "ClientId=9849384h9efz9ehfw9eh298g28g";

  std::vector<char> req(raw.begin(), raw.end());

  EXPECT_FALSE(parser.parse(req, req.size(), ec));
  EXPECT_EQ("invalid character in header", ec.message());
  EXPECT_TRUE(ec == acatl::net::http_error::invalid_header_token);
}

TEST(HttpRequestParserTest, handlerError)
{
  std::error_code ec;
  MyCallbackHandler handler(1);

  acatl::net::HttpRequestParser parser(handler);

  std::string raw = "POST /mqtt/status?q=connection HTTP/1.1\r\n"
                    "Accept: */*\r\n"
                    "Transfer-Encoding: identity\r\n"
                    "Content-Length: 36\r\n"
                    "Connection: keep-alive\r\n"
                    "\r\n"
                    "ClientId=9849384h9efz9ehfw9eh298g28g";

  std::vector<char> req(raw.begin(), raw.end());

  EXPECT_FALSE(parser.parse(req, req.size(), ec));
  EXPECT_EQ("the on_message_begin callback failed", ec.message());
  EXPECT_TRUE(ec == acatl::net::http_error::message_begin_cb_error);
}

TEST(HttpRequestParserTest, overflowError)
{
  std::error_code ec;
  MyCallbackHandler handler;

  acatl::net::HttpRequestParser parser(handler);

  std::string raw = "POST /mqtt/status?q=connection HTTP/1.1\r\n"
                    "Accept: */*\r\n"
                    "Transfer-Encoding: identity\r\n"
                    "Content-Length: 36\r\n"
                    "Connection: keep-alive\r\n"
                    "\r\n"
                    "ClientId=9849384h9efz9ehfw9eh298g28g";
  std::vector<char> req(raw.begin(), raw.end());

  EXPECT_FALSE(parser.parse(req, 10000, ec));
  EXPECT_EQ(acatl::net::http_error::buffer_overflow, ec);
}

TEST(HttpResponseParserTest, parseResponse)
{
  std::error_code ec;
  MyCallbackHandler handler;

  acatl::net::HttpResponseParser parser(handler);

  std::string response = "HTTP/1.1 200 OK\r\n"
                         "Date: Mon, 27 Jul 2009 12:28:53 GMT\r\n"
                         "Server: acatl1.0.0\r\n"
                         "Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n"
                         "Content-Length: 52\r\n"
                         "Content-Type: text/html\r\n"
                         "Connection: close\r\n\r\n"
                         "<html>\n"
                         "<body>\n"
                         "<h1>Hello, World!</h1>\n"
                         "</body>\n"
                         "</html>";

  std::vector<char> resp(response.begin(), response.end());

  EXPECT_TRUE(parser.parse(resp, resp.size(), ec));
  EXPECT_FALSE(ec);
  EXPECT_EQ(acatl::net::HttpStatus::OK, handler._statusCode);
  EXPECT_EQ("OK", handler._statusText);
  EXPECT_EQ(6u, handler._header.size());
  EXPECT_EQ("Date", std::get<0>(handler._header[0]));
  EXPECT_EQ("Mon, 27 Jul 2009 12:28:53 GMT", std::get<1>(handler._header[0]));
  EXPECT_EQ("Server", std::get<0>(handler._header[1]));
  EXPECT_EQ("acatl1.0.0", std::get<1>(handler._header[1]));
  EXPECT_EQ("Last-Modified", std::get<0>(handler._header[2]));
  EXPECT_EQ("Wed, 22 Jul 2009 19:15:56 GMT", std::get<1>(handler._header[2]));
  EXPECT_EQ("Content-Length", std::get<0>(handler._header[3]));
  EXPECT_EQ("52", std::get<1>(handler._header[3]));
  EXPECT_EQ("Content-Type", std::get<0>(handler._header[4]));
  EXPECT_EQ("text/html", std::get<1>(handler._header[4]));
  EXPECT_EQ("Connection", std::get<0>(handler._header[5]));
  EXPECT_EQ("close", std::get<1>(handler._header[5]));
  EXPECT_EQ("<html>\n<body>\n<h1>Hello, World!</h1>\n</body>\n</html>", handler._body);
}

TEST(HttpErrorTest, basic)
{
  std::error_code ec = acatl::net::http_error::header_overflow;
  EXPECT_EQ("too many header bytes seen; overflow detected", ec.message());

  ec = acatl::net::http_error::invalid_method;
  EXPECT_EQ("invalid HTTP method", ec.message());
}
