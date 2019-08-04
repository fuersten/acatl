//
//  exception_test.cpp
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

#include <fstream>
#include <iostream>

#include <acatl/exception.h>
#include <acatl/filesystem.h>
#include <acatl/logging.h>
#include <acatl/string_helper.h>

#include <test/acatl/test_helper.h>

namespace fs = acatl::filesystem;

ACATL_DECLARE_EXCEPTION(FilesystemException, acatl::Exception);

TEST(ExceptionTest, DerivedExceptionTest)
{
  try {
    throw FilesystemException(ENOENT, "Filesystem", __FILE__, __LINE__);
  } catch(const acatl::Exception& ex) {
    EXPECT_EQ(ENOENT, ex.code().value());
  }
}

TEST(ExceptionTest, errnoTest)
{
  errno = EWOULDBLOCK;
  std::string txt = acatl::errnoText();
  EXPECT_EQ(::strerror(EWOULDBLOCK), txt);
}

TEST(ExceptionTest, sysExceptionTest)
{
  errno = EWOULDBLOCK;
  try {
    acatl::throwSysError("mydomain", __FILE__, __LINE__);
  } catch(const acatl::Exception& ex) {
    EXPECT_EQ(EWOULDBLOCK, ex.code().value());
    EXPECT_EQ(std::string("mydomain: ") + acatl::errnoText(), ex.what());
  }
}

TEST(ExceptionTest, evaluateExceptionTest)
{
  acatl::filesystem::TemporaryDirectoryGuard guard;
  fs::path path = guard.temporaryDirectoryPath();
  acatl::filesystem::current_path(path);

  RedirectStdErr rse;

  acatl::Logging::init(json(), true);

  try {
    throw FilesystemException(ENOENT, "Filesystem", __FILE__, __LINE__);
  } catch(...) {
    acatl::evaluateException();
  }

  try {
    throw std::runtime_error("This is my message");
  } catch(...) {
    acatl::evaluateException();
  }

  try {
    throw std::string("This is my custom exception");
  } catch(...) {
    acatl::evaluateException();
  }

  std::ifstream log(std::string("./stderr.txt"));
  EXPECT_TRUE(log.good());
  std::string line;
  int line_count(0);
  while(std::getline(log, line).good()) {
    acatl::StringVector parts;
    acatl::split(line, '|', parts);
    EXPECT_EQ(5U, parts.size());

    switch(line_count) {
      case 0: {
        EXPECT_EQ("exception caught: [2] Filesystem: No such file or directory", parts[3]);
        parts[4] = parts[4].substr(parts[4].find_last_of('/') + 1);
        parts[4] = parts[4].substr(0, parts[4].find_first_of(':'));
        EXPECT_EQ("exception_test.cpp", parts[4]);
        break;
      }
      case 1: {
        EXPECT_EQ("std::exception caught: This is my message", parts[3]);
        break;
      }
      case 2: {
        EXPECT_EQ("unknown exception caught", parts[3]);
        break;
      }
    }
    ++line_count;
  }
  log.close();
  EXPECT_EQ(3, line_count);
}
