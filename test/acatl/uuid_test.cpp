//
//  uuid_test.cpp
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

#include <acatl/uuid.h>

TEST(UuidTest, generateTest)
{
  acatl::Uuid uuid = acatl::UuidGenerator::generate();
  EXPECT_FALSE(uuid.isNull());
}

TEST(UuidTest, nullTest)
{
  acatl::Uuid uuid;
  EXPECT_TRUE(uuid.isNull());

  EXPECT_EQ("00000000-0000-0000-0000-000000000000", uuid.toString());
}

TEST(UuidTest, ToStringAndBackTest)
{
  acatl::Uuid uuid = acatl::UuidGenerator::generate();
  std::string uid = uuid.toString();
  EXPECT_EQ(36U, uid.length());

  acatl::Uuid uuid2(uid);
  EXPECT_EQ(36U, uuid2.toString().length());
  EXPECT_EQ(uuid, uuid2);
}

TEST(UuidTest, UniqueTest)
{
  acatl::Uuid uuid = acatl::UuidGenerator::generate();
  acatl::Uuid uuid2 = acatl::UuidGenerator::generate();
  EXPECT_NE(uuid, uuid2);
}

TEST(UuidTest, CopyTest)
{
  acatl::Uuid uuid = acatl::UuidGenerator::generate();
  acatl::Uuid uuid2(uuid);
  EXPECT_EQ(uuid, uuid2);
}

TEST(UuidTest, AssignmentTest)
{
  acatl::Uuid uuid = acatl::UuidGenerator::generate();
  acatl::Uuid uuid2 = uuid;
  EXPECT_EQ(uuid, uuid2);

  uuid = acatl::UuidGenerator::generate();
  uuid2 = acatl::Uuid(uuid.toString());
  EXPECT_EQ(uuid, uuid2);
}
