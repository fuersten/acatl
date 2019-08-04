//
//  http_types.h
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

#ifndef acatl_http_types_h
#define acatl_http_types_h

#include <ostream>

namespace acatl
{
  namespace net
  {
    typedef uint16_t Port;

    /// Supported http methods.
    enum class HttpMethod
    {
      Get,       ///< HTTP GET method
      Post,      ///< HTTP POST method
      Put,       ///< HTTP PUT method
      Delete,    ///< HTTP DELETE method
      Head,      ///< HTTP HEAD method
      Connect,   ///< HTTP CONNECT method
      Options,   ///< HTTP OPTIONS method
      Trace      ///< HTTP TRACE method
    };

    template<class CharT, class Traits>
    std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const HttpMethod& m)
    {
      switch(m) {
        case HttpMethod::Get:
          os << "GET";
          break;
        case HttpMethod::Post:
          os << "POST";
          break;
        case HttpMethod::Put:
          os << "PUT";
          break;
        case HttpMethod::Delete:
          os << "DELETE";
          break;
        case HttpMethod::Head:
          os << "HEAD";
          break;
        case HttpMethod::Connect:
          os << "CONNECT";
          break;
        case HttpMethod::Options:
          os << "OPTIONS";
          break;
        case HttpMethod::Trace:
          os << "TRACE";
          break;
      }
      return os;
    }

    /// Supported http versions.
    enum class HttpVersion
    {
      HTTP_1_0,   ///< Http 1.0 version
      HTTP_1_1    ///< Http 1.1 version
    };

    template<class CharT, class Traits>
    std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const HttpVersion& m)
    {
      switch(m) {
        case HttpVersion::HTTP_1_0:
          os << "HTTP/1.0";
          break;
        case HttpVersion::HTTP_1_1:
          os << "HTTP/1.1";
          break;
      }
      return os;
    }

    // Http status codes
    enum class HttpStatus : std::uint16_t
    {
      CONTINUE = 100,
      SWITCHING_PROTOCOLS = 101,
      PROCESSING = 102,
      OK = 200,
      CREATED = 201,
      ACCEPTED = 202,
      NON_AUTHORITATIVE_INFORMATION = 203,
      NO_CONTENT = 204,
      RESET_CONTENT = 205,
      PARTIAL_CONTENT = 206,
      MULTI_STATUS = 207,
      ALREADY_REPORTED = 208,
      IM_USED = 226,
      MULTIPLE_CHOICES = 300,
      MOVED_PERMANENTLY = 301,
      FOUND = 302,
      SEE_OTHER = 303,
      NOT_MODIFIED = 304,
      USE_PROXY = 305,
      TEMPORARY_REDIRECT = 307,
      PERMANENT_REDIRECT = 308,
      BAD_REQUEST = 400,
      UNAUTHORIZED = 401,
      PAYMENT_REQUIRED = 402,
      FORBIDDEN = 403,
      NOT_FOUND = 404,
      METHOD_NOT_ALLOWED = 405,
      NOT_ACCEPTABLE = 406,
      PROXY_AUTHENTICATION_REQUIRED = 407,
      REQUEST_TIMEOUT = 408,
      CONFLICT = 409,
      GONE = 410,
      LENGTH_REQUIRED = 411,
      PRECONDITION_FAILED = 412,
      PAYLOAD_TOO_LARGE = 413,
      URI_TOO_LONG = 414,
      UNSUPPORTED_MEDIA_TYPE = 415,
      RANGE_NOT_SATISFIABLE = 416,
      EXPECTATION_FAILED = 417,
      MISDIRECTED_REQUEST = 421,
      UNPROCESSABLE_ENTITY = 422,
      LOCKED = 423,
      FAILED_DEPENDENCY = 424,
      UPGRADE_REQUIRED = 426,
      PRECONDITION_REQUIRED = 428,
      TOO_MANY_REQUESTS = 429,
      REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
      UNAVAILABLE_FOR_LEGAL_REASONS = 451,
      INTERNAL_SERVER_ERROR = 500,
      NOT_IMPLEMENTED = 501,
      BAD_GATEWAY = 502,
      SERVICE_UNAVAILABLE = 503,
      GATEWAY_TIMEOUT = 504,
      HTTP_VERSION_NOT_SUPPORTED = 505,
      VARIANT_ALSO_NEGOTIATES = 506,
      INSUFFICIENT_STORAGE = 507,
      LOOP_DETECTED = 508,
      NOT_EXTENDED = 510,
      NETWORK_AUTHENTICATION_REQUIRED = 511
    };

    /// Converts a HttpStatus code into a message string
    /// @param status The status code to convert
    /// @return Message string for the code
    const std::string& httpStatusToString(HttpStatus status);

    template<class CharT, class Traits>
    std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const HttpStatus& status)
    {
      os << httpStatusToString(status);
      return os;
    }

    /// Callbacks for the http parser. Will be called while parsing chunks of http data.
    class HttpCallbacks
    {
    public:
      virtual ~HttpCallbacks()
      {
      }

      /// Will be called upon the start of an request
      /// @param method The http method used for the request
      /// @return 0 upon success and != 0 upon failure. Request processing will stop upon failure.
      virtual int onMessageBegin(HttpMethod method) = 0;

      /// Will be called upon the start of an response
      /// @param statusCode The http status code of the response
      /// @param statusText The http status text of the response
      /// @return 0 upon success and != 0 upon failure. Request processing will stop upon failure.
      virtual int onStatus(HttpStatus statusCode, const std::string& statusText) = 0;

      /// Will be called upon parsing the url
      /// @param url The url used for the request
      /// @return 0 upon success and != 0 upon failure. Request processing will stop upon failure.
      virtual int onUrl(const std::string& url) = 0;

      /// Will be called for each header field name, before calling onHeaderValue for the corresponding header
      /// value
      /// @param field The header field name
      /// @return 0 upon success and != 0 upon failure. Request processing will stop upon failure.
      virtual int onHeaderField(const std::string& field) = 0;

      /// Will be called for each header value, just after calling onHeaderField
      /// @param value The header value
      /// @return 0 upon success and != 0 upon failure. Request processing will stop upon failure.
      virtual int onHeaderValue(const std::string& value) = 0;

      /// Will be called upon completion of parsing all header values
      /// @param version The http version of the request
      /// @return 0 upon success and != 0 upon failure. Request processing will stop upon failure.
      virtual int onHeaderComplete(HttpVersion version) = 0;

      /// Will be called for the body of the request. This callback will be called potentially various times,
      /// depending of body length.
      /// @param data The data of the body
      /// @param length The length of the body data
      /// @return 0 upon success and != 0 upon failure. Request processing will stop upon failure.
      virtual int onBody(const char* data, size_t length) = 0;

      /// Will be called once at the end of the request.
      /// @param closeConnection Set to true if the connection shall be closed right after processing this request
      /// or not.
      /// @return 0 upon success and != 0 upon failure. Request processing will stop upon failure.
      virtual int onMessageComplete(bool closeConnection) = 0;
    };
  }
}

#endif
