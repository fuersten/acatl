//
//  exception.cpp
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

#include "exception.h"
#include "logging.h"

#include <cstring>
#include <iostream>

#include <sys/errno.h>

namespace acatl
{
  Exception::Exception(std::error_code ec, const std::string& message, const std::string& file, uint32_t line) noexcept
  : std::system_error(ec, message)
  , _file(file)
  , _line(line)
  {
  }

  Exception::Exception(int ev, const std::string& message, const std::string& file, uint32_t line) noexcept
  : std::system_error(ev, std::generic_category(), message)
  , _file(file)
  , _line(line)
  {
  }

  Exception::Exception(std::errc ec, const std::string& message, const std::string& file, uint32_t line) noexcept
  : std::system_error(std::make_error_code(ec), message)
  , _file(file)
  , _line(line)
  {
  }

  Exception::Exception(const std::string& message, const std::string& file, uint32_t line) noexcept
  : std::system_error(0, std::generic_category(), message)
  , _file(file)
  , _line(line)
  {
  }

  Exception::Exception(const Exception& ex) noexcept
  : std::system_error(ex)
  , _file(ex._file)
  , _line(ex._line)
  {
  }

  std::string errnoText()
  {
    char buf[512];
#if defined __linux__
    char* s = strerror_r(errno, buf, sizeof(buf));
    return s ? s : buf;
#else
    strerror_r(errno, buf, sizeof(buf));
    return buf;
#endif
  }

  void throwSysError(const std::string& error_domain, const std::string& file, uint32_t line)
  {
    throw Exception{errno, error_domain, file, line};
  }

  void evaluateException()
  {
    try {
      throw;
    } catch(Exception& ex) {
      ACATL_EXCEPTIONLOG("exception caught: ", ex);
    } catch(std::system_error& ex) {
      ACATL_ERRORLOG("std::system error caught: [" << ex.code().value() << "] " << ex.what());
    } catch(std::exception& ex) {
      ACATL_ERRORLOG("std::exception caught: " << ex.what());
    } catch(...) {
      ACATL_ERRORLOG("unknown exception caught");
    }
  }

  void evaluateExceptionAndThrow()
  {
    evaluateException();
    throw;
  }
}
