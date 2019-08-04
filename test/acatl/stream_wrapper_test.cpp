//
//  strea_wrapper_test.cpp
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

#include <acatl/stream_wrapper.h>

#include <fstream>

TEST(StreamWrapperTest, WriteStringStreamTest)
{
  std::stringstream ss;

  FILE* fp = acatl::open_ostream(ss);
  fprintf(fp, "This is my %s day!", "coolest");
  fclose(fp);

  EXPECT_EQ("This is my coolest day!", ss.str());
}

TEST(StreamWrapperTest, StdOutStreamTest)
{
  acatl::filesystem::TemporaryDirectoryGuard guard;
  fs::path path = guard.temporaryDirectoryPath();
  acatl::filesystem::current_path(path);
  RedirectStdOut rs;

  FILE* fp = acatl::open_ostream(std::cout);
  fprintf(fp, "This is my %s day!\n", "coolest");
  fclose(fp);

  std::ifstream log(path / "stdout.txt");
  ASSERT_EQ(true, log.good());
  std::string line;
  size_t count(0);
  while(std::getline(log, line).good()) {
    EXPECT_EQ("This is my coolest day!", line);
    ++count;
  }
  EXPECT_EQ(1u, count);
}
