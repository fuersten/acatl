//
//  timestamp_test.cpp
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

#include <acatl/timestamp.h>

using namespace date::literals;


TEST(TimestampTest, constructionTest)
{
  {
    acatl::TimePoint tp = acatl::datetime_utc_to_timepoint(1970_y / date::September / date::day{23},
                                                           std::chrono::hours{8},
                                                           std::chrono::minutes{9},
                                                           std::chrono::seconds{11});

    acatl::Timestamp ts{tp};

    EXPECT_TRUE(ts.ok());
    EXPECT_EQ(date::day{23}, ts.day());
    EXPECT_EQ(date::September, ts.month());
    EXPECT_EQ(1970_y, ts.year());
    EXPECT_EQ(std::chrono::hours{8}, ts.hours());
    EXPECT_EQ(std::chrono::minutes{9}, ts.minutes());
    EXPECT_EQ(std::chrono::seconds{11}, ts.seconds());
    EXPECT_EQ(std::chrono::milliseconds{0}, ts.milliseconds());
  }

  {
    acatl::Timestamp ts{1970_y / date::September / date::day{23},
                        std::chrono::hours{8},
                        std::chrono::minutes{9},
                        std::chrono::seconds{11},
                        std::chrono::milliseconds{115}};

    EXPECT_TRUE(ts.ok());
    EXPECT_EQ(date::day{23}, ts.day());
    EXPECT_EQ(date::September, ts.month());
    EXPECT_EQ(1970_y, ts.year());
    EXPECT_EQ(std::chrono::hours{8}, ts.hours());
    EXPECT_EQ(std::chrono::minutes{9}, ts.minutes());
    EXPECT_EQ(std::chrono::seconds{11}, ts.seconds());
    EXPECT_EQ(std::chrono::milliseconds{115}, ts.milliseconds());
  }

  {
    acatl::Timestamp ts;
    EXPECT_FALSE(ts.ok());
  }

  {
    acatl::Timestamp ts{9999_y / date::December / date::day{31},
                        std::chrono::hours{23},
                        std::chrono::minutes{59},
                        std::chrono::seconds{59},
                        std::chrono::milliseconds{999}};

    EXPECT_TRUE(ts.ok());
    EXPECT_EQ(date::day{31}, ts.day());
    EXPECT_EQ(date::December, ts.month());
    EXPECT_EQ(9999_y, ts.year());
    EXPECT_EQ(std::chrono::hours{23}, ts.hours());
    EXPECT_EQ(std::chrono::minutes{59}, ts.minutes());
    EXPECT_EQ(std::chrono::seconds{59}, ts.seconds());
    EXPECT_EQ(std::chrono::milliseconds{999}, ts.milliseconds());
  }
}

TEST(TimestampTest, importExportTest)
{
  acatl::Timestamp ts{1970_y / date::September / date::day{23},
                      std::chrono::hours{8},
                      std::chrono::minutes{9},
                      std::chrono::seconds{11},
                      std::chrono::milliseconds{115}};

  std::chrono::milliseconds ms = ts.to_timepoint().time_since_epoch();
  int64_t rep = ms.count();

  acatl::TimePoint tp;
  tp += std::chrono::milliseconds{rep};

  EXPECT_EQ(ts.to_timepoint(), tp);

  acatl::Timestamp ts1{tp};
  EXPECT_TRUE(ts1.ok());
  EXPECT_EQ(date::day{23}, ts1.day());
  EXPECT_EQ(date::September, ts1.month());
  EXPECT_EQ(1970_y, ts1.year());
  EXPECT_EQ(std::chrono::hours{8}, ts1.hours());
  EXPECT_EQ(std::chrono::minutes{9}, ts1.minutes());
  EXPECT_EQ(std::chrono::seconds{11}, ts1.seconds());
  EXPECT_EQ(std::chrono::milliseconds{115}, ts1.milliseconds());
}

TEST(TimestampTest, formatTest)
{
  acatl::Timestamp ts1{1970_y / date::September / date::day{23},
                       std::chrono::hours{8},
                       std::chrono::minutes{9},
                       std::chrono::seconds{11},
                       std::chrono::milliseconds{115}};

  EXPECT_EQ("1970-09-23T08:09:11.115", to_string("%FT%T", ts1));
  EXPECT_EQ("1970-09-23T08:09:11.115", to_string("%Y-%m-%dT%H:%M:%S", ts1));

  acatl::Timestamp ts2{2015_y / date::July / date::day{2},
                       std::chrono::hours{14},
                       std::chrono::minutes{20},
                       std::chrono::seconds{30},
                       std::chrono::milliseconds{0}};

  EXPECT_EQ("2015-07-02T14:20:30.000", to_string("%FT%T", ts2));

  EXPECT_EQ("23.09.1970 266 39 3", to_string("%d.%m.%Y %j %V %w", ts1));
  EXPECT_EQ("02.07.2015 183 27 4", to_string("%d.%m.%Y %j %V %w", ts2));
}

TEST(TimestampTest, validateTest)
{
  {
    acatl::Timestamp ts{9999_y / date::December / date::day{31},
                        std::chrono::hours{23},
                        std::chrono::minutes{59},
                        std::chrono::seconds{59},
                        std::chrono::milliseconds{999}};
    EXPECT_TRUE(ts.ok());
  }
  {
    acatl::Timestamp ts{9999_y / date::December / date::day{31},
      std::chrono::hours{23},
      std::chrono::minutes{59},
      std::chrono::seconds{59},
      std::chrono::milliseconds{999}};
    EXPECT_TRUE(ts.ok());
  }
  {
    acatl::Timestamp ts{2014_y / date::November / date::day{31},
      std::chrono::hours{25},
      std::chrono::minutes{5},
      std::chrono::seconds{17},
      std::chrono::milliseconds{0}};
    EXPECT_FALSE(ts.ok());
  }
  {
    acatl::Timestamp ts{2012_y / date::February / date::day{29},
      std::chrono::hours{23},
      std::chrono::minutes{59},
      std::chrono::seconds{59},
      std::chrono::milliseconds{135}};
    EXPECT_TRUE(ts.ok());
  }
  {
    acatl::Timestamp ts{2013_y / date::February / date::day{29},
      std::chrono::hours{23},
      std::chrono::minutes{59},
      std::chrono::seconds{59},
      std::chrono::milliseconds{135}};
    EXPECT_FALSE(ts.ok());
  }
}

TEST(TimestampTest, parseTimestamp)
{
  acatl::TimePoint tp = acatl::timepoint_from_string("1970-09-23T08:09:11.115");
  EXPECT_EQ("1970-09-23T08:09:11.115", to_string("%FT%T", acatl::Timestamp{tp}));
}
