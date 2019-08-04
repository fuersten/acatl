//
//  tribool_test.cpp
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

#include "acatl/tribool.h"

#include <vector>

static bool operatorBool(const acatl::Tribool& tb)
{
  if(tb) {
    return true;
  }

  return false;
}

TEST(TriboolTest, construction)
{
  {
    acatl::Tribool tb;
    EXPECT_EQ(false, operatorBool(tb));
    EXPECT_EQ(true, tb.isIndeterminate());
    EXPECT_EQ(false, tb.isTrue());
    EXPECT_EQ(false, tb.isFalse());
  }
  {
    acatl::Tribool tb(false);
    EXPECT_EQ(false, operatorBool(tb));
    EXPECT_EQ(false, tb.isIndeterminate());
    EXPECT_EQ(false, tb.isTrue());
    EXPECT_EQ(true, tb.isFalse());
  }
  {
    acatl::Tribool tb(true);
    EXPECT_EQ(true, operatorBool(tb));
    EXPECT_EQ(false, tb.isIndeterminate());
    EXPECT_EQ(true, tb.isTrue());
    EXPECT_EQ(false, tb.isFalse());
  }
}

TEST(TriboolTest, triboolTest)
{
  acatl::Tribool tb;
  EXPECT_EQ(false, !tb);
  EXPECT_EQ(false, operatorBool(tb));
  EXPECT_EQ(true, tb.isIndeterminate());
  EXPECT_EQ(false, tb.isTrue());
  EXPECT_EQ(false, tb.isFalse());

  tb.set(true);
  EXPECT_EQ(false, !tb);
  EXPECT_EQ(true, operatorBool(tb));
  EXPECT_EQ(false, tb.isIndeterminate());
  EXPECT_EQ(true, tb.isTrue());
  EXPECT_EQ(false, tb.isFalse());

  tb.set(false);
  EXPECT_EQ(true, !tb);
  EXPECT_EQ(false, operatorBool(tb));
  EXPECT_EQ(false, tb.isIndeterminate());
  EXPECT_EQ(false, tb.isTrue());
  EXPECT_EQ(true, tb.isFalse());
}

TEST(TriboolTest, equalityTest)
{
  acatl::Tribool tb1;
  acatl::Tribool tb2;

  EXPECT_EQ(false, tb1 == tb2);

  tb1.set(true);
  EXPECT_EQ(false, tb1 == tb2);
  tb2.set(false);
  EXPECT_EQ(false, tb1 == tb2);
  tb2.set(true);
  EXPECT_EQ(true, tb1 == tb2);
  tb1.set(false);
  tb2.set(false);
  EXPECT_EQ(true, tb1 == tb2);
}
