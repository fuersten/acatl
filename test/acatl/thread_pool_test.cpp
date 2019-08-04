//
//  thread_pool_test.cpp
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

#include <chrono>
#include <iostream>
#include <list>
#include <sstream>

#include <acatl/thread_pool.h>

TEST(ThreadpoolTest, ConfigTest)
{
  acatl::Threadpool tp;
  EXPECT_EQ(std::thread::hardware_concurrency(), tp.numThreads());
}

TEST(ThreadpoolTest, SimpleTest)
{
  std::atomic_int counter(0);

  acatl::Threadpool tp;

  std::list<std::future<bool>> futures;

  for(int n = 0; n < 100; ++n) {
    futures.push_back(tp.enqueue([&counter] {
      ++counter;

      long r = random() % 50;
      std::this_thread::sleep_for(std::chrono::milliseconds(r));

      return true;
    }));
  }

  int n = 0;
  for(auto& fut : futures) {
    fut.get();
    ++n;
  }

  EXPECT_EQ(100, counter);
}

TEST(ThreadpoolTest, NestedTest)
{
  acatl::Threadpool tp;

  std::atomic_bool b(false);
  std::mutex mutex;
  std::condition_variable cv;

  tp.enqueue([&tp, &b, &cv, &mutex] {
    tp.enqueue([&b, &cv, &mutex] {
      {
        std::unique_lock<std::mutex> lock(mutex);
        b = true;
      }

      cv.notify_all();
    });
  });

  std::unique_lock<std::mutex> lock(mutex);
  cv.wait(lock);

  EXPECT_EQ(true, b.load());
}
