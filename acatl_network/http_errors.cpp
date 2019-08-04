//
//  http_errors.cpp
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

#include <acatl_network/http_errors.h>

#include <http_parser.h>


namespace acatl
{
  namespace net
  {
    const char* http_error_category_t::name() const noexcept
    {
      return "http_error";
    }

    std::string http_error_category_t::message(int ev) const
    {
      switch(http_error(ev)) {
        case http_error::message_begin_cb_error:
          return http_errno_description(HPE_CB_message_begin);
        case http_error::url_cb_error:
          return http_errno_description(HPE_CB_url);
        case http_error::header_field_cb_error:
          return http_errno_description(HPE_CB_header_field);
        case http_error::header_value_cb_error:
          return http_errno_description(HPE_CB_header_value);
        case http_error::headers_complete_cb_error:
          return http_errno_description(HPE_CB_headers_complete);
        case http_error::body_cb_error:
          return http_errno_description(HPE_CB_body);
        case http_error::message_complete_cb_error:
          return http_errno_description(HPE_CB_message_complete);
        case http_error::status_cb_error:
          return http_errno_description(HPE_CB_status);
        case http_error::chunk_header_cb_error:
          return http_errno_description(HPE_CB_chunk_header);
        case http_error::chunk_complete_cb_error:
          return http_errno_description(HPE_CB_chunk_complete);
        case http_error::invalid_eof_state:
          return http_errno_description(HPE_INVALID_EOF_STATE);
        case http_error::header_overflow:
          return http_errno_description(HPE_HEADER_OVERFLOW);
        case http_error::closed_connection:
          return http_errno_description(HPE_CLOSED_CONNECTION);
        case http_error::invalid_version:
          return http_errno_description(HPE_INVALID_VERSION);
        case http_error::invalid_status:
          return http_errno_description(HPE_INVALID_STATUS);
        case http_error::invalid_method:
          return http_errno_description(HPE_INVALID_METHOD);
        case http_error::invalid_url:
          return http_errno_description(HPE_INVALID_URL);
        case http_error::invalid_host:
          return http_errno_description(HPE_INVALID_HOST);
        case http_error::invalid_port:
          return http_errno_description(HPE_INVALID_PORT);
        case http_error::invalid_path:
          return http_errno_description(HPE_INVALID_PATH);
        case http_error::invalid_query_string:
          return http_errno_description(HPE_INVALID_QUERY_STRING);
        case http_error::invalid_fragment:
          return http_errno_description(HPE_INVALID_FRAGMENT);
        case http_error::lf_expected:
          return http_errno_description(HPE_LF_EXPECTED);
        case http_error::invalid_header_token:
          return http_errno_description(HPE_INVALID_HEADER_TOKEN);
        case http_error::invalid_content_length:
          return http_errno_description(HPE_INVALID_CONTENT_LENGTH);
        case http_error::unexpected_content_length:
          return http_errno_description(HPE_UNEXPECTED_CONTENT_LENGTH);
        case http_error::invalid_chunk_size:
          return http_errno_description(HPE_INVALID_CHUNK_SIZE);
        case http_error::invalid_constant:
          return http_errno_description(HPE_INVALID_CONSTANT);
        case http_error::invalid_internal_state:
          return http_errno_description(HPE_INVALID_INTERNAL_STATE);
        case http_error::strict:
          return http_errno_description(HPE_STRICT);
        case http_error::paused:
          return http_errno_description(HPE_PAUSED);
        case http_error::unknown:
          return http_errno_description(HPE_UNKNOWN);
        case http_error::buffer_overflow:
          return "buffer overflowed";
        case http_error::parsing_complete:
          return "parsing was already comlete, reset the parser to continue";
        default:
          throw std::runtime_error("unknown error code");
      }
    }

    const std::error_category& http_error_category()
    {
      static http_error_category_t instance;
      return instance;
    }
  }
}
