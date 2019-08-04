//
//  date_test.cpp
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

#include <acatl/date_helper.h>


using namespace date::literals;


TEST(DateTest, constructionTest)
{
  auto d1 = date::year_month_day{1970_y / date::September / 23};
  EXPECT_EQ(date::day{23}, d1.day());
  EXPECT_EQ(23_d, d1.day());
  EXPECT_EQ(date::September, d1.month());
  EXPECT_EQ(1970_y, d1.year());

  auto d2 = date::year_month_day{};
  EXPECT_FALSE(d2.ok());

  struct tm y2k;
  y2k.tm_hour = 0;
  y2k.tm_min = 0;
  y2k.tm_sec = 0;
  y2k.tm_year = 2000 - 1900;
  y2k.tm_mon = 0;
  y2k.tm_mday = 1;
  y2k.tm_isdst = 0;
  char utc[] = "UTC";
  y2k.tm_zone = &utc[0];
  time_t time = timegm(&y2k);

  auto d3 = date::year_month_day{date::floor<date::days>(std::chrono::system_clock::from_time_t(time))};
  EXPECT_EQ(1_d, d3.day());
  EXPECT_EQ(date::January, d3.month());
  EXPECT_EQ(2000_y, d3.year());

  auto d4 = date::year_month_day{0_y / date::January / 1};
  EXPECT_EQ(1_d, d4.day());
  EXPECT_EQ(date::January, d4.month());
  EXPECT_EQ(0_y, d4.year());

  auto d5 = date::year_month_day{9999_y / date::December / 31};
  EXPECT_EQ(31_d, d5.day());
  EXPECT_EQ(date::December, d5.month());
  EXPECT_EQ(9999_y, d5.year());
}

TEST(DateTest, specialGetterTest)
{
  auto d1 = 1970_y / date::September / 23;

  EXPECT_EQ(date::Wednesday, date::weekday{d1});
  auto iso = iso_week::year_weeknum_weekday{d1};
  EXPECT_EQ(266, acatl::day_of_year(d1));
  EXPECT_EQ(iso_week::weeknum{39}, iso.weeknum());

  auto d2 = 2014_y / date::December / 30;
  EXPECT_EQ(date::Tuesday, date::weekday{d2});
  EXPECT_EQ(364, acatl::day_of_year(d2));
  EXPECT_EQ(iso_week::weeknum{1}, iso_week::year_weeknum_weekday{d2}.weeknum());
}

TEST(DateTest, addDaysTest)
{
  auto d1 = 1970_y / date::September / 23;

  d1 = acatl::add_days(d1, date::days{7});

  EXPECT_EQ(30_d, d1.day());
  EXPECT_EQ(date::September, d1.month());
  EXPECT_EQ(1970_y, d1.year());

  d1 = acatl::add_days(d1, date::days{-7});
  EXPECT_EQ(23_d, d1.day());
  EXPECT_EQ(date::September, d1.month());
  EXPECT_EQ(1970_y, d1.year());

  auto d2 = 2014_y / date::December / 30;
  d2 = acatl::add_days(d2, date::days{10});
  EXPECT_EQ(9_d, d2.day());
  EXPECT_EQ(date::January, d2.month());
  EXPECT_EQ(2015_y, d2.year());
}

TEST(DateTest, addMonthsTest)
{
  auto d1 = 1970_y / date::September / 23;

  d1 = acatl::add_months(d1, date::months{4});

  EXPECT_EQ(23_d, d1.day());
  EXPECT_EQ(date::January, d1.month());
  EXPECT_EQ(1971_y, d1.year());

  auto d2 = 2014_y / date::December / 31;

  d2 = acatl::add_months(d2, date::months{2});

  EXPECT_EQ(28_d, d2.day());
  EXPECT_EQ(date::February, d2.month());
  EXPECT_EQ(2015_y, d2.year());
}

TEST(DateTest, formatTest)
{
  auto d1 = 1970_y / date::September / 23;
  auto d2 = 2015_y / date::January / 1;

  EXPECT_EQ("1970-09-23", acatl::to_string("%F", d1));
  EXPECT_EQ("2015-01-01", acatl::to_string("%F", d2));

  EXPECT_EQ("23.09.1970 266 39 3", acatl::to_string("%d.%m.%Y %j %V %w", d1));
  EXPECT_EQ("01.01.2015 001 01 4", acatl::to_string("%d.%m.%Y %j %V %w", d2));

  auto d3 = 1_y / date::January / 1;
  EXPECT_EQ("0001-01-01", acatl::to_string("%F", d3));
}

TEST(DateTest, dateFromInt)
{
  auto d1 = 1970_y / date::September / 23;
  auto epoch = date::sys_days{d1}.time_since_epoch();
  date::sys_days d2{date::days{epoch.count()}};
  EXPECT_EQ("1970-09-23", acatl::to_string("%F", d2));
}

TEST(DateTest, calculateGregorianEastern)
{
  EXPECT_EQ(1963_y / date::April / 14, acatl::calculateGregorianEaster(1963_y));
  EXPECT_EQ(1970_y / date::March / 29, acatl::calculateGregorianEaster(1970_y));
  EXPECT_EQ(2018_y / date::April / 1, acatl::calculateGregorianEaster(2018_y));
  EXPECT_EQ(2019_y / date::April / 21, acatl::calculateGregorianEaster(2019_y));
}

TEST(DateTest, parseDate)
{
  auto d = acatl::date_from_string("1970-09-23");
  EXPECT_EQ("1970-09-23", acatl::to_string("%F", d));
}
