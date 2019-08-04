//
//  http_cookie_test.cpp
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

#include <acatl_network/http_cookie.h>

TEST(HttpCookieTest, good)
{
  auto cookies = acatl::net::HttpCookie::parseCookie(
    acatl::net::CookieType::eSERVER,
    "Customer=\"WILE_E_COYOTE\"; Version=\"1\"; Path=\"/acme\"; Domain=\".acme.com\";Secure");
  EXPECT_EQ(1u, cookies.size());
  auto cookie = cookies.front();
  EXPECT_EQ("Customer", cookie.name());
  EXPECT_EQ("WILE_E_COYOTE", cookie.value());
  EXPECT_EQ("/acme", cookie.path());
  EXPECT_EQ(".acme.com", cookie.domain());
  EXPECT_TRUE(cookie.isSecure());
  EXPECT_TRUE(cookie.isHttpOnly());
}

TEST(HttpCookieTest, multi)
{
  auto cookies =
    acatl::net::HttpCookie::parseCookie(acatl::net::CookieType::eSERVER, "theme=light, sessionToken=abc123");
  EXPECT_EQ(2u, cookies.size());
  auto cookie = cookies.front();
  EXPECT_EQ("theme", cookie.name());
  EXPECT_EQ("light", cookie.value());
  EXPECT_EQ("", cookie.path());
  EXPECT_EQ("", cookie.domain());
  EXPECT_TRUE(cookie.isSecure());
  EXPECT_TRUE(cookie.isHttpOnly());
}

TEST(HttpCookieTest, client)
{
  auto cookies = acatl::net::HttpCookie::parseCookie(
    acatl::net::CookieType::eCLIENT,
    "$Version=\"1\"; Customer=\"WILE_E_COYOTE\"; $Domain=\"acme.org\", $Path=\"/acme\"");
  EXPECT_EQ(1u, cookies.size());
  auto cookie = cookies.front();
  EXPECT_EQ("customer", cookie.name());
  EXPECT_EQ("WILE_E_COYOTE", cookie.value());
  EXPECT_EQ("/acme", cookie.path());
  EXPECT_EQ("acme.org", cookie.domain());
}

TEST(HttpCookieTest, multiClient)
{
  auto cookies = acatl::net::HttpCookie::parseCookie(acatl::net::CookieType::eCLIENT,
                                                     "$Version=\"1\"; Customer=\"WILE_E_COYOTE\"; "
                                                     "$Path=\"/acme/account\", Part_Number=\"Rocket_Launcher_0001\"; "
                                                     "$Path=\"/acme\"; Shipping=\"FedEx\", $Path=\"/acme\"");
  EXPECT_EQ(3u, cookies.size());
  auto cookie = cookies[0];
  EXPECT_EQ("customer", cookie.name());
  EXPECT_EQ("WILE_E_COYOTE", cookie.value());
  EXPECT_EQ("/acme/account", cookie.path());

  cookie = cookies[1];
  EXPECT_EQ("part_number", cookie.name());
  EXPECT_EQ("Rocket_Launcher_0001", cookie.value());
  EXPECT_EQ("/acme", cookie.path());

  cookie = cookies[2];
  EXPECT_EQ("shipping", cookie.name());
  EXPECT_EQ("FedEx", cookie.value());
  EXPECT_EQ("/acme", cookie.path());
}
