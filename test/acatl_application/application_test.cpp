//
//  application_test.cpp
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

#include <acatl_application/application.h>

class MyApplication : public acatl::Application
{
public:
  MyApplication(int argc, char** argv, bool setUp)
  : acatl::Application(argc, argv)
  , _didSetup(false)
  , _didRun(false)
  , _didTearDown(false)
  , _setUp(setUp)
  {
  }

  bool _didSetup;
  bool _didRun;
  bool _didTearDown;

private:
  bool setUp(const acatl::StringVector& args) override
  {
    EXPECT_EQ(2U, args.size());

    _didSetup = true;
    return _setUp;
  }

  int doRun() override
  {
    _didRun = true;
    return 0;
  }

  void tearDown(int) override
  {
    _didTearDown = true;
  }

  bool _setUp;
};

TEST(ApplicationTest, run)
{
  int argc = 2;
  const char* args[] = {"MyApp", "Arg1"};

  MyApplication myapp(argc, const_cast<char**>(args), true);
  int ret = myapp.run();

  EXPECT_EQ(0, ret);
  EXPECT_TRUE(myapp._didSetup);
  EXPECT_TRUE(myapp._didRun);
  EXPECT_TRUE(myapp._didTearDown);
}

TEST(ApplicationTest, dontRun)
{
  int argc = 2;
  const char* args[] = {"MyApp", "Arg1"};

  MyApplication myapp(argc, const_cast<char**>(args), false);
  int ret = myapp.run();

  EXPECT_EQ(-1, ret);
  EXPECT_TRUE(myapp._didSetup);
  EXPECT_FALSE(myapp._didRun);
  EXPECT_FALSE(myapp._didTearDown);
}
