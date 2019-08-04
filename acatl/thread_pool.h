//
//  threadpool.h
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

#ifndef acatl_threadpool_hpp
#define acatl_threadpool_hpp

#include <acatl/inc.h>

#include <acatl/exception.h>
#include <acatl/types.h>

#include <algorithm>
#include <atomic>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace acatl
{
  ACATL_EXPORT ACATL_DECLARE_EXCEPTION(ThreadpoolException, Exception);

  /// A thread pool for the asynchronous execution of tasks.
  ACATL_EXPORT class Threadpool : public NonCopyable
  {
  public:
    /// Creates a thread pool.
    /// @param numThreads Number of threads to spawn in the pool. Default is to use
    /// std::thread::hardware_concurrency()
    ///                   number of threads.
    Threadpool(size_t numThreads = 0)
    : _numThreads(numThreads)
    , _stop(false)
    {
      if(_numThreads == 0) {
        _numThreads = std::thread::hardware_concurrency();
      }

      for(size_t n = 0; n < _numThreads; ++n) {
        _worker.emplace_back([this] {
          while(true) {
            Task task;
            {
              std::unique_lock<std::mutex> lock{_queueMutex};

              _queueCondition.wait(lock, [this] { return _stop.load() || !_tasks.empty(); });

              if(_stop.load() && _tasks.empty()) {
                return;
              }
              if(!_tasks.empty()) {
                task = std::move(_tasks.front());
                _tasks.pop();
              }
            }

            if(task) {
              task();
            }
          }
        });
      }
    }

    /// Destory the pool. Will wait until all threads have terminated. A thread terminates, if its task returns.
    ~Threadpool()
    {
      _stop.store(true);
      _queueCondition.notify_all();

      std::for_each(_worker.begin(), _worker.end(), [](std::thread& t) {
        if(t.joinable()) {
          t.join();
        }
      });
    }

    /// Return the number of threads in the pool.
    /// @return Number of threads in the pool.
    size_t numThreads() const
    {
      return _numThreads;
    }

    /// Add a task to the pool. The task will be queued and eventually executed as soon as at least one thread is
    /// free.
    /// @tparam R The type of the task. Will normally be deduced automatically.
    /// @tparam Args The types of the arguments of the task. Will normally be deduced automatically.
    /// @param r The task to execute. The easiest way to add a task is to use a lambda, but you can also use a
    /// function
    ///          or std::bind or whatever you like.
    /// @param args The arguments for the task to execute can be left empty.
    /// @return A future for the tasks result. Can be used to wait explicitely for the task to finish.
    template<typename R, typename... Args>
    std::future<typename std::result_of<R(Args...)>::type> enqueue(R&& r, Args&&... args)
    {
      auto task = std::make_shared<std::packaged_task<typename std::result_of<R(Args...)>::type()>>(
        std::bind(std::forward<R>(r), std::forward<Args>(args)...));
      {
        std::unique_lock<std::mutex> lock{_queueMutex};
        if(_stop.load()) {
          ACATL_THROW(ThreadpoolException, "Threadpool was already stopped");
        }
        _tasks.emplace([task]() mutable { (*task)(); });
      }
      _queueCondition.notify_one();
      return task->get_future();
    }

  private:
    typedef std::function<void()> Task;
    size_t _numThreads;
    std::vector<std::thread> _worker;
    std::queue<Task> _tasks;
    std::mutex _queueMutex;
    std::condition_variable _queueCondition;
    std::atomic_bool _stop;
  };
}

#endif
