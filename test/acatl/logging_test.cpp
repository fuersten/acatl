//
//  logging_test.cpp
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

#include <test/acatl/test_helper.h>

#include <acatl/filesystem.h>
#include <acatl/json.h>
#include <acatl/logging.h>
#include <acatl/string_helper.h>

#include <fstream>

namespace fs = acatl::filesystem;

class LoggingTest : public ::testing::Test
{
public:
  LoggingTest()
  {
    std::string json = R"(
{
  "log" : {
      "level" : [
                 {"test::TestCheck" : 1},
                 {"test::TestCheck1" : 2},
                 {"Check" : 2}
      ],
      "separator" : ";"
}
})";

    _json = json::parse(json);
  }

  json _json;
};

namespace test
{
  class TestCheck
  {
  };

  class TestCheck1
  {
  };
}

TEST_F(LoggingTest, logInitTest)
{
  acatl::filesystem::TemporaryDirectoryGuard guard;
  fs::path path = guard.temporaryDirectoryPath();

  acatl::Logging::init(_json, true);

  EXPECT_EQ(1, acatl::Logging::level("test::TestCheck"));
  EXPECT_EQ(2, acatl::Logging::level("test::TestCheck1"));
  EXPECT_EQ(0, acatl::Logging::level("no-such-class"));
}

TEST_F(LoggingTest, logTest)
{
  acatl::filesystem::TemporaryDirectoryGuard guard;
  fs::path path = guard.temporaryDirectoryPath();
  acatl::filesystem::current_path(path);
  RedirectStdErr rs;

  acatl::Logging::init(_json, true);

  acatl::LogEvent event;
  event._category = "Foo";
  event._tid = std::this_thread::get_id();
  event._file = __FILE__;
  event._line = __LINE__;
  event._time = acatl::now();
  event._message = "This is a test message!";
  event._classname = "test::TestCheck";

  acatl::Logging::log(event);

  std::ifstream log(path / "stderr.txt");
  ASSERT_EQ(true, log.good());
  std::string line;
  std::getline(log, line);

  acatl::StringVector parts;
  acatl::split(line, ';', parts);

  ASSERT_EQ(6U, parts.size());
  EXPECT_EQ("Foo", parts[1]);
  EXPECT_EQ("test::TestCheck", parts[3]);
  EXPECT_EQ("This is a test message!", parts[4]);
}

TEST_F(LoggingTest, logMacroTest)
{
  acatl::filesystem::TemporaryDirectoryGuard guard;
  fs::path path = guard.temporaryDirectoryPath();
  acatl::filesystem::current_path(path);
  RedirectStdErr rs;

  acatl::Logging::init(_json, true);

  ACATL_INFOLOG("Just shoot out this message");
  ACATL_ERRORLOG("Here we have some kind of error");
  ACATL_CLASSLOG(test::TestCheck, 1, "This has to be found in the log");
  ACATL_CLASSLOG(test::TestCheck1, 3, "This is not in the log");
  ACATL_CLASSLOG(LoggingTest, 1, "This is also not in the log");
  ACATL_CLASSLOG(test::TestCheck1, 2, "This can be found in the log. Yeah!");

  std::ifstream log(path / "stderr.txt");
  ASSERT_EQ(true, log.good());
  std::string line;
  size_t count(0);
  while(std::getline(log, line).good()) {
    ++count;
    acatl::StringVector parts;
    acatl::split(line, '|', parts);

    switch(parts.size()) {
      case 4:
        EXPECT_EQ("INFO", parts[1]);
        EXPECT_EQ("Just shoot out this message", parts[3]);
        break;
      case 5:
        EXPECT_EQ("ERROR", parts[1]);
        EXPECT_EQ("Here we have some kind of error", parts[3]);
        break;
      case 6:
        EXPECT_EQ("DEBUG", parts[1]);
        if(parts[3] == "test::TestCheck") {
          EXPECT_EQ("This has to be found in the log", parts[4]);
        } else if(parts[3] == "test::TestCheck1") {
          EXPECT_EQ("This can be found in the log. Yeah!", parts[4]);
        } else {
          EXPECT_TRUE(false) << "unknown classname found";
        }
        break;
    }
  }

  EXPECT_EQ(4U, count);
}

TEST_F(LoggingTest, noLevelsTest)
{
  std::string s = R"(
    {
        "log" : {
            "separator" : ";"
        }
    })";

  json obj = json::parse(s);
  acatl::Logging::init(obj, true);
}

TEST_F(LoggingTest, mulitpleInitCall)
{
  std::string s = R"(
    {
        "log" : {
            "level" : [
                       {"test::TestCheck" : 1},
                       {"test::TestCheck1" : 2}
                       ]
        }
    })";

  json obj = json::parse(s);
  acatl::Logging::init(_json, true);

  std::string json1 = R"(
    {
        "log" : {
            "level" : [
                       {"test::TestCheck" : 1},
                       {"test::TestCheck1" : 2},
                       {"huztli" : 3}
                       ]
        }
    })";

  obj = json::parse(json1);
  acatl::Logging::init(obj);

  EXPECT_EQ(0, acatl::Logging::level("huztli"));
}

TEST_F(LoggingTest, debugLog)
{
  acatl::filesystem::TemporaryDirectoryGuard guard;
  fs::path path = guard.temporaryDirectoryPath();
  acatl::filesystem::current_path(path);
  RedirectStdErr rs;

  acatl::Logging::init(_json, true);
  EXPECT_EQ(2, acatl::Logging::level("Check"));

  ACATL_DEBUGLOG(Check, 1, "This definitely has to be found in the log");

  std::ifstream log(path / "stderr.txt");
  ASSERT_EQ(true, log.good());
  std::string line;
  size_t count(0);
  while(std::getline(log, line).good()) {
    ++count;
    acatl::StringVector parts;
    acatl::split(line, ';', parts);

    switch(parts.size()) {
      case 6:
        EXPECT_EQ("DEBUG", parts[1]);
        if(parts[3] == "Check") {
          EXPECT_EQ("This definitely has to be found in the log", parts[4]);
        } else {
          EXPECT_TRUE(false) << "unknown unitname found";
        }
        break;
    }
  }

  EXPECT_EQ(1U, count);
}
