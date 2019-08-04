//
//  exception.h
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

#ifndef acatl_exception_h
#define acatl_exception_h

#include <acatl/inc.h>

#include <sstream>
#include <string>
#include <system_error>

namespace acatl
{
  /// Base class for all exceptions.
  ACATL_EXPORT class Exception : public std::system_error
  {
  public:
    /// Constructor.
    /// @param ec The error code to set
    /// @param message The message that shall be tied to this execption.
    /// @param file The file where the exception was thrown
    /// @param line The line in the file the exception was thrown
    explicit Exception(std::error_code ec, const std::string& message, const std::string& file, uint32_t line) noexcept;

    /// Constructor.
    /// @param ev The error value to set
    /// @param message The message that shall be tied to this execption.
    /// @param file The file where the exception was thrown
    /// @param line The line in the file the exception was thrown
    explicit Exception(int ev, const std::string& message, const std::string& file, uint32_t line) noexcept;

    /// Constructor.
    /// @param ec The error condition to set
    /// @param message The message that shall be tied to this execption.
    /// @param file The file where the exception was thrown
    /// @param line The line in the file the exception was thrown
    explicit Exception(std::errc ec, const std::string& message, const std::string& file, uint32_t line) noexcept;

    /// Constructor.
    /// @param message The message that shall be tied to this execption.
    /// @param file The file where the exception was thrown
    /// @param line The line in the file the exception was thrown
    explicit Exception(const std::string& message, const std::string& file, uint32_t line) noexcept;

    Exception(const Exception& ex) noexcept;

    /// Returns the file where the exception was thrown
    /// @return The file where the exception was thrown
    const std::string& file() const noexcept
    {
      return _file;
    }

    /// Returns the line where the exception was thrown
    /// @return The line where the exception was thrown
    uint32_t line() const noexcept
    {
      return _line;
    }

  private:
    std::string _file;
    uint32_t _line;
  };

  // Macro for exception class definitions with a specific base class.
#define ACATL_DECLARE_EXCEPTION(ex, base)                                                                              \
  class ex : public base                                                                                               \
  {                                                                                                                    \
  public:                                                                                                              \
    explicit ex(std::error_code ec, const std::string& message, const std::string& file, uint32_t line) noexcept       \
    : base(ec, message, file, line)                                                                                    \
    {                                                                                                                  \
    }                                                                                                                  \
    explicit ex(int ev, const std::string& message, const std::string& file, uint32_t line) noexcept                   \
    : base(ev, message, file, line)                                                                                    \
    {                                                                                                                  \
    }                                                                                                                  \
    explicit ex(std::errc ec, const std::string& message, const std::string& file, uint32_t line) noexcept             \
    : base(ec, message, file, line)                                                                                    \
    {                                                                                                                  \
    }                                                                                                                  \
    explicit ex(const std::string& message, const std::string& file, uint32_t line) noexcept                           \
    : base(message, file, line)                                                                                        \
    {                                                                                                                  \
    }                                                                                                                  \
    ex(const base& ex) noexcept                                                                                        \
    : base(ex)                                                                                                         \
    {                                                                                                                  \
    }                                                                                                                  \
  };

  /// Returns the text representation of the current errno value.
  /// @return The text representation of the current errno value.
  ACATL_EXPORT std::string errnoText();

  /// Throws an Exception based on the current errno value.
  /// @param error_domain This string is prependet to the errno strerror_r error message.
  /// @param file The file where the exception was thrown
  /// @param line The line in the file the exception was thrown
  ACATL_EXPORT void throwSysError(const std::string& error_domain, const std::string& file, uint32_t line);

  /// Internally rethrows an exception, catches it and outputs a message. It catches in the order:
  /// 1. Exception
  /// 2. std::exception
  /// 3. ...
  ACATL_EXPORT void evaluateException();

  /// As evaluateException(), but rethrows the exception after outputting a message.
  ACATL_EXPORT void evaluateExceptionAndThrow();

  ACATL_DECLARE_EXCEPTION(BadcastException, acatl::Exception);
  ACATL_DECLARE_EXCEPTION(OutOfRangeException, acatl::Exception);
  ACATL_DECLARE_EXCEPTION(InvalidParameterException, acatl::Exception);

  // Macro for exception throwing.
#define ACATL_THROW(ex, arg)                                                                                           \
  do {                                                                                                                 \
    std::ostringstream XX_ss__;                                                                                        \
    XX_ss__ << arg;                                                                                                    \
    throw ex(XX_ss__.str(), __FILE__, __LINE__);                                                                       \
  } while(0);
}

#endif
