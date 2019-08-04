//
//  http_parser.h
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

#ifndef acatl_http_parser_h
#define acatl_http_parser_h

#include <acatl/tribool.h>

#include <acatl_network/http_errors.h>
#include <acatl_network/http_types.h>

#include <memory>
#include <vector>

namespace acatl
{
  namespace net
  {
    /// A streaming parser to parse http requests. Extracted http information will be made available through
    /// callbacks. Uses the http parser C implementation from https://github.com/nodejs/http-parser .
    class HttpParser
    {
    public:
      /// Destroys the parser and frees all resources
      virtual ~HttpParser();

      /// Parse a chunk of http data. Will call callbacks for extracted http information.
      /// @param buffer Http data
      /// @param length Length of http data in buffer. Will flag an error if length is bigger than the actual
      /// buffer size.
      /// @param ec Error code set upon error. The error code will be cleared if no error was encountered.
      Tribool parse(const std::vector<char>& buffer, size_t length, std::error_code& ec);

      /// Reset parser state to start parsing new request
      void reset();

    protected:
      enum Type
      {
        REQUEST,
        RESPONSE
      };

      /// Constructs a http parser and will use the given callbacks for extracted http information.
      /// @param callbacks The callback implementation to use
      /// @param type The type of http parser to construct
      HttpParser(HttpCallbacks& callbacks, Type type);

      class Impl;
      std::unique_ptr<Impl> _impl;
    };

    class HttpRequestParser : public HttpParser
    {
    public:
      /// Constructs a http response parser and will use the given callbacks for extracted http information.
      /// @param callbacks The callback implementation to use
      HttpRequestParser(HttpCallbacks& callbacks);
    };

    class HttpResponseParser : public HttpParser
    {
    public:
      /// Constructs a http response parser and will use the given callbacks for extracted http information.
      /// @param callbacks The callback implementation to use
      HttpResponseParser(HttpCallbacks& callbacks);
    };
  }
}

#endif
