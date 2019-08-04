//
//  numeric_cast_test.cpp
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

#include <acatl/numeric_cast.h>

TEST(TypesTest, NumericCastTest)
{
  long l = -700;
  EXPECT_THROW(acatl::numeric_cast<uint16_t>(l), acatl::BadNumericCast);
  l = 70000;
  EXPECT_THROW(acatl::numeric_cast<uint16_t>(l), acatl::BadNumericCast);

  int i = 42;
  EXPECT_NO_THROW(acatl::numeric_cast<short>(i));
  EXPECT_EQ(42, acatl::numeric_cast<short>(i));

  EXPECT_THROW(acatl::numeric_cast<long>(std::numeric_limits<unsigned long>::max()), acatl::BadNumericCast);

  EXPECT_NO_THROW(acatl::numeric_cast<int>(42));

  uint32_t ui32 = 4378738;
  EXPECT_EQ(4378738, acatl::numeric_cast<int64_t>(ui32));

  ssize_t n = 459457947;
  EXPECT_EQ(459457947u, acatl::numeric_cast<size_t>(n));

  float f = -42.1234f;
  EXPECT_THROW(acatl::numeric_cast<uint32_t>(f), acatl::BadNumericCast);
  EXPECT_EQ(-42, acatl::numeric_cast<int32_t>(f));

  double d = -42.1234f + acatl::numeric_cast<double>(123);
  EXPECT_DOUBLE_EQ(-42.1234f + 123.0, d);

  EXPECT_THROW(acatl::numeric_cast<float>(std::numeric_limits<double>::max()), acatl::BadNumericCast);
  EXPECT_THROW(acatl::numeric_cast<float>(std::numeric_limits<double>::lowest()), acatl::BadNumericCast);

  EXPECT_NO_THROW(acatl::numeric_cast<double>(std::numeric_limits<float>::max()));
  EXPECT_NO_THROW(acatl::numeric_cast<double>(std::numeric_limits<float>::lowest()));

  EXPECT_THROW(acatl::numeric_cast<uint64_t>(std::numeric_limits<double>::max()), acatl::BadNumericCast);
  EXPECT_THROW(acatl::numeric_cast<int64_t>(std::numeric_limits<double>::max()), acatl::BadNumericCast);
  EXPECT_THROW(acatl::numeric_cast<int64_t>(std::numeric_limits<double>::lowest()), acatl::BadNumericCast);

  uint32_t ui = 473839;
  EXPECT_NO_THROW(d = acatl::numeric_cast<double>(ui));
  EXPECT_DOUBLE_EQ(473839.0, d);
}
