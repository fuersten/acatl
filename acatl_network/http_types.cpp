//
//  http_types.cpp
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

#include <acatl_network/http_types.h>

#include <thread>
#include <unordered_map>

namespace acatl
{
  namespace net
  {
    std::once_flag initStatusMap;

    const std::string& httpStatusToString(HttpStatus status)
    {
      static std::unordered_map<HttpStatus, std::string> statusCodeMap;

      std::call_once(initStatusMap,
                     [](std::unordered_map<HttpStatus, std::string>& statusCodeMap) {
                       statusCodeMap[HttpStatus::CONTINUE] = "100 Continue";
                       statusCodeMap[HttpStatus::SWITCHING_PROTOCOLS] = "101 Switching Protocols";
                       statusCodeMap[HttpStatus::PROCESSING] = "102 Processing";
                       statusCodeMap[HttpStatus::OK] = "200 OK";
                       statusCodeMap[HttpStatus::CREATED] = "201 Created";
                       statusCodeMap[HttpStatus::ACCEPTED] = "202 Accepted";
                       statusCodeMap[HttpStatus::NON_AUTHORITATIVE_INFORMATION] = "203 Non-Authoritative Information";
                       statusCodeMap[HttpStatus::NO_CONTENT] = "204 No Content";
                       statusCodeMap[HttpStatus::RESET_CONTENT] = "205 Reset Content";
                       statusCodeMap[HttpStatus::PARTIAL_CONTENT] = "206 Partial Content";
                       statusCodeMap[HttpStatus::MULTI_STATUS] = "207 Multi-Status";
                       statusCodeMap[HttpStatus::ALREADY_REPORTED] = "208 Already Reported";
                       statusCodeMap[HttpStatus::IM_USED] = "226 IM Used";
                       statusCodeMap[HttpStatus::MULTIPLE_CHOICES] = "300 Multiple Choices";
                       statusCodeMap[HttpStatus::MOVED_PERMANENTLY] = "301 Moved Permanently";
                       statusCodeMap[HttpStatus::FOUND] = "302 Found";
                       statusCodeMap[HttpStatus::SEE_OTHER] = "303 See Other";
                       statusCodeMap[HttpStatus::NOT_MODIFIED] = "304 Not Modified";
                       statusCodeMap[HttpStatus::USE_PROXY] = "305 Use Proxy";
                       statusCodeMap[HttpStatus::TEMPORARY_REDIRECT] = "307 Temporary Redirect";
                       statusCodeMap[HttpStatus::PERMANENT_REDIRECT] = "308 Permanent Redirect";
                       statusCodeMap[HttpStatus::BAD_REQUEST] = "400 Bad Request";
                       statusCodeMap[HttpStatus::UNAUTHORIZED] = "401 Unauthorized";
                       statusCodeMap[HttpStatus::PAYMENT_REQUIRED] = "402 Payment Required";
                       statusCodeMap[HttpStatus::FORBIDDEN] = "403 Forbidden";
                       statusCodeMap[HttpStatus::NOT_FOUND] = "404 Not Found";
                       statusCodeMap[HttpStatus::METHOD_NOT_ALLOWED] = "405 Method Not Allowed";
                       statusCodeMap[HttpStatus::NOT_ACCEPTABLE] = "406 Not Acceptable";
                       statusCodeMap[HttpStatus::PROXY_AUTHENTICATION_REQUIRED] = "407 Proxy Authentication Required";
                       statusCodeMap[HttpStatus::REQUEST_TIMEOUT] = "408 Request Timeout";
                       statusCodeMap[HttpStatus::CONFLICT] = "409 Conflict";
                       statusCodeMap[HttpStatus::GONE] = "410 Gone";
                       statusCodeMap[HttpStatus::LENGTH_REQUIRED] = "411 Length Required";
                       statusCodeMap[HttpStatus::PRECONDITION_FAILED] = "412 Precondition Failed";
                       statusCodeMap[HttpStatus::PAYLOAD_TOO_LARGE] = "413 Payload Too Large";
                       statusCodeMap[HttpStatus::URI_TOO_LONG] = "414 URI Too Long";
                       statusCodeMap[HttpStatus::UNSUPPORTED_MEDIA_TYPE] = "415 Unsupported Media Type";
                       statusCodeMap[HttpStatus::RANGE_NOT_SATISFIABLE] = "416 Range Not Satisfiable";
                       statusCodeMap[HttpStatus::EXPECTATION_FAILED] = "417 Expectation Failed";
                       statusCodeMap[HttpStatus::MISDIRECTED_REQUEST] = "421 Misdirected Request";
                       statusCodeMap[HttpStatus::UNPROCESSABLE_ENTITY] = "422 Unprocessable Entity";
                       statusCodeMap[HttpStatus::LOCKED] = "423 Locked";
                       statusCodeMap[HttpStatus::FAILED_DEPENDENCY] = "424 Failed Dependency";
                       statusCodeMap[HttpStatus::UPGRADE_REQUIRED] = "426 Upgrade Required";
                       statusCodeMap[HttpStatus::PRECONDITION_REQUIRED] = "428 Precondition Required";
                       statusCodeMap[HttpStatus::TOO_MANY_REQUESTS] = "429 Too Many Requests";
                       statusCodeMap[HttpStatus::REQUEST_HEADER_FIELDS_TOO_LARGE] =
                         "431 Request Header Fields Too Large";
                       statusCodeMap[HttpStatus::UNAVAILABLE_FOR_LEGAL_REASONS] = "451 Unavailable For Legal Reasons";
                       statusCodeMap[HttpStatus::INTERNAL_SERVER_ERROR] = "500 Internal Server Error";
                       statusCodeMap[HttpStatus::NOT_IMPLEMENTED] = "501 Not Implemented";
                       statusCodeMap[HttpStatus::BAD_GATEWAY] = "502 Bad Gateway";
                       statusCodeMap[HttpStatus::SERVICE_UNAVAILABLE] = "503 Service Unavailable";
                       statusCodeMap[HttpStatus::GATEWAY_TIMEOUT] = "504 Gateway Timeout";
                       statusCodeMap[HttpStatus::HTTP_VERSION_NOT_SUPPORTED] = "505 HTTP Version Not Supported";
                       statusCodeMap[HttpStatus::VARIANT_ALSO_NEGOTIATES] = "506 Variant Also Negotiates";
                       statusCodeMap[HttpStatus::INSUFFICIENT_STORAGE] = "507 Insufficient Storage";
                       statusCodeMap[HttpStatus::LOOP_DETECTED] = "508 Loop Detected";
                       statusCodeMap[HttpStatus::NOT_EXTENDED] = "510 Not Extended";
                       statusCodeMap[HttpStatus::NETWORK_AUTHENTICATION_REQUIRED] =
                         "511 Network Authentication Required";
                     },
                     statusCodeMap);

      return statusCodeMap[status];
    }
  }
}
