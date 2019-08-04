//
//  string_helper_test.cpp
//  acatl
//
//  BSD 3-Clause License
//  Copyright (c) 2016, Lars-Christian Fürstenberg
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
#include <acatl/string_helper.h>

#include <fstream>
#include <vector>

namespace fs = acatl::filesystem;

namespace acatl
{
  static std::string callTimeStream(const std::chrono::system_clock::time_point& tp)
  {
    std::ostringstream os;
    os << tp;
    return os.str();
  }
}

TEST(StringHelperTest, tokenizer)
{
  std::string s("This is my glorious test   string");
  std::vector<std::string> tokens;
  acatl::split(s, ' ', tokens);

  EXPECT_EQ(8u, tokens.size());

  EXPECT_EQ("This", tokens[0]);
  EXPECT_EQ("is", tokens[1]);
  EXPECT_EQ("my", tokens[2]);
  EXPECT_EQ("glorious", tokens[3]);
  EXPECT_EQ("test", tokens[4]);
  EXPECT_EQ("", tokens[5]);
  EXPECT_EQ("", tokens[6]);
  EXPECT_EQ("string", tokens[7]);

  s = "This,is,my,glorious,test,,,string";
  acatl::split(s, ' ', tokens);

  EXPECT_EQ(1u, tokens.size());

  EXPECT_EQ("This,is,my,glorious,test,,,string", tokens[0]);

  s = "This,is,my,glorious,test,,,string";
  acatl::split(s, ',', tokens);

  EXPECT_EQ(8u, tokens.size());

  EXPECT_EQ("This", tokens[0]);
  EXPECT_EQ("is", tokens[1]);
  EXPECT_EQ("my", tokens[2]);
  EXPECT_EQ("glorious", tokens[3]);
  EXPECT_EQ("test", tokens[4]);
  EXPECT_EQ("", tokens[5]);
  EXPECT_EQ("", tokens[6]);
  EXPECT_EQ("string", tokens[7]);

  s = "This,is,my,glorious,test,,,string";
  acatl::split(s, ',', tokens, false);
  EXPECT_EQ(6u, tokens.size());

  EXPECT_EQ("This", tokens[0]);
  EXPECT_EQ("is", tokens[1]);
  EXPECT_EQ("my", tokens[2]);
  EXPECT_EQ("glorious", tokens[3]);
  EXPECT_EQ("test", tokens[4]);
  EXPECT_EQ("string", tokens[5]);
}

TEST(StringHelperTest, joinTest)
{
  std::vector<std::string> tokens;
  tokens.push_back("This");
  tokens.push_back("is");
  tokens.push_back("my");
  tokens.push_back("glorious");
  tokens.push_back("test");
  tokens.push_back("string");

  EXPECT_EQ("This,is,my,glorious,test,string", acatl::join(tokens, ","));
}

TEST(StringHelperTest, joinIntsTest)
{
  std::vector<int64_t> ints = {1, 2, 3, 4, 5, 6, 7, 8, 9};

  EXPECT_EQ("1,2,3,4,5,6,7,8,9", acatl::join(ints, ","));
}

TEST(StringHelperTest, upperTest)
{
  std::string s("Not All upper");
  acatl::toupper(s);
  EXPECT_EQ("NOT ALL UPPER", s);

  std::string s1("Not All upper");
  std::string s2 = acatl::toupper_copy(s1);
  EXPECT_EQ("NOT ALL UPPER", s2);
  EXPECT_EQ("Not All upper", s1);
}

TEST(StringHelperTest, lowerTest)
{
  std::string s("Not All upper");
  acatl::tolower(s);
  EXPECT_EQ("not all upper", s);

  std::string s1("Not All upper");
  std::string s2 = acatl::tolower_copy(s1);
  EXPECT_EQ("not all upper", s2);
  EXPECT_EQ("Not All upper", s1);
}

TEST(StringHelperTest, stringCompareTest)
{
  EXPECT_EQ(0, acatl::stricmp("Test it", "TEST IT"));
  EXPECT_GE(acatl::stricmp("Txst it", "TEST IT"), 1);
  EXPECT_EQ(0, acatl::strnicmp("Test it", "TEST IT", 5));
  EXPECT_GE(acatl::strnicmp("Txst it", "TEST IT", 5), 1);
}

TEST(StringHelperTest, timeFormatTest)
{
  std::chrono::duration<int, std::mega> megaSecs(22);
  std::chrono::duration<int, std::kilo> kiloSecs(921);
  std::chrono::duration<int, std::deca> decaSecs(20);
  std::chrono::system_clock::time_point tp;
  tp += megaSecs;
  tp += kiloSecs;
  tp += decaSecs;

  struct tm ts;
  ts.tm_hour = 7;
  ts.tm_min = 0;
  ts.tm_sec = 0;
  ts.tm_year = 1970 - 1900;
  ts.tm_mon = 8;
  ts.tm_mday = 23;
  ts.tm_isdst = 0;
  char utc[] = "UTC";
  ts.tm_zone = &utc[0];

  char buffer[20];
  time_t tt = timegm(&ts);
  struct tm* lt = ::localtime(&tt);
  ::strftime(buffer, 20, "%FT%T", lt);

  EXPECT_EQ(buffer, acatl::callTimeStream(tp));
  EXPECT_EQ("Wed, 23 Sep 1970 07:00:00 GMT", acatl::formatDateRfc1123(tp));
}

TEST(StringHelperTest, trimTest)
{
  std::string s("     left whitespace ");
  EXPECT_EQ("left whitespace ", acatl::trim_left(s));
  s = "no left whitespace";
  EXPECT_EQ("no left whitespace", acatl::trim_left(s));
  s = " right whitespace    ";
  EXPECT_EQ(" right whitespace", acatl::trim_right(s));
  s = "no right whitespace";
  EXPECT_EQ("no right whitespace", acatl::trim_right(s));
}

TEST(StringHelperTest, formattedDurationTest)
{
  std::string res = acatl::formattedDuration(std::chrono::duration<double, std::micro>(99));
  EXPECT_EQ("99.0us", res);
  res = acatl::formattedDuration(std::chrono::duration<double, std::micro>(100));
  EXPECT_EQ("0.1ms", res);
  res = acatl::formattedDuration(std::chrono::duration<double, std::micro>(900));
  EXPECT_EQ("0.9ms", res);
  res = acatl::formattedDuration(std::chrono::duration<double, std::micro>(10000));
  EXPECT_EQ("10.0ms", res);
  res = acatl::formattedDuration(std::chrono::duration<double, std::micro>(100000));
  EXPECT_EQ("0.1s", res);
  res = acatl::formattedDuration(std::chrono::duration<double, std::micro>(900000));
  EXPECT_EQ("0.9s", res);
  res = acatl::formattedDuration(std::chrono::duration<double, std::micro>(1000000));
  EXPECT_EQ("1.0s", res);
  res = acatl::formattedDuration(std::chrono::duration<double, std::micro>(1500000));
  EXPECT_EQ("1.5s", res);
  res = acatl::formattedDuration(std::chrono::duration<double, std::micro>(9000000));
  EXPECT_EQ("9.0s", res);
  res = acatl::formattedDuration(std::chrono::duration<double, std::micro>(60000000));
  EXPECT_EQ("60.0s", res);
  res = acatl::formattedDuration(std::chrono::duration<double, std::micro>(120000000));
  EXPECT_EQ("2.0min", res);
  res = acatl::formattedDuration(std::chrono::duration<double, std::micro>(3540000000));
  EXPECT_EQ("59.0min", res);
  res = acatl::formattedDuration(std::chrono::duration<double, std::micro>(3594000000));
  EXPECT_EQ("59.9min", res);
  res = acatl::formattedDuration(std::chrono::duration<double, std::micro>(3600000000));
  EXPECT_EQ("60.0min", res);
  res = acatl::formattedDuration(std::chrono::duration<double, std::micro>(36000000000));
  EXPECT_EQ("10.0h", res);
  res = acatl::formattedDuration(std::chrono::duration<double, std::micro>(360000000000));
  EXPECT_EQ("100.0h", res);
}

TEST(StringHelperTest, streamCopyTest)
{
  acatl::filesystem::TemporaryDirectoryGuard guard;
  fs::path path = guard.temporaryDirectoryPath();
  acatl::filesystem::current_path(path);

  std::string s("This is the test string");
  std::stringstream ss(s);

  RedirectStdErr rs;
  acatl::stream_copy(ss, std::cerr);

  std::ifstream log(path / "stderr.txt");
  ASSERT_EQ(true, log.good());
  std::string line;
  EXPECT_TRUE(std::getline(log, line));
  EXPECT_EQ("This is the test string", line);
}

TEST(StringHelperTest, loadFileTest)
{
  acatl::filesystem::TemporaryDirectoryGuard guard;
  fs::path path = guard.temporaryDirectoryPath();
  acatl::filesystem::current_path(path);

  {
    std::ofstream file("myfile.txt");
    file << "This is a cool text about something very unusual";
    file.close();
  }

  std::string file_contents = acatl::load_file(path / "myfile.txt");
  EXPECT_EQ("This is a cool text about something very unusual", file_contents);

  std::error_code ec;
  file_contents = acatl::load_file(path / "myfile.txt", ec);
  EXPECT_FALSE(ec);
  EXPECT_EQ("This is a cool text about something very unusual", file_contents);
}

TEST(StringHelperTest, loadFileErrorTest)
{
  EXPECT_THROW(acatl::load_file("check_this_out.txt"), acatl::Exception);

  std::error_code ec;
  acatl::load_file("check_this_out.txt", ec);
  EXPECT_TRUE(ec);
}

TEST(StringHelperTest, replaceAllTest)
{
  std::string s("This is a text with some \n and \t\t \n");
  s = acatl::replaceAll(acatl::replaceAll(s, "\n", "\\n"), "\t", "\\t");
  EXPECT_EQ("This is a text with some \\n and \\t\\t \\n", s);
}

TEST(StringHelperTest, decodeTest)
{
  std::string encoded("wikiPageName=Testpage&wikiPageMarkdown=Markdown%0D%0A%3D%3D%3D%3D%3D%3D%3D%3D%0D%0A%0D%0A-+"
                      "some+tests%0D%0A-+and+more%0D%0A%0D%0A");
  std::string decoded;
  EXPECT_TRUE(acatl::decode(encoded, decoded));
  EXPECT_EQ("wikiPageName=Testpage&wikiPageMarkdown=Markdown\r\n========\r\n\r\n- some tests\r\n- and more\r\n\r\n",
            decoded);

  encoded = "wikiPageName=Testpage&wikiPageMarkdown=Markdown% "
            "%0A%3D%3D%3D%3D%3D%3D%3D%3D%0D%0A%0D%0A-+some+tests%0D%0A-+and+more%0D%0A%0D%0A";
  EXPECT_FALSE(acatl::decode(encoded, decoded));
}
