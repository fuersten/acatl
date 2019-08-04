//
//  io_context_pool.h
//  acatl
//
//  BSD 3-Clause License
//  Copyright (c) 2018, Lars-Christian Fürstenberg
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

#ifndef acatl_io_context_pool_h
#define acatl_io_context_pool_h

#include <acatl/types.h>

#include <atomic>
#include <thread>
#include <vector>

#include <asio/io_context.hpp>

namespace acatl
{
  namespace net
  {
    /// Manages a bunch of asio::io_context instances. Each call to get a new
    /// context will deliver one of the previously created in a round-robin fashion.
    /// This takes up some ideas from Konrad Zemek concerning the scalability of
    /// io_context and cores/threads. See here for more information:
    /// https://konradzemek.com/2015/08/16/asio-ssl-and-scalability/
    class IoContextPool final : public acatl::NonCopyable
    {
    public:
      /// Initializes the given number of asio::io_context instances. A good
      /// value for number is the number of cores the executing machine has.
      /// @param number Number of contexts / threads to create.
      IoContextPool(uint32_t number)
      : _ioContextPool(number)
      {
        for(auto& context : _ioContextPool) {
          _idleWorker.emplace_back(context);
        }
      }

      /// Starts the execution on the previously created asio::io_context
      /// instances and blocks until all have been finished.
      void run()
      {
        for(auto& context : _ioContextPool) {
          _threadPool.emplace_back([&] {
            try {
              context.run();
            } catch(const std::exception&) {
              // just run until io_context.run exits
            }
          });
        }

        for(auto& thread : _threadPool) {
          if(thread.joinable()) {
            thread.join();
          }
        }
      }

      /// Stops the asio::io_context instances. Calling stop() should terminate
      /// all running contexts and will finally unblock the run() method.
      void stop()
      {
        for(auto& context : _ioContextPool) {
          context.stop();
        }
      }

      /// Destructor calls implicitely stop().
      ~IoContextPool()
      {
        stop();
      }

      /// Retrieves a context from the previously created ones in a round-robin
      /// fashion.
      /// @return Returns an asio::io_context instance from the context pool.
      asio::io_context& get()
      {
        return _ioContextPool[(_nextContext++ % _ioContextPool.size())];
      }

    private:
      std::atomic<std::size_t> _nextContext{0};
      std::vector<asio::io_context> _ioContextPool;
      std::vector<asio::io_context::work> _idleWorker;
      std::vector<std::thread> _threadPool;
    };
  }
}

#endif
