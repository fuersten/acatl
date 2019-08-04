//
//  type_test.cpp
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
#include <acatl/types.h>

namespace acatl
{
  namespace testspace
  {
    class Test
    {
    };
  }
}

TEST(TypesTest, StriptypenameTest)
{
  EXPECT_EQ("acatl::testspace::Test", acatl::stripTypeName(typeid(acatl::testspace::Test).name()));
  EXPECT_EQ("acatl::Exception", acatl::stripTypeName(typeid(acatl::Exception).name()));
  EXPECT_EQ("long", acatl::stripTypeName(typeid(long).name()));
}

TEST(TypesTest, GetTypenameTest)
{
  EXPECT_EQ("long", acatl::getTypename<long>());
  EXPECT_EQ("string", acatl::getTypename<std::string>());
}

TEST(TypesTest, ByteOrderTest)
{
  // TODO lcf: we need a compile/runtime for the endianess in oder to check correctly
  EXPECT_EQ(acatl::ByteOrder::eLITTLE_ENDIAN, acatl::checkByteOrder());
}
