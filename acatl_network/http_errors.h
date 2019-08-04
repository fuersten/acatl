//
//  http_errors.h
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

#ifndef acatl_http_errors_hpp
#define acatl_http_errors_hpp

#include <acatl/inc.h>

#include <string>
#include <system_error>


namespace acatl
{
  namespace net
  {
    /// Http related errors
    enum class http_error
    {
      message_begin_cb_error = 1,
      url_cb_error,
      header_field_cb_error,
      header_value_cb_error,
      headers_complete_cb_error,
      body_cb_error,
      message_complete_cb_error,
      status_cb_error,
      chunk_header_cb_error,
      chunk_complete_cb_error,
      invalid_eof_state,
      header_overflow,
      closed_connection,
      invalid_version,
      invalid_status,
      invalid_method,
      invalid_url,
      invalid_host,
      invalid_port,
      invalid_path,
      invalid_query_string,
      invalid_fragment,
      lf_expected,
      invalid_header_token,
      invalid_content_length,
      unexpected_content_length,
      invalid_chunk_size,
      invalid_constant,
      invalid_internal_state,
      strict,
      paused,
      unknown,
      buffer_overflow,
      parsing_complete
    };

    /// Error category for http related errors
    class http_error_category_t : public std::error_category
    {
    public:
      const char* name() const noexcept override;
      std::string message(int ev) const override;
    };

    /// The only instance of the http error category
    const std::error_category& http_error_category();

    /// Converts a http error code to a std error_code
    inline std::error_code make_error_code(acatl::net::http_error e)
    {
      const std::error_category& cat = acatl::net::http_error_category();
      return std::error_code(static_cast<int>(e), cat);
    }
  }
}

namespace std
{
  template<>
  struct is_error_code_enum<acatl::net::http_error> : public true_type
  {
  };
}

#endif
