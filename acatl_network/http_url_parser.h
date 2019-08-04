//
//  http_url_parser.h
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

#ifndef acatl_http_url_parser_h
#define acatl_http_url_parser_h

#include <acatl_network/http_types.h>

namespace acatl
{
  namespace net
  {
    /// A HttpUrl holds all parts of an url. If a part is not set in the url, the corresponding getter will return
    /// an empty string.
    class HttpUrl
    {
    public:
      /// Returns the schema or an empty string of an url.
      /// @return The schema of the url
      const std::string& schema() const;

      /// Returns the user info or an empty string of an url.
      /// @return The user info of the url
      const std::string& userInfo() const;

      /// Returns the host or an empty string of an url.
      /// @return The host of the url
      const std::string& host() const;

      /// Returns the port of an url. If no port was specified, 0 is returned.
      /// @return The port of the url
      Port port() const;

      /// Returns the path or an empty string of an url.
      /// @return The path of the url
      const std::string& path() const;

      /// Returns the query or an empty string of an url.
      /// @return The query of the url
      const std::string& query() const;

      /// Returns the fragment or an empty string of an url.
      /// @return The fragment of the url
      const std::string& fragment() const;

    private:
      friend class HttpUrlParser;

      HttpUrl();

      std::string _schema;
      std::string _userInfo;
      std::string _host;
      Port _port;
      std::string _path;
      std::string _query;
      std::string _fragment;
    };

    /// A parser for urls.
    /// Uses the http parser C implementation from https://github.com/nodejs/http-parser .
    class HttpUrlParser
    {
    public:
      /// Constructs an url parser
      HttpUrlParser();

      ~HttpUrlParser();

      /// Parses an url string and returns an HttpUrl instance.
      /// @param url The url to parse
      /// @param ec The error code is set upon failure. It will not be
      ///           reset upon success.
      /// @return Upon success, returns a filled HttpUrl instance. Otherwise
      ///         an empty HttpUrl instance. The error code ec will be set
      ///         upon error.
      HttpUrl parse(const std::string& url, std::error_code& ec);

    private:
      struct Impl;
      std::unique_ptr<Impl> _impl;
    };
  }
}

#endif
