//
//  http_query_parameter.cpp
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

#include "http_query_parameter.h"

#include <acatl/exception.h>
#include <acatl/string_helper.h>

namespace acatl
{
  namespace net
  {
    HttpQueryParameter::HttpQueryParameter(const std::string& query)
    {
      std::string decoded;
      if(!acatl::decode(query, decoded)) {
        ACATL_THROW(InvalidParameterException, "Could not url decode '" << query << "'");
      }
      StringVector parameter;
      acatl::split(decoded, '&', parameter);

      for(const auto& s : parameter) {
        std::string::size_type p = s.find_first_of("=");
        if(p != std::string::npos) {
          _parameters[s.substr(0, p)] = s.substr(p + 1);
        }
      }
    }

    bool HttpQueryParameter::hasParameter(const std::string& name) const
    {
      return _parameters.find(name) != _parameters.end();
    }

    const std::string& HttpQueryParameter::parameter(const std::string& name) const
    {
      auto iter = _parameters.find(name);
      if(iter == _parameters.end()) {
        ACATL_THROW(InvalidParameterException, "Parameter '" << name << "' not found");
      }
      return iter->second;
    }
  }
}
