//
//  http_query_parameter_test.cpp
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

#include <acatl/exception.h>
#include <acatl_network/http_query_parameter.h>

TEST(HttpQueryParameterTest, basic)
{
  std::string query("wikiPageName=Testpage&wikiPageMarkdown=Markdown%0D%0A%3D%3D%3D%3D%3D%3D%3D%3D%0D%0A%0D%0A-+some+"
                    "tests%0D%0A-+and+more%0D%0A%0D%0A");
  acatl::net::HttpQueryParameter parameter(query);
  EXPECT_TRUE(parameter.hasParameter("wikiPageName"));
  EXPECT_TRUE(parameter.hasParameter("wikiPageMarkdown"));
  EXPECT_EQ("Testpage", parameter.parameter("wikiPageName"));
  EXPECT_EQ("Markdown\r\n========\r\n\r\n- some tests\r\n- and more\r\n\r\n", parameter.parameter("wikiPageMarkdown"));
  EXPECT_THROW(parameter.parameter("hutzli"), acatl::InvalidParameterException);
}
