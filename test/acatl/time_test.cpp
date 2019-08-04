//
//  time_test.cpp
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

#include <acatl/date_helper.h>


TEST(TimeTest, constructionTest)
{
  {
    auto tod = date::time_of_day<std::chrono::milliseconds>();

    EXPECT_TRUE(tod.in_conventional_range());
    EXPECT_EQ(std::chrono::hours{0}, tod.hours());
    EXPECT_EQ(std::chrono::minutes{0}, tod.minutes());
    EXPECT_EQ(std::chrono::seconds{0}, tod.seconds());
    EXPECT_EQ(std::chrono::milliseconds{0}, tod.subseconds());
  }
  {
    auto tod = date::make_time(
      std::chrono::hours{11}, std::chrono::minutes{45}, std::chrono::seconds{32}, std::chrono::milliseconds{485}, 0);

    EXPECT_TRUE(tod.in_conventional_range());
    EXPECT_EQ(std::chrono::hours{11}, tod.hours());
    EXPECT_EQ(std::chrono::minutes{45}, tod.minutes());
    EXPECT_EQ(std::chrono::seconds{32}, tod.seconds());
    EXPECT_EQ(std::chrono::milliseconds{485}, tod.subseconds());
  }
  {
    auto tod = date::make_time(
      std::chrono::hours{23}, std::chrono::minutes{59}, std::chrono::seconds{59}, std::chrono::milliseconds{999}, 0);

    EXPECT_TRUE(tod.in_conventional_range());
    EXPECT_EQ(std::chrono::hours{23}, tod.hours());
    EXPECT_EQ(std::chrono::minutes{59}, tod.minutes());
    EXPECT_EQ(std::chrono::seconds{59}, tod.seconds());
    EXPECT_EQ(std::chrono::milliseconds{999}, tod.subseconds());
    auto dur = tod.to_duration();
    auto tod2 = date::make_time(std::chrono::milliseconds{dur.count()});
    EXPECT_EQ(dur, tod2.to_duration());
  }
  {
    struct tm y2k;
    y2k.tm_hour = 16;
    y2k.tm_min = 55;
    y2k.tm_sec = 22;
    y2k.tm_year = 2000 - 1900;
    y2k.tm_mon = 0;
    y2k.tm_mday = 1;
    y2k.tm_isdst = 0;
    char utc[] = "UTC";
    y2k.tm_zone = &utc[0];
    time_t time = timegm(&y2k);

    auto tp = std::chrono::system_clock::from_time_t(time);
    auto ts = date::floor<date::days>(tp);

    auto tod = date::make_time(tp - ts);

    EXPECT_TRUE(tod.in_conventional_range());
    EXPECT_EQ(std::chrono::hours{16}, tod.hours());
    EXPECT_EQ(std::chrono::minutes{55}, tod.minutes());
    EXPECT_EQ(std::chrono::seconds{22}, tod.seconds());
    EXPECT_EQ(std::chrono::milliseconds{0}, tod.subseconds());
  }
}

TEST(TimeTest, format)
{
  auto tod = date::make_time(
    std::chrono::hours{23}, std::chrono::minutes{59}, std::chrono::seconds{59}, std::chrono::milliseconds{999}, 0);
  EXPECT_EQ("23:59:59.999", acatl::to_string("%H:%M:%S", tod));
  EXPECT_EQ("11:59:59.999 PM", acatl::to_string("%I:%M:%S %p", tod));
}

TEST(TimeTest, arithmetic)
{
  auto tod1 = date::make_time(
    std::chrono::hours{11}, std::chrono::minutes{45}, std::chrono::seconds{32}, std::chrono::milliseconds{485}, 0);
  auto tod2 = date::make_time(
    std::chrono::hours{23}, std::chrono::minutes{59}, std::chrono::seconds{59}, std::chrono::milliseconds{999}, 0);

  EXPECT_EQ(std::chrono::milliseconds{44067514}, tod2.to_duration() - tod1.to_duration());
  EXPECT_EQ(std::chrono::milliseconds{-44067514}, tod1.to_duration() - tod2.to_duration());
}

TEST(TimeTest, parseTime)
{
  std::istringstream iss{"23:59:59.999"};
  std::chrono::milliseconds time;
  date::from_stream(iss, "%H:%M:%S", time);
  auto tod = date::make_time(time);
  EXPECT_EQ("23:59:59.999", acatl::to_string("%H:%M:%S", tod));
}

/*
    void addTest()
    {
        csvsqldb::Time t(18, 50, 45);
        int32_t hours = t.addHours(6);
        MPF_TEST_ASSERTEQUAL(1, hours);
        MPF_TEST_ASSERTEQUAL(0, t.hour());
        MPF_TEST_ASSERTEQUAL(50, t.minute());
        MPF_TEST_ASSERTEQUAL(45, t.second());

        csvsqldb::Time t1(23, 50, 45);
        hours = t1.addMinutes(10);
        MPF_TEST_ASSERTEQUAL(1, hours);
        MPF_TEST_ASSERTEQUAL(0, t1.hour());
        MPF_TEST_ASSERTEQUAL(0, t1.minute());
        MPF_TEST_ASSERTEQUAL(45, t1.second());

        csvsqldb::Time t2(23, 59, 45);
        hours = t2.addSeconds(20);
        MPF_TEST_ASSERTEQUAL(1, hours);
        MPF_TEST_ASSERTEQUAL(0, t2.hour());
        MPF_TEST_ASSERTEQUAL(0, t2.minute());
        MPF_TEST_ASSERTEQUAL(5, t2.second());
    }
*/
