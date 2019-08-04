//
//  types.h
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

#ifndef acatl_types_h
#define acatl_types_h

#include <acatl/inc.h>

#include <acatl/exception.h>

#include <chrono>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <sys/param.h>
#endif

#if defined(__APPLE__) && defined(__MACH__)
#define ACATL_OS_MACOSX
#elif defined(__linux__)
#define ACATL_OS_LINUX
#elif defined(BSD)
#define ACATL_OS_BSD
#elif defined(_WIN64)
#define ACATL_OS_WINDOWS
#define ACATL_OS_WIN64
#elif defined(_WIN32)
#define ACATL_OS_WINDOWS
#define ACATL_OS_WIN32
#else
#error "Operating system currently not supported!"
#endif

namespace acatl
{
  /// Timestamp
  typedef std::chrono::system_clock::time_point Timepoint;

  typedef std::vector<std::string> StringVector;

  /// Templating helper to enforce a specific type for an argument.
  template<typename T>
  struct Typer
  {
    typedef T type;
  };

  // Templating helper to convert an integer into a type.
  template <int v>
  struct int2type {
    enum { value = v };
  };

  // Returns the underlying type of the given enum
  // @param t The enum to find the underlying type of
  // @return The underlying type of the enum
  template<typename T>
  constexpr auto underlyingType(T t)
  {
    return static_cast<typename std::underlying_type<T>::type>(t);
  }

  /// A base class to make derived classes non-copyable by deleting the copy constructor and move constructor and
  /// corresponding assignment operator.
  struct NonCopyable
  {
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&) = delete;
    NonCopyable& operator=(NonCopyable&&) = delete;
  };

  /// Remove compiler name mangling artifacts from a given class name.
  /// @note The implemetation is based on code from the framework MPF (http://libmpf.sf.net)
  /// @param classname The classname, typically from a typeid(MyClass).name()-like call.
  /// @return The pure class name with namespaces.
  std::string stripTypeName(const std::string& classname);

  /// Returns the name of a type. The default is to get the RTTI name.
  /// @return The RTTI name of the type.
  template<typename T>
  inline std::string getTypename()
  {
    return stripTypeName(typeid(T).name());
  }

  /// Returns the name of the std::string.
  /// @return "string".
  template<>
  inline std::string getTypename<std::string>()
  {
    return "string";
  }

  /// Byte orders
  enum class ByteOrder
  {
    eLITTLE_ENDIAN,
    eBIG_ENDIAN,
    eHOST,
    eNETWORK
  };

  /// Function to determine the host machines byte order
  /// @return The byte order of this machine
  ACATL_EXPORT ByteOrder checkByteOrder();
}

#endif
