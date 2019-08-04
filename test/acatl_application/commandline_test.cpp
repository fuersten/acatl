//
//  commandline_test.cpp
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

#include <acatl_application/command_line_options.h>

TEST(CommandlineTest, optionGroups)
{
  acatl::StringVector args = {"MyApp", "--host=127.0.0.1", "-p4711"};

  // clang-format off
  acatl::OptionGroup group1{"help", {
    {"", "help", 0, 1, "display help"},
    {"", "version", "display version"}
  }};

  acatl::OptionGroup group2{"net", {
    {"h", "host", "<HOST>", "host to connect to"},
    {"p", "port", "<PORT>", 0, 1, "port to connect to"}
  }};
  // clang-format on

  std::stringstream ss;

  EXPECT_FALSE(group1.parse("MyApp", args, ss));
  EXPECT_EQ(0, group1.option("help").count());
  EXPECT_TRUE(group2.parse("MyApp", args, ss));
  EXPECT_EQ(1, group2.option("host").count());
  EXPECT_EQ(1, group2.option("port").count());
  EXPECT_EQ("127.0.0.1", group2.option("host").value<std::string>());
  EXPECT_EQ(4711u, group2.option("port").value<uint16_t>());
  ss.str("");
  ss.clear();

  args = {"MyApp", "--version"};
  EXPECT_TRUE(group1.parse("MyApp", args, ss));
  EXPECT_FALSE(group2.parse("MyApp", args, ss));
  EXPECT_EQ(1, group1.option("version").count());
  EXPECT_EQ(0, group1.option("help").count());
  EXPECT_EQ(0, group2.option("host").count());
}

TEST(CommandlineTest, basic)
{
  acatl::StringVector args = {"MyApp", "--host=127.0.0.1", "-p4711"};

  // clang-format off
    acatl::CommandLineOptions options("MyApp", {
        {
            "version", {
                {"v", "", 0, 1, "display the version"}
            }
        },
        {
            "options", {
                {"h", "host", "<HOST>", 1, 1, "Host to connect to"},
                {"p", "port", "<PORT>", 1, 1, "Port to connect to"}
            }
        }
    });
  // clang-format on

  EXPECT_TRUE(options.parse(args, std::cerr));
  EXPECT_THROW(options.option("v"), acatl::BadOption);
  EXPECT_EQ(0, options.untaggedOptionsCount());
  EXPECT_EQ(0, options.count("v"));
  EXPECT_EQ(1, options.count("host"));
  EXPECT_EQ(1, options.option("host").count());
  EXPECT_EQ(1, options.option("port").count());
  EXPECT_EQ(1, options.option("h").count());
  EXPECT_EQ(1, options.option("p").count());
  EXPECT_EQ(1, options.count("p"));
  EXPECT_EQ("127.0.0.1", options.option("host").value<std::string>(0));
  EXPECT_EQ(4711, options.option("port").value<int>());
}

TEST(CommandlineTest, positionDependent)
{
  acatl::StringVector args = {"MyApp", "file1", "file2", "file3", "file4"};

  // clang-format off
    acatl::CommandLineOptions options("MyApp", {
        {
            "files", {
                {"", "", "<FILE>", 1, 10, "Files to do something with"}
            }
        }
    });
  // clang-format on

  EXPECT_TRUE(options.parse(args, std::cerr));
  EXPECT_EQ(1, options.untaggedOptionsCount());
  EXPECT_EQ(4, options.count(0));
  EXPECT_EQ(4, options.untaggedOption(0).count());
  EXPECT_EQ("file1", options.untaggedOption(0).value<acatl::filesystem::path>(0));
  EXPECT_EQ("file2", options.untaggedOption(0).value<acatl::filesystem::path>(1));
  EXPECT_EQ("file3", options.untaggedOption(0).value<acatl::filesystem::path>(2));
  EXPECT_EQ("file4", options.untaggedOption(0).value<acatl::filesystem::path>(3));
}

TEST(CommandlineTest, format)
{
  // clang-format off
    acatl::CommandLineOptions options("MyApp", {
        {
            "help", {
                {"v", "", 0, 1, "display the version"}
            }
        },
        {
            "options", {
                {"h", "host", "<HOST>", 1, 1, "Host to connect to"},
                {"p", "port", "<PORT>", 1, 1, "Port to connect to"}
            }
        }
    });
  // clang-format on

  std::stringstream ss;
  options.usage(ss);

  std::string expect = R"(Usage: MyApp [ [-v] ] | [ -h <HOST> -p <PORT> ]
help:
  -v                                  display the version

options:
  -h, --host=<HOST>                   Host to connect to
  -p, --port=<PORT>                   Port to connect to

)";

  EXPECT_EQ(expect, ss.str());
}

TEST(CommandlineTest, nOption)
{
  // clang-format off
    acatl::CommandLineOptions options("MyApp", {
        {
            "version", {
                {"v", "", 0, 10, "verbose"}
            }
        }
    });
  // clang-format on

  {
    acatl::StringVector args = {"MyApp", "-v"};
    EXPECT_TRUE(options.parse(args, std::cerr));
    EXPECT_EQ(1, options.option("v").count());
  }

  {
    acatl::StringVector args = {"MyApp", "-vvvvv"};
    EXPECT_TRUE(options.parse(args, std::cerr));
    EXPECT_EQ(5, options.option("v").count());
  }

  {
    acatl::StringVector args = {"MyApp", "-vvvvvvvvvvv"};
    std::stringstream ss;
    EXPECT_FALSE(options.parse(args, ss));
    EXPECT_EQ("MyApp: extraneous option -v\n", ss.str());
  }
}
