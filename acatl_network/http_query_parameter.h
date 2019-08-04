//
//  http_query_parameter.h
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

#ifndef acatl_http_query_parameter_h
#define acatl_http_query_parameter_h

#include <string>
#include <unordered_map>

namespace acatl
{
  namespace net
  {
    /**
     * Extracts and serves query parameter from the query string of an Url.
     */
    class HttpQueryParameter
    {
    public:
      /**
       * Constructs QueryParameter from a query string.
       * @param query The query parameter as Url query string
       */
      HttpQueryParameter(const std::string& query);

      /**
       * Checks if the given parameter exists.
       * @param name The parameter name to check
       * @return true, if the parameter exists, else otherwise
       */
      bool hasParameter(const std::string& name) const;

      /**
       * Fetches the given parameter. Throws an InvalidParameterException if the parameter is unknown.
       * @param name The parameter name to fetch
       * @return The parameter value.
       */
      const std::string& parameter(const std::string& name) const;

    private:
      std::unordered_map<std::string, std::string> _parameters;
    };
  }
}

#endif
