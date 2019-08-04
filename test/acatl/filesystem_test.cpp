//
//  filesystem_test.cpp
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

#include <fstream>

namespace fs = acatl::filesystem;

TEST(FilesystemPathTest, constructor)
{
  fs::path p1;
  EXPECT_TRUE(p1.empty());

  fs::path p2("/test/path.ext");
  EXPECT_FALSE(p2.empty());

  fs::path p3(std::string("/test/path"));
  EXPECT_FALSE(p3.empty());

  fs::path p4(p1);
  EXPECT_TRUE(p4.empty());

  fs::path p5(p3);
  EXPECT_FALSE(p5.empty());
}

TEST(FilesystemPathTest, assignment)
{
  fs::path p1;
  fs::path p2("/test/path.ext");
  fs::path p3(std::string("/test/path"));
  fs::path p4(p3);

  fs::path p5 = p1;
  EXPECT_TRUE(p5.empty());

  p5 = p2;
  EXPECT_TRUE(p5 == p2);

  p5 = p3;
  EXPECT_TRUE(p5 == p3);

  p5 = p4;
  EXPECT_TRUE(p5 == p3);
  EXPECT_TRUE(p5 == p4);

  p5 = "/absolute/path/test/foo.txt";
  p4 = std::string("/absolute/path/test/foo.txt");
  EXPECT_TRUE(p5 == p4);

  p1.assign("/da/checker/foo.bar");
  EXPECT_TRUE("/da/checker/foo.bar" == p1);
}

TEST(FilesystemPathTest, compare)
{
  fs::path p1;
  fs::path p2("/test/path.ext");
  fs::path p3("a/relative/test/path");
  fs::path p4("/absolute/path");
  fs::path p5("/absolute/path/test/foo.txt");
  fs::path p6("..");

  EXPECT_EQ(p1, p1);
  EXPECT_EQ(p2, p2);
  EXPECT_NE(p1, p2);
  EXPECT_NE(p3, p2);
  EXPECT_EQ("..", p6);
  EXPECT_EQ(p6, "..");
  EXPECT_EQ(p4, std::string("/absolute/path"));
  EXPECT_EQ(std::string("/absolute/path"), p4);
  EXPECT_NE(std::string("/absolute/path/test/foo.txt/"), p5);

  fs::path p7("abc/def/ghi");
  fs::path p8("abc/eef/ghi");
  fs::path p9("abc/cef/ghi");

  EXPECT_TRUE(p8 > p7);
  EXPECT_TRUE(p7 < p8);
  EXPECT_TRUE(p8 >= p7);
  EXPECT_TRUE(p7 <= p8);
  EXPECT_TRUE(p9 < p7);
  EXPECT_TRUE(p9 <= p9);
  EXPECT_TRUE(p9 >= p9);
}

TEST(FilesystemPathTest, append)
{
  fs::path p1;
  p1 /= "/test/path.ext";
  EXPECT_FALSE(p1.empty());
  EXPECT_EQ("/test/path.ext", p1);

  fs::path p2(std::string("a/relative/test/path"));
  fs::path p3("/absolute/path");

  fs::path p4 = p3 / p2;
  EXPECT_EQ("/absolute/path/a/relative/test/path", p4);

  p3 /= p2;
  EXPECT_EQ("/absolute/path/a/relative/test/path", p3);

  p3.append("check");
  EXPECT_EQ("/absolute/path/a/relative/test/path/check", p3);

  fs::path p5;
  p5 /= p2;
  EXPECT_EQ("a/relative/test/path", p5);

  p5 /= fs::path();
  EXPECT_EQ("a/relative/test/path", p5);

  fs::path p6("/this/is/my/test/");
  p6 /= fs::path("dude");
  EXPECT_EQ("/this/is/my/test/dude", p6);

  fs::path p7("/this/is/my/test");
  p7 /= fs::path("/dude");
  EXPECT_EQ("/this/is/my/test/dude", p7);

  fs::path p8 = fs::path("/this") / "is" / "my" / "path" / "foo.bar";
  EXPECT_EQ("/this/is/my/path/foo.bar", p8);
}

TEST(FilesystemPathTest, concat)
{
  fs::path p1;
  p1 += "/test/path.ext";
  EXPECT_FALSE(p1.empty());
  EXPECT_EQ("/test/path.ext", p1);

  fs::path p2(std::string("a/relative/test/path"));
  fs::path p3("/absolute/path");

  p3 += p2;
  EXPECT_EQ("/absolute/patha/relative/test/path", p3);

  p3 += 'X';
  EXPECT_EQ("/absolute/patha/relative/test/pathX", p3);

  p2 += std::string("foobar");
  EXPECT_EQ("a/relative/test/pathfoobar", p2);

  p2.concat("check");
  EXPECT_EQ("a/relative/test/pathfoobarcheck", p2);

  p2.concat("/theend.txt");
  EXPECT_EQ("a/relative/test/pathfoobarcheck/theend.txt", p2);
}

TEST(FilesystemPathTest, clear)
{
  fs::path p1;
  p1.clear();
  EXPECT_TRUE(p1.empty());

  fs::path p2("/test/path.ext");
  p2.clear();
  EXPECT_TRUE(p2.empty());
}

TEST(FilesystemPathTest, removeFilename)
{
  {
    fs::path p("/test/path.ext");
    p.remove_filename();
    EXPECT_EQ("/test", p.string());
  }

  {
    fs::path p("/test");
    p.remove_filename();
    EXPECT_EQ("/", p.string());
  }

  {
    fs::path p("/");
    p.remove_filename();
    EXPECT_EQ("", p.string());
  }
}

TEST(FilesystemPathTest, replaceFilename)
{
  EXPECT_EQ("/bar", fs::path("/foo").replace_filename("bar").string());
  EXPECT_EQ("bar", fs::path("/").replace_filename("bar").string());
}

TEST(FilesystemPathTest, replaceExtension)
{
  {
    fs::path p("relative/path/test/foo.txt");
    EXPECT_EQ("relative/path/test/foo", p.replace_extension().string());
  }

  {
    fs::path p("relative/path/test/foo.txt");
    EXPECT_EQ("relative/path/test/foo.zip", p.replace_extension(".zip").string());
  }

  {
    fs::path p("relative/path/test/foo.txt");
    EXPECT_EQ("relative/path/test/foo.gzip", p.replace_extension("gzip").string());
  }
}

TEST(FilesystemPathTest, swap)
{
  fs::path p1(std::string("a/relative/test/path"));
  fs::path p2("/absolute/path");
  p1.swap(p2);
  EXPECT_EQ("a/relative/test/path", p2);
  EXPECT_EQ("/absolute/path", p1);

  fs::swap(p1, p2);
  EXPECT_EQ("a/relative/test/path", p1);
  EXPECT_EQ("/absolute/path", p2);
}

TEST(FilesystemPathTest, string)
{
  fs::path p1;
  fs::path p2(std::string("a/relative/test/path"));
  fs::path p3("/absolute/path");

  EXPECT_EQ("", std::string(p1.c_str()));
  EXPECT_EQ("a/relative/test/path", p2.string());
  EXPECT_EQ("/absolute/path", static_cast<std::string>(p3));
  EXPECT_EQ("/absolute/path", p3.native());
}

TEST(FilesystemPathTest, iterator)
{
  {
    fs::path p;
    fs::path::iterator iter = p.begin();
    EXPECT_EQ(iter, p.end());

    p = "test";
    iter = p.begin();
    auto end = p.end();
    EXPECT_NE(iter, end);
    EXPECT_EQ("test", iter->string());
    ++iter;
    EXPECT_EQ(iter, end);

    p = "/";
    iter = p.begin();
    end = p.end();
    EXPECT_NE(iter, end);
    EXPECT_EQ("/", iter->string());
    ++iter;
    EXPECT_EQ(iter, end);

    p = "////";
    iter = p.begin();
    end = p.end();
    EXPECT_NE(iter, end);
    EXPECT_EQ("/", iter->string());
    ++iter;
    EXPECT_EQ(".", iter->string());
    ++iter;
    EXPECT_EQ(iter, end);

    p = "/test";
    iter = p.begin();
    end = p.end();
    EXPECT_NE(iter, end);
    EXPECT_EQ("/", iter->string());
    ++iter;
    EXPECT_EQ("test", iter->string());
    ++iter;
    EXPECT_EQ(iter, end);

    p = "test/";
    iter = p.begin();
    end = p.end();
    EXPECT_NE(iter, end);
    EXPECT_EQ("test", iter->string());
    ++iter;
    EXPECT_EQ(".", iter->string());
    ++iter;
    EXPECT_EQ(iter, end);

    p = "/test/";
    iter = p.begin();
    end = p.end();
    EXPECT_NE(iter, end);
    EXPECT_EQ("/", iter->string());
    ++iter;
    EXPECT_EQ("test", iter->string());
    ++iter;
    EXPECT_EQ(".", iter->string());
    ++iter;
    EXPECT_EQ(iter, end);

    p = "/test/path";
    iter = p.begin();
    end = p.end();
    EXPECT_NE(iter, end);
    EXPECT_EQ("/", iter->string());
    ++iter;
    EXPECT_EQ("test", iter->string());
    ++iter;
    EXPECT_EQ("path", iter->string());
    ++iter;
    EXPECT_EQ(iter, end);

    p = "/test/path/";
    iter = p.begin();
    end = p.end();
    EXPECT_NE(iter, end);
    EXPECT_EQ("/", iter->string());
    ++iter;
    EXPECT_EQ("test", iter->string());
    ++iter;
    EXPECT_EQ("path", iter->string());
    ++iter;
    EXPECT_EQ(".", iter->string());
    ++iter;
    EXPECT_EQ(iter, end);

    p = "test/path/";
    iter = p.begin();
    end = p.end();
    EXPECT_NE(iter, end);
    EXPECT_EQ("test", iter->string());
    ++iter;
    EXPECT_EQ("path", iter->string());
    ++iter;
    EXPECT_EQ(".", iter->string());
    ++iter;
    EXPECT_EQ(iter, end);

    p = "test/path";
    iter = p.begin();
    end = p.end();
    EXPECT_NE(iter, end);
    EXPECT_EQ("test", iter->string());
    ++iter;
    EXPECT_EQ("path", iter->string());
    ++iter;
    EXPECT_EQ(iter, end);

    p = "test//path";
    iter = p.begin();
    end = p.end();
    EXPECT_NE(iter, end);
    EXPECT_EQ("test", iter->string());
    ++iter;
    EXPECT_EQ("path", iter->string());
    ++iter;
    EXPECT_EQ(iter, end);
  }

  {
    fs::path p("/absolute///path/test/foo.txt");
    fs::path::iterator iter = p.begin();
    EXPECT_NE(iter, p.end());

    EXPECT_EQ("/", iter->string());
    ++iter;
    EXPECT_EQ("absolute", iter->string());
    ++iter;
    EXPECT_EQ("path", iter->string());
    ++iter;
    EXPECT_EQ("test", iter->string());
    ++iter;
    EXPECT_EQ("foo.txt", iter->string());
    ++iter;
    EXPECT_EQ(iter, p.end());
  }

  {
    fs::path p("relative/path/test/foo.txt");
    fs::path::iterator iter = p.begin();
    EXPECT_NE(iter, p.end());

    EXPECT_EQ("relative", iter->string());
    ++iter;
    EXPECT_EQ("path", iter->string());
    ++iter;
    EXPECT_EQ("test", iter->string());
    ++iter;
    EXPECT_EQ("foo.txt", iter->string());
    ++iter;
    EXPECT_EQ(iter, p.end());
  }

  {
    fs::path p("foo.txt");
    fs::path::iterator iter = p.begin();
    EXPECT_NE(iter, p.end());

    EXPECT_EQ("foo.txt", iter->string());
    ++iter;
    EXPECT_EQ(iter, p.end());
  }

  {
    fs::path p("/foo.txt");
    fs::path::iterator iter = p.begin();
    EXPECT_NE(iter, p.end());

    EXPECT_EQ("/", iter->string());
    ++iter;
    EXPECT_EQ("foo.txt", iter->string());
    ++iter;
    EXPECT_EQ(iter, p.end());
  }

  {
    fs::path p("a/check/");
    fs::path::iterator iter = p.begin();
    EXPECT_NE(iter, p.end());

    EXPECT_EQ("a", iter->string());
    ++iter;
    EXPECT_EQ("check", iter->string());
    ++iter;
    EXPECT_EQ(".", iter->string());
    ++iter;
    EXPECT_EQ(iter, p.end());
  }

  {
    fs::path p("./a/check/..");
    fs::path::iterator iter = p.begin();
    EXPECT_NE(iter, p.end());

    EXPECT_EQ(".", iter->string());
    ++iter;
    EXPECT_EQ("a", iter->string());
    ++iter;
    EXPECT_EQ("check", iter->string());
    ++iter;
    EXPECT_EQ("..", iter->string());
    ++iter;
    EXPECT_EQ(iter, p.end());
  }

  {
    fs::path p(".");
    fs::path::iterator iter = p.begin();
    EXPECT_NE(iter, p.end());

    EXPECT_EQ(".", iter->string());
    ++iter;
    EXPECT_EQ(iter, p.end());
  }

  {
    fs::path p("..");
    fs::path::iterator iter = p.begin();
    EXPECT_NE(iter, p.end());

    EXPECT_EQ("..", iter->string());
    ++iter;
    EXPECT_EQ(iter, p.end());
  }

  {
    fs::path p("/.");
    fs::path::iterator iter = p.begin();
    EXPECT_NE(iter, p.end());

    EXPECT_EQ("/", iter->string());
    ++iter;
    EXPECT_EQ(".", iter->string());
    ++iter;
    EXPECT_EQ(iter, p.end());
  }

  {
    fs::path p("./");
    fs::path::iterator iter = p.begin();
    EXPECT_NE(iter, p.end());

    EXPECT_EQ(".", iter->string());
    ++iter;
    EXPECT_EQ(".", iter->string());
    ++iter;
    EXPECT_EQ(iter, p.end());
  }

  {
    fs::path p("/..");
    fs::path::iterator iter = p.begin();
    EXPECT_NE(iter, p.end());

    EXPECT_EQ("/", iter->string());
    ++iter;
    EXPECT_EQ("..", iter->string());
    ++iter;
    EXPECT_EQ(iter, p.end());
  }

  {
    fs::path p("../");
    fs::path::iterator iter = p.begin();
    EXPECT_NE(iter, p.end());

    EXPECT_EQ("..", iter->string());
    ++iter;
    EXPECT_EQ(".", iter->string());
    ++iter;
    EXPECT_EQ(iter, p.end());
  }

  {
    fs::path p("foo/../bar");
    fs::path::iterator iter = p.begin();
    EXPECT_NE(iter, p.end());

    EXPECT_EQ("foo", iter->string());
    ++iter;
    EXPECT_EQ("..", iter->string());
    ++iter;
    EXPECT_EQ("bar", iter->string());
    ++iter;
    EXPECT_EQ(iter, p.end());
  }

  {
    fs::path p("//net/foo/bar");
    fs::path::iterator iter = p.begin();
    EXPECT_NE(iter, p.end());

    EXPECT_EQ("//net", iter->string());
    ++iter;
    EXPECT_EQ("foo", iter->string());
    ++iter;
    EXPECT_EQ("bar", iter->string());
    ++iter;
    EXPECT_EQ(iter, p.end());
  }

  {
    fs::path p("//net");
    fs::path::iterator iter = p.begin();
    EXPECT_NE(iter, p.end());

    EXPECT_EQ("//net", iter->string());
    ++iter;
    EXPECT_EQ(iter, p.end());
  }

  {
    fs::path p("/absolute/path/test/foo.txt");
    auto iter = p.begin();
    std::vector<fs::path> vec;

    while(iter != p.end()) {
      vec.push_back(*iter);
      ++iter;
    }
    EXPECT_EQ("/", vec[0].string());
    EXPECT_EQ("absolute", vec[1].string());
    EXPECT_EQ("path", vec[2].string());
    EXPECT_EQ("test", vec[3].string());
    EXPECT_EQ("foo.txt", vec[4].string());
  }

  {
    fs::path p("/absolute/path/test/foo.txt");
    auto iter = p.begin();
    std::vector<fs::path> vec;

    for(const fs::path& pa : p) {
      vec.push_back(pa);
    }
    EXPECT_EQ("/", vec[0].string());
    EXPECT_EQ("absolute", vec[1].string());
    EXPECT_EQ("path", vec[2].string());
    EXPECT_EQ("test", vec[3].string());
    EXPECT_EQ("foo.txt", vec[4].string());
  }
}

TEST(FilesystemPathTest, iteratorDecrement)
{
  {
    fs::path p("/");
    auto iter = p.end();
    EXPECT_EQ(p.end(), iter);
    EXPECT_NE(p.begin(), iter);

    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_EQ(p.begin(), iter);
    EXPECT_EQ("/", (*iter).string());
  }

  {
    fs::path p("///");
    auto iter = p.end();
    EXPECT_EQ(p.end(), iter);
    EXPECT_NE(p.begin(), iter);

    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ(".", (*iter).string());
    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_EQ(p.begin(), iter);
    EXPECT_EQ("/", (*iter).string());
  }

  {
    fs::path p("path/test");
    auto iter = p.end();
    EXPECT_EQ(p.end(), iter);
    EXPECT_NE(p.begin(), iter);

    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ("test", (*iter).string());
    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_EQ(p.begin(), iter);
    EXPECT_EQ("path", (*iter).string());
  }

  {
    fs::path p("path////test");
    auto iter = p.end();
    EXPECT_EQ(p.end(), iter);
    EXPECT_NE(p.begin(), iter);

    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ("test", (*iter).string());
    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_EQ(p.begin(), iter);
    EXPECT_EQ("path", (*iter).string());
  }

  {
    fs::path p("/path/test");
    auto iter = p.end();
    EXPECT_EQ(p.end(), iter);
    EXPECT_NE(p.begin(), iter);

    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ("test", (*iter).string());
    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ("path", (*iter).string());
    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_EQ(p.begin(), iter);
    EXPECT_EQ("/", (*iter).string());
  }

  {
    fs::path p("/path/test/");
    auto iter = p.end();
    EXPECT_EQ(p.end(), iter);
    EXPECT_NE(p.begin(), iter);

    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ(".", (*iter).string());
    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ("test", (*iter).string());
    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ("path", (*iter).string());
    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_EQ(p.begin(), iter);
    EXPECT_EQ("/", (*iter).string());
  }

  {
    fs::path p("/path/test///");
    auto iter = p.end();
    EXPECT_EQ(p.end(), iter);
    EXPECT_NE(p.begin(), iter);

    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ(".", (*iter).string());
    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ("test", (*iter).string());
    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ("path", (*iter).string());
    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_EQ(p.begin(), iter);
    EXPECT_EQ("/", (*iter).string());
  }

  {
    fs::path p("//net/path/test/");
    auto iter = p.end();
    EXPECT_EQ(p.end(), iter);
    EXPECT_NE(p.begin(), iter);

    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ(".", (*iter).string());
    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ("test", (*iter).string());
    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ("path", (*iter).string());
    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_EQ(p.begin(), iter);
    EXPECT_EQ("//net", (*iter).string());
  }
}

TEST(FilesystemPathTest, iteratorMix)
{
  {
    fs::path p("//net/path/test/");
    auto iter = p.end();
    EXPECT_EQ(p.end(), iter);
    EXPECT_NE(p.begin(), iter);

    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ(".", (*iter).string());
    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ("test", (*iter).string());
    --iter;
    ++iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ("test", (*iter).string());
    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ("path", (*iter).string());
    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_EQ(p.begin(), iter);
    EXPECT_EQ("//net", (*iter).string());
  }

  {
    fs::path p("//net/path/test/");
    auto iter = p.begin();
    EXPECT_NE(p.end(), iter);
    EXPECT_EQ(p.begin(), iter);
    EXPECT_EQ("//net", (*iter).string());
    ++iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ("path", (*iter).string());
    ++iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ("test", (*iter).string());
    ++iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ(".", (*iter).string());
    ++iter;
    EXPECT_EQ(p.end(), iter);
    EXPECT_NE(p.begin(), iter);

    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ(".", (*iter).string());
    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ("test", (*iter).string());
    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_NE(p.begin(), iter);
    EXPECT_EQ("path", (*iter).string());
    --iter;
    EXPECT_NE(p.end(), iter);
    EXPECT_EQ(p.begin(), iter);
    EXPECT_EQ("//net", (*iter).string());
  }
}

TEST(FilesystemPathTest, stream)
{
  {
    fs::path p("/absolute/path/test/foo.txt");
    std::stringstream ss;
    ss << p;
    EXPECT_EQ("\"/absolute/path/test/foo.txt\"", ss.str());
  }

  {
    fs::path p;
    std::istringstream is("\"/absolute/path/test/foo.txt\"");
    is >> p;
    EXPECT_EQ("/absolute/path/test/foo.txt", p.string());
  }
}

TEST(FilesystemPathTest, decomposition)
{
  {
    fs::path p("/");

    EXPECT_EQ("", p.root_name().string());
    EXPECT_EQ("/", p.root_directory().string());
    EXPECT_EQ("/", p.root_path().string());
    EXPECT_EQ("", p.relative_path().string());
    EXPECT_EQ("", p.parent_path().string());
    EXPECT_EQ("/", p.filename().string());
    EXPECT_EQ("", p.extension().string());
  }

  {
    fs::path p(".");

    EXPECT_EQ("", p.root_name().string());
    EXPECT_EQ("", p.root_directory().string());
    EXPECT_EQ("", p.root_path().string());
    EXPECT_EQ(".", p.relative_path().string());
    EXPECT_EQ("", p.parent_path().string());
    EXPECT_EQ(".", p.filename().string());
    EXPECT_EQ("", p.extension().string());
  }

  {
    fs::path p("..");

    EXPECT_EQ("", p.root_name().string());
    EXPECT_EQ("", p.root_directory().string());
    EXPECT_EQ("", p.root_path().string());
    EXPECT_EQ("..", p.relative_path().string());
    EXPECT_EQ("", p.parent_path().string());
    EXPECT_EQ("..", p.filename().string());
    EXPECT_EQ("", p.extension().string());
  }

  {
    fs::path p("/.");

    EXPECT_EQ("", p.root_name().string());
    EXPECT_EQ("/", p.root_directory().string());
    EXPECT_EQ("/", p.root_path().string());
    EXPECT_EQ(".", p.relative_path().string());
    EXPECT_EQ("/", p.parent_path().string());
    EXPECT_EQ(".", p.filename().string());
    EXPECT_EQ("", p.extension().string());
  }

  {
    fs::path p("/..");

    EXPECT_EQ("", p.root_name().string());
    EXPECT_EQ("/", p.root_directory().string());
    EXPECT_EQ("/", p.root_path().string());
    EXPECT_EQ("..", p.relative_path().string());
    EXPECT_EQ("/", p.parent_path().string());
    EXPECT_EQ("..", p.filename().string());
    EXPECT_EQ("", p.extension().string());
  }

  {
    fs::path p("foo");

    EXPECT_EQ("", p.root_name().string());
    EXPECT_EQ("", p.root_directory().string());
    EXPECT_EQ("", p.root_path().string());
    EXPECT_EQ("foo", p.relative_path().string());
    EXPECT_EQ("", p.parent_path().string());
    EXPECT_EQ("foo", p.filename().string());
    EXPECT_EQ("", p.extension().string());
  }

  {
    fs::path p("/absolute/path/test/foo.txt");

    EXPECT_EQ("", p.root_name().string());
    EXPECT_EQ("/", p.root_directory().string());
    EXPECT_EQ("/", p.root_path().string());
    EXPECT_EQ("absolute/path/test/foo.txt", p.relative_path().string());
    EXPECT_EQ("/absolute/path/test", p.parent_path().string());
    EXPECT_EQ("foo.txt", p.filename().string());
    EXPECT_EQ(".txt", p.extension().string());
  }

  {
    fs::path p("foo.txt");

    EXPECT_EQ("", p.root_name().string());
    EXPECT_EQ("", p.root_directory().string());
    EXPECT_EQ("", p.root_path().string());
    EXPECT_EQ("foo.txt", p.relative_path().string());
    EXPECT_EQ("", p.parent_path().string());
    EXPECT_EQ("foo.txt", p.filename().string());
    EXPECT_EQ(".txt", p.extension().string());
  }

  {
    fs::path p("/foo.txt");

    EXPECT_EQ("", p.root_name().string());
    EXPECT_EQ("/", p.root_directory().string());
    EXPECT_EQ("/", p.root_path().string());
    EXPECT_EQ("foo.txt", p.relative_path().string());
    EXPECT_EQ("/", p.parent_path().string());
    EXPECT_EQ("foo.txt", p.filename().string());
    EXPECT_EQ(".txt", p.extension().string());
  }

  {
    fs::path p("//net/absolute/path/test/foo.");

    EXPECT_EQ("//net", p.root_name().string());
    EXPECT_EQ("/", p.root_directory().string());
    EXPECT_EQ("//net/", p.root_path().string());
    EXPECT_EQ("absolute/path/test/foo.", p.relative_path().string());
    EXPECT_EQ("//net/absolute/path/test", p.parent_path().string());
    EXPECT_EQ("foo.", p.filename().string());
    EXPECT_EQ(".", p.extension().string());
  }

  {
    fs::path p("///foo///");

    EXPECT_EQ("", p.root_name().string());
    EXPECT_EQ("/", p.root_directory().string());
    EXPECT_EQ("/", p.root_path().string());
    EXPECT_EQ("foo///", p.relative_path().string());
    EXPECT_EQ("///foo", p.parent_path().string());
    EXPECT_EQ(".", p.filename().string());
    EXPECT_EQ("", p.extension().string());
  }

  {
    fs::path p("//net");

    EXPECT_EQ("//net", p.root_name().string());
    EXPECT_EQ("", p.root_directory().string());
    EXPECT_EQ("//net", p.root_path().string());
    EXPECT_EQ("", p.relative_path().string());
    EXPECT_EQ("", p.parent_path().string());
    EXPECT_EQ("//net", p.filename().string());
    EXPECT_EQ("", p.extension().string());
  }

  {
    fs::path p("relative/path/test/foo.txt");

    EXPECT_EQ("", p.root_name().string());
    EXPECT_EQ("", p.root_directory().string());
    EXPECT_EQ("", p.root_path().string());
    EXPECT_EQ("relative/path/test/foo.txt", p.relative_path().string());
    EXPECT_EQ("relative/path/test", p.parent_path().string());
    EXPECT_EQ("foo.txt", p.filename().string());
  }

  {
    fs::path p = "foo.bar.baz.tar";
    std::vector<std::string> parts;
    for(; !p.extension().empty(); p = p.stem()) {
      parts.push_back(p.extension().string());
    }
    ASSERT_EQ(3u, parts.size());
    EXPECT_EQ(".tar", parts[0]);
    EXPECT_EQ(".baz", parts[1]);
    EXPECT_EQ(".bar", parts[2]);
  }
}

TEST(FilesystemPathTest, hash)
{
  {
    fs::path p("relative/path/test/foo.txt");
    fs::path p1("relative/path/test/foo.txt");

    EXPECT_EQ(fs::hash_value(p), fs::hash_value(p1));
  }

  {
    fs::path p("relative/path/test/foo.txt");
    fs::path p1("absolute/path/test/foo.");

    EXPECT_NE(fs::hash_value(p), fs::hash_value(p1));
  }
}

TEST(FilesystemPathTest, filesystemException)
{
  {
    fs::filesystem_error ex("This is a test", std::error_code(EBADF, std::system_category()));
    EXPECT_EQ(EBADF, ex.code().value());
    EXPECT_TRUE(ex.path1().empty());
    EXPECT_TRUE(ex.path2().empty());
  }

  {
    fs::filesystem_error ex(
      "This is another test", fs::path("/my/cool/file.txt"), std::error_code(EPIPE, std::system_category()));
    EXPECT_EQ(EPIPE, ex.code().value());
    EXPECT_FALSE(ex.path1().empty());
    EXPECT_EQ("/my/cool/file.txt", ex.path1());
    EXPECT_TRUE(ex.path2().empty());
  }

  {
    fs::filesystem_error ex("This is yet another test",
                            fs::path("/my/cool/file1.txt"),
                            fs::path("/my/cool/file2.txt"),
                            std::error_code(EINVAL, std::system_category()));
    EXPECT_EQ(EINVAL, ex.code().value());
    EXPECT_FALSE(ex.path1().empty());
    EXPECT_EQ("/my/cool/file1.txt", ex.path1());
    EXPECT_FALSE(ex.path2().empty());
    EXPECT_EQ("/my/cool/file2.txt", ex.path2());
  }
}

TEST(FilesystemPrmsTest, basic)
{
  fs::perms prms = fs::perms::owner_exec | fs::perms::owner_read | fs::perms::owner_write;
  EXPECT_EQ(fs::perms::owner_all, prms);

  prms |= fs::perms::group_all;
  prms |= fs::perms::others_all;
  EXPECT_EQ(fs::perms::all, prms);

  prms &= fs::perms::group_all;
  EXPECT_EQ(fs::perms::group_all, prms);
}

TEST(FilesystemFileStatusTest, basic)
{
  fs::file_status fs(fs::file_type::regular, fs::perms::all);
  EXPECT_EQ(fs::file_type::regular, fs.type());
  EXPECT_EQ(fs::perms::all, fs.permissions());

  fs.type(fs::file_type::block);
  fs.permissions(fs::perms::owner_exec | fs::perms::owner_read | fs::perms::owner_write);

  EXPECT_EQ(fs::file_type::block, fs.type());
  EXPECT_EQ(fs::perms::owner_all, fs.permissions());
}

TEST(FilesystemStatusTest, basic)
{
  fs::TemporaryDirectoryGuard guard;
  fs::path dir = guard.temporaryDirectoryPath();
  std::error_code ec;
  fs::file_status stat = fs::status(dir, ec);
  EXPECT_FALSE(ec);
  EXPECT_EQ(fs::file_type::directory, stat.type());
  EXPECT_EQ(fs::perms::owner_all, stat.permissions() & fs::perms::owner_all);

  fs::path file = dir / "test.txt";
  std::ofstream f(file);
  EXPECT_TRUE(f.is_open());
  stat = fs::status(file, ec);
  EXPECT_FALSE(ec);
  EXPECT_EQ(fs::file_type::regular, stat.type());
  EXPECT_NE(fs::perms::owner_all, stat.permissions() & fs::perms::owner_all);
  EXPECT_EQ(fs::perms::owner_read, stat.permissions() & fs::perms::owner_read);
  EXPECT_EQ(fs::perms::owner_write, stat.permissions() & fs::perms::owner_write);
}

TEST(FilesystemStatusTest, error)
{
  std::error_code ec;
  fs::file_status stat = fs::status(fs::path("does") / "not" / "exist", ec);
  EXPECT_TRUE(ec);
  EXPECT_EQ(fs::file_type::not_found, stat.type());
}

TEST(FilesystemDirentryTest, basic)
{
  fs::TemporaryDirectoryGuard guard;
  fs::path dir = guard.temporaryDirectoryPath();
  fs::path file = dir / "test.txt";
  std::ofstream f(file);
  EXPECT_TRUE(f.is_open());

  fs::directory_entry entry(file);
  EXPECT_EQ(file, entry.path());
  std::error_code ec;
  fs::file_status stat = entry.status(ec);
  EXPECT_FALSE(ec);
  EXPECT_EQ(fs::file_type::regular, stat.type());
  EXPECT_NE(fs::perms::owner_all, stat.permissions() & fs::perms::owner_all);
  EXPECT_EQ(fs::perms::owner_read, stat.permissions() & fs::perms::owner_read);
  EXPECT_EQ(fs::perms::owner_write, stat.permissions() & fs::perms::owner_write);

  fs::directory_entry entry2("/my/cool/path/test.txt");
  EXPECT_EQ("/my/cool/path/test.txt", entry2.path().string());
}

TEST(FilesystemDirentryTest, compare)
{
  fs::path p1("/my/cool/path/test.txt");
  fs::path p2("/my/other/cool/path/test.txt");
  fs::path p3("/my/cool/a/path/test.txt");
  fs::path p4("/my/cool/b/path/test.txt");

  EXPECT_TRUE(fs::directory_entry(p1) == fs::directory_entry(p1));
  EXPECT_TRUE(fs::directory_entry(p1) != fs::directory_entry(p2));
  EXPECT_TRUE(fs::directory_entry(p3) < fs::directory_entry(p4));
  EXPECT_TRUE(fs::directory_entry(p4) > fs::directory_entry(p3));
}

TEST(FilesystemDirentryTest, assign)
{
  fs::path p1("/my/cool/path/test.txt");
  fs::path p2("/my/other/cool/path/test.txt");

  fs::directory_entry entry = fs::directory_entry(p1);
  fs::directory_entry entry2 = fs::directory_entry(p2);

  EXPECT_NE(entry, entry2);
  entry = entry2;
  EXPECT_EQ(entry, entry2);

  entry.assign(p1);
  EXPECT_NE(p2, entry.path());
  EXPECT_EQ(p1, entry.path());
}

TEST(FilesystemDirentryTest, replace)
{
  fs::directory_entry entry("/my/cool/path/test.txt");
  entry.replace_filename("check.zip");
  EXPECT_EQ("/my/cool/path/check.zip", entry.path().string());
}

TEST(FilesystemDirIter, iterator)
{
  fs::TemporaryDirectoryGuard guard;
  fs::path dir = guard.temporaryDirectoryPath();

  fs::path file = dir;

  for(uint8_t n = 0; n < 10; ++n) {
    std::ofstream f((file / "test").concat(std::to_string(n)).concat(".txt"));
  }

  std::vector<fs::path> paths;
  fs::directory_iterator iter(dir);

  for(const auto& entry : iter) {
    paths.push_back(entry.path());
  }
  EXPECT_EQ(10u, paths.size());
  EXPECT_EQ(dir.string(), paths[0].parent_path().string());
}

TEST(FilesystemTest, tempDirTest)
{
  fs::path dir = fs::unique_temp_directory_path();
  EXPECT_TRUE(fs::exists(dir));
  fs::remove(dir);
  std::error_code ec;
  EXPECT_FALSE(fs::exists(dir, ec));
}

TEST(FilesystemTest, tempDirGuardTest)
{
  fs::path path;
  {
    fs::TemporaryDirectoryGuard guard;
    path = guard.temporaryDirectoryPath();
    EXPECT_TRUE(fs::exists(path));
  }
  std::error_code ec;
  EXPECT_FALSE(fs::exists(path, ec));
}

TEST(FilesytemTest, removeNonEmptyDirTest)
{
  fs::TemporaryDirectoryGuard guard;
  fs::path path = guard.temporaryDirectoryPath();

  for(int i = 0; i < 10; ++i) {
    std::ofstream file((path / "test").concat(std::to_string(i)).concat(".txt"));
    EXPECT_TRUE(file.is_open());
  }

  std::error_code ec;
  fs::remove(path, ec);
  EXPECT_TRUE(ec);
}

TEST(FilesystemTest, changeAndCheckCurWorkDir)
{
  fs::TemporaryDirectoryGuard guard;
  fs::path path = guard.temporaryDirectoryPath();

  fs::current_path(path);
  EXPECT_EQ(fs::canonical(fs::absolute(path)), fs::current_path());

  EXPECT_THROW(fs::current_path("/some/fantasy/path"), fs::filesystem_error);
}

TEST(FilesystemTest, canonical)
{
  fs::TemporaryDirectoryGuard guard;
  fs::path path = guard.temporaryDirectoryPath();

  EXPECT_THROW(fs::canonical("/some/fantasy/path"), fs::filesystem_error);
  std::error_code ec;
  fs::canonical("/some/fantasy/path", ec);
  EXPECT_TRUE(ec);

  fs::path end = *(--path.end());
  fs::path check = path / ".." / end;
  EXPECT_EQ(fs::canonical(path), fs::canonical(check));
}

TEST(FilesystemTest, remove_all)
{
  fs::TemporaryDirectoryGuard guard;
  fs::path path = guard.temporaryDirectoryPath();

  fs::current_path(path);

  for(int i = 0; i < 10; ++i) {
    std::ofstream file((path / "test").concat(std::to_string(i)).concat(".txt"));
    EXPECT_TRUE(file.is_open());
  }

  fs::create_directory(path / "deep");
  for(int i = 0; i < 10; ++i) {
    std::ofstream file((path / "deep" / "test").concat(std::to_string(i)).concat(".txt"));
    EXPECT_TRUE(file.is_open());
  }

  fs::current_path("..");
  uintmax_t sum = fs::remove_all(path);
  EXPECT_EQ(22u, sum);
  std::error_code ec;
  EXPECT_FALSE(fs::exists(path, ec));
  EXPECT_FALSE(ec);
}

TEST(FilesystemTest, is_directory)
{
  fs::TemporaryDirectoryGuard guard;
  fs::path path = guard.temporaryDirectoryPath();

  EXPECT_TRUE(fs::is_directory(path));

  std::ofstream file((path / "test").concat(".txt"));
  EXPECT_TRUE(file.is_open());
  EXPECT_FALSE(fs::is_directory((path / "test").concat(".txt")));
}

TEST(FilesystemTest, is_regular)
{
  fs::TemporaryDirectoryGuard guard;
  fs::path path = guard.temporaryDirectoryPath();

  EXPECT_FALSE(fs::is_regular_file(path));

  std::ofstream file((path / "test").concat(".txt"));
  EXPECT_TRUE(file.is_open());
  EXPECT_TRUE(fs::is_regular_file((path / "test").concat(".txt")));
}

TEST(FilesystemTest, is_empty)
{
  fs::TemporaryDirectoryGuard guard;
  fs::path path = guard.temporaryDirectoryPath();

  EXPECT_TRUE(fs::is_empty(path));

  fs::path f((path / "test").concat(".txt"));
  std::ofstream file(f);
  EXPECT_TRUE(file.is_open());
  EXPECT_TRUE(fs::is_empty(f));
  file << "This is a text" << std::endl;
  EXPECT_FALSE(fs::is_empty(f));
  EXPECT_EQ(15u, fs::file_size(f));

  EXPECT_FALSE(fs::is_empty(path));

  std::error_code ec;
  EXPECT_FALSE(fs::is_empty(fs::path("dumb") / "path", ec));
  EXPECT_TRUE(ec);
}

TEST(FilesystemTest, createDirectory)
{
  fs::TemporaryDirectoryGuard guard;
  fs::path path = guard.temporaryDirectoryPath();

  std::error_code ec;
  EXPECT_TRUE(fs::create_directory(path / "test", ec));
  EXPECT_FALSE(ec);
  EXPECT_TRUE(fs::is_directory(path / "test"));

  // trying to create an existing directory is not an error
  EXPECT_TRUE(fs::create_directory(path / "test", ec));

  fs::path f((path / "test" / "check").concat(".txt"));
  std::ofstream file(f);
  EXPECT_TRUE(file.is_open());

  EXPECT_FALSE(fs::create_directory(f, ec));
  EXPECT_EQ(std::errc::not_a_directory, ec);
  EXPECT_THROW(fs::create_directory(f), fs::filesystem_error);

  EXPECT_FALSE(fs::create_directory(fs::path(), ec));

  EXPECT_FALSE(fs::create_directory(path / "test" / "deep" / "create", ec));
  EXPECT_EQ(std::errc::not_a_directory, ec);

  EXPECT_TRUE(fs::create_directories(path / "test" / "deep" / "create", ec));
}

TEST(FilesystemTest, recursiveDirectoryIterator)
{
  fs::TemporaryDirectoryGuard guard;
  fs::path path = guard.temporaryDirectoryPath();

  std::error_code ec;
  fs::recursive_directory_iterator iter(fs::path("this") / "path" / "does" / "not" / "exists", ec);
  EXPECT_TRUE(ec);

  iter = fs::recursive_directory_iterator(path);
  EXPECT_EQ(fs::recursive_directory_iterator(), iter);

  fs::create_directory(path / "deep");
  iter = fs::recursive_directory_iterator(path);

  size_t count(0);
  size_t maxDepth(0);
  iter = fs::recursive_directory_iterator(path);
  std::for_each(iter, fs::recursive_directory_iterator(), [&](auto&& entry) {
    if(fs::is_directory(entry)) {
    }
    ++count;
    maxDepth = std::max(maxDepth, static_cast<size_t>(iter.depth()));
  });
  EXPECT_EQ(1u, count);
  EXPECT_EQ(1u, maxDepth);

  for(int i = 0; i < 10; ++i) {
    std::ofstream file((path / "test").concat(std::to_string(i)).concat(".txt"));
    EXPECT_TRUE(file.is_open());
  }

  for(int i = 0; i < 10; ++i) {
    std::ofstream file((path / "deep" / "test").concat(std::to_string(i)).concat(".txt"));
    EXPECT_TRUE(file.is_open());
  }

  fs::create_directory(path / "deep" / "xavier");

  size_t numberDirectories(0);
  count = 0;
  maxDepth = 0;
  iter = fs::recursive_directory_iterator(path);
  std::for_each(iter, fs::recursive_directory_iterator(), [&](auto&& entry) {
    if(fs::is_directory(entry)) {
      ++numberDirectories;
    }
    ++count;
    maxDepth = std::max(maxDepth, static_cast<size_t>(iter.depth()));
  });
  EXPECT_EQ(22u, count);
  EXPECT_EQ(2u, numberDirectories);
  EXPECT_EQ(2u, maxDepth);

  for(int i = 0; i < 10; ++i) {
    std::ofstream file((path / "deep" / "xavier" / "test").concat(std::to_string(i)).concat(".txt"));
    EXPECT_TRUE(file.is_open());
  }

  count = 0;
  maxDepth = 0;
  iter = fs::recursive_directory_iterator(path);
  std::for_each(iter, fs::recursive_directory_iterator(), [&](auto&& /*ignored*/) {
    ++count;
    maxDepth = std::max(maxDepth, static_cast<size_t>(iter.depth()));
  });
  EXPECT_EQ(32u, count);
  EXPECT_EQ(3u, maxDepth);

  count = 0;
  maxDepth = 0;
  iter = fs::recursive_directory_iterator(path / "deep" / "xavier");
  std::for_each(iter, fs::recursive_directory_iterator(), [&](auto&& /*ignored*/) {
    ++count;
    maxDepth = std::max(maxDepth, static_cast<size_t>(iter.depth()));
  });
  EXPECT_EQ(10u, count);
  EXPECT_EQ(1u, maxDepth);
}

TEST(FilesystemTest, readSymbolicLink)
{
  // tbd
}

TEST(FilesystemTest, equivalentTest)
{
  acatl::filesystem::TemporaryDirectoryGuard guard;
  fs::path path = guard.temporaryDirectoryPath();
  acatl::filesystem::current_path(path);

  EXPECT_TRUE(fs::equivalent(path, fs::current_path()));

  path /= "check.txt";

  std::error_code ec;
  EXPECT_FALSE(fs::equivalent(path, fs::current_path(), ec));
  EXPECT_TRUE(ec);

  std::ofstream file(path);
  EXPECT_TRUE(file.is_open());
  EXPECT_FALSE(fs::equivalent(path, fs::current_path(), ec));
  EXPECT_FALSE(ec);
}

TEST(FilesystemTest, setPermissions)
{
  // tbd
}

TEST(FilesystemTest, createSymlink)
{
  fs::TemporaryDirectoryGuard guard;
  fs::path path = guard.temporaryDirectoryPath();

  std::error_code ec;
  fs::create_symlink(path / "not_existent", path / "link", ec);

  EXPECT_FALSE(ec);
  EXPECT_TRUE(fs::is_symlink(path / "link"));

  fs::remove(path / "link");
  EXPECT_FALSE(fs::is_symlink(path / "link", ec));

  fs::create_directory(path / "mydir");

  ec.clear();
  fs::create_symlink(path / "mydir", path / "link", ec);
  EXPECT_FALSE(ec);
  EXPECT_TRUE(fs::is_symlink(path / "link"));
}
