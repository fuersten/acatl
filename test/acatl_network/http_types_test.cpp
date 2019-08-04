//
//  http_types_test.cpp
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

#include <acatl_network/http_types.h>

TEST(HttpTypesTest, httpMethod)
{
  acatl::net::HttpMethod method(acatl::net::HttpMethod::Get);
  std::stringstream ss;

  ss << method;
  EXPECT_EQ("GET", ss.str());
  ss.str("");
  ss.clear();

  method = acatl::net::HttpMethod::Delete;
  ss << method;
  EXPECT_EQ("DELETE", ss.str());
}

TEST(HttpTypesTest, httpVersion)
{
  acatl::net::HttpVersion version(acatl::net::HttpVersion::HTTP_1_0);
  std::stringstream ss;

  ss << version;
  EXPECT_EQ("HTTP/1.0", ss.str());
  ss.str("");
  ss.clear();

  version = acatl::net::HttpVersion::HTTP_1_1;
  ss << version;
  EXPECT_EQ("HTTP/1.1", ss.str());
}

TEST(HttpTypesTest, httpStatus)
{
  {
    std::stringstream ss;
    ss << acatl::net::HttpStatus::OK;
    EXPECT_EQ("200 OK", ss.str());
  }
  {
    std::stringstream ss;
    ss << acatl::net::HttpStatus::CONTINUE;
    EXPECT_EQ("100 Continue", ss.str());
  }
  {
    std::stringstream ss;
    ss << acatl::net::HttpStatus::NETWORK_AUTHENTICATION_REQUIRED;
    EXPECT_EQ("511 Network Authentication Required", ss.str());
  }
}
