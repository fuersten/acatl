//
//  mime_types.h
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

#ifndef acatl_http_server_mime_types_h
#define acatl_http_server_mime_types_h

#include <acatl/filesystem.h>

#include <algorithm>

typedef std::pair<acatl::filesystem::path, std::string> Mapping;

/// Convert a file extension into a MIME type.
/// @param extension The extension of a path to analyze
/// @return Returns the matching extension. If no extension matches, "text/plain"
///         will be returned.
static const std::string& extension_to_type(const acatl::filesystem::path& extension)
{
  static std::vector<Mapping> s_mappings = {{".txt", "text/plain"},
                                            {".xhtml", "application/xhtml+xml"},
                                            {".css", "text/css"},
                                            {".htm", "text/html"},
                                            {".html", "text/html"},
                                            {".js", "text/javascript"},
                                            {".gif", "image/gif"},
                                            {".jpg", "image/jpeg"},
                                            {".png", "image/png"}};

  auto it = std::find_if(
    s_mappings.begin(), s_mappings.end(), [&extension](const Mapping& mapping) { return mapping.first == extension; });

  if(it != s_mappings.end()) {
    return it->second;
  }

  return s_mappings[0].second;
}

#endif
