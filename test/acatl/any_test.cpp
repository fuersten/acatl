//
//  any_test.cpp
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

#include <acatl/any.h>

TEST(AnyTest, constructionTest)
{
  acatl::Any any_float(47.11f);
  acatl::Any any_string(std::string("test"));
  acatl::Any any_integer(384765);
  acatl::Any any_char(static_cast<const char*>("test"));
  acatl::Any any_empty;
  std::vector<std::string> strings;
  acatl::Any any_vector(strings);

  EXPECT_EQ(true, any_empty.empty());
  EXPECT_EQ(false, any_float.empty());
}

TEST(AnyTest, copyTest)
{
  acatl::Any any_integer(384765);
  acatl::Any any_second(any_integer);
  EXPECT_EQ(false, any_second.empty());
  EXPECT_EQ(384765, acatl::any_cast<int32_t>(any_second));
}

TEST(AnyTest, assignmentTest)
{
  acatl::Any any_integer(384765);
  acatl::Any any_second = any_integer;
  EXPECT_EQ(false, any_second.empty());
  EXPECT_EQ(384765, acatl::any_cast<int32_t>(any_second));

  acatl::Any any_third = std::string("Test");
  EXPECT_EQ(false, any_third.empty());
  EXPECT_EQ("Test", acatl::any_cast<std::string>(any_third));

  any_third = 47.11;
  EXPECT_EQ(false, any_third.empty());
  ASSERT_DOUBLE_EQ(47.11, acatl::any_cast<double>(any_third));
}

TEST(AnyTest, anyMove)
{
  acatl::Any any_integer(384765);
  acatl::Any any_move(std::move(any_integer));

  EXPECT_EQ(384765, acatl::any_cast<int32_t>(any_move));
  EXPECT_TRUE(any_integer.empty());

  acatl::Any any_move_assign = std::move(any_move);
  EXPECT_TRUE(any_move.empty());
  EXPECT_EQ(384765, acatl::any_cast<int32_t>(any_move_assign));
}

TEST(AnyTest, castTest)
{
  acatl::Any any_double(47.11);
  ASSERT_DOUBLE_EQ(47.11, acatl::any_cast<double>(any_double));
  EXPECT_THROW(acatl::any_cast<int32_t>(any_double), acatl::BadcastException);

  ASSERT_DOUBLE_EQ(47.11, *acatl::any_cast<double>(&any_double));
  EXPECT_FALSE(acatl::any_cast<int32_t>(&any_double));
}

TEST(AnyTest, constRefCastTest)
{
  acatl::Any any_integer(384765);
  EXPECT_EQ(384765, acatl::any_cast<const int&>(any_integer));
}
