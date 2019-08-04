//
//  http_url_parser_test.cpp
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

#include <gtest/gtest.h>

#include <acatl_network/http_url_parser.h>

TEST(HttpUrlParserTest, basic)
{
  acatl::net::HttpUrlParser parser;

  std::error_code ec;
  acatl::net::HttpUrl url = parser.parse("http://mydomain.de:8080/mqtt/status?q=connection", ec);
  ASSERT_FALSE(ec);
  ASSERT_EQ("http", url.schema());
  ASSERT_TRUE(url.userInfo().empty());
  ASSERT_EQ("mydomain.de", url.host());
  ASSERT_EQ(8080, url.port());
  ASSERT_EQ("/mqtt/status", url.path());
  ASSERT_EQ("q=connection", url.query());
  ASSERT_TRUE(url.fragment().empty());

  url = parser.parse("http://example", ec);
  ASSERT_FALSE(ec);
  ASSERT_EQ("http", url.schema());
  ASSERT_TRUE(url.userInfo().empty());
  ASSERT_EQ("example", url.host());
  ASSERT_EQ(0, url.port());
  ASSERT_TRUE(url.path().empty());
  ASSERT_TRUE(url.query().empty());
  ASSERT_TRUE(url.fragment().empty());
}

TEST(HttpUrlParserTest, complex)
{
  acatl::net::HttpUrlParser parser;

  std::error_code ec;
  acatl::net::HttpUrl url = parser.parse("https://www.example.org/suche?stichwort=wiki&ausgabe=liste#top", ec);
  ASSERT_FALSE(ec);
  ASSERT_EQ("https", url.schema());
  ASSERT_TRUE(url.userInfo().empty());
  ASSERT_EQ("www.example.org", url.host());
  ASSERT_EQ(0, url.port());
  ASSERT_EQ("/suche", url.path());
  ASSERT_EQ("stichwort=wiki&ausgabe=liste", url.query());
  ASSERT_EQ("top", url.fragment());
}

TEST(HttpUrlParserTest, withCredentials)
{
  acatl::net::HttpUrlParser parser;

  std::error_code ec;
  acatl::net::HttpUrl url = parser.parse("https://max:muster@www.example.com:8080/index.html?p1=A&p2=B#ressource", ec);
  ASSERT_FALSE(ec);
  ASSERT_EQ("https", url.schema());
  ASSERT_EQ("max:muster", url.userInfo());
  ASSERT_EQ("www.example.com", url.host());
  ASSERT_EQ(8080, url.port());
  ASSERT_EQ("/index.html", url.path());
  ASSERT_EQ("p1=A&p2=B", url.query());
  ASSERT_EQ("ressource", url.fragment());
}

TEST(HttpUrlParserTest, error)
{
  acatl::net::HttpUrlParser parser;

  {
    std::error_code ec;
    acatl::net::HttpUrl url = parser.parse("://www.example.org/suche?stichwort=wiki&ausgabe=liste#top", ec);
    ASSERT_TRUE(ec);
    ASSERT_TRUE(url.schema().empty());
    ASSERT_TRUE(url.userInfo().empty());
    ASSERT_TRUE(url.host().empty());
    ASSERT_EQ(0, url.port());
    ASSERT_TRUE(url.path().empty());
    ASSERT_TRUE(url.query().empty());
    ASSERT_TRUE(url.fragment().empty());
  }

  {
    std::error_code ec;
    acatl::net::HttpUrl url = parser.parse("example", ec);
    ASSERT_TRUE(ec);
    ASSERT_TRUE(url.schema().empty());
    ASSERT_TRUE(url.userInfo().empty());
    ASSERT_TRUE(url.host().empty());
    ASSERT_EQ(0, url.port());
    ASSERT_TRUE(url.path().empty());
    ASSERT_TRUE(url.query().empty());
    ASSERT_TRUE(url.fragment().empty());
  }
}
