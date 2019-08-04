//
//  numeric_cast.h
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

#ifndef acatl_numeric_cast_h
#define acatl_numeric_cast_h

#include <acatl/inc.h>

#include <acatl/exception.h>
#include <acatl/types.h>

#include <type_traits>
#include <typeinfo>

namespace acatl
{
  ACATL_DECLARE_EXCEPTION(BadNumericCast, acatl::Exception);

  /// Casts a numeric type into another, throwing exceptions, if the source type does not fit into the target type.
  /// This overload assures, that only numeric types can be cast and other types give a compiler error.
  template<typename Target,
           typename Source,
           typename std::enable_if<!((std::is_integral<Source>::value || std::is_floating_point<Source>::value)
                                     && (std::is_integral<Target>::value || std::is_floating_point<Target>::value)),
                                   int>::type = 0>
  Target numeric_cast(Source val)
  {
    static_assert(((std::is_integral<Source>::value || std::is_floating_point<Source>::value)
                   && (std::is_integral<Target>::value || std::is_floating_point<Target>::value)),
                  "Cannot cast non integral or non floating point values");
    return Target();
  }

  /// Casts a numeric type into another, throwing exceptions, if the source type does not fit into the target type.
  /// This overload just returns the source value if both types are the same.
  template<typename Target,
           typename Source,
           typename std::enable_if<std::is_same<Target, Source>::value
                                     && (std::is_integral<Source>::value || std::is_floating_point<Source>::value)
                                     && (std::is_integral<Target>::value || std::is_floating_point<Target>::value),
                                   int>::type = 0>
  Target numeric_cast(Source val)
  {
    return val;
  }

  /// Casts a numeric type into another, throwing exceptions, if the source type does not fit into the target type.
  /// This overload casts only integral types of the same signedness.
  template<typename Target,
           typename Source,
           typename std::enable_if<!std::is_same<Target, Source>::value && std::is_integral<Source>::value
                                     && std::is_integral<Target>::value
                                     && ((std::is_signed<Source>::value && std::is_signed<Target>::value)
                                         || (std::is_unsigned<Source>::value && std::is_unsigned<Target>::value)),
                                   int>::type = 0>
  Target numeric_cast(Source val)
  {
    if(std::numeric_limits<Target>::max() < std::numeric_limits<Source>::max()) {
      if(val > std::numeric_limits<Target>::max()) {
        // does not fit in => exception
        ACATL_THROW(BadNumericCast, "Value out of target range: " << val << " to " << getTypename<Target>());
      }
    }
    if(std::numeric_limits<Target>::min() > std::numeric_limits<Source>::min()) {
      if(val < std::numeric_limits<Target>::min()) {
        // does not fit in => exception
        ACATL_THROW(BadNumericCast, "Value out of target range: " << val << " to " << getTypename<Target>());
      }
    }
    return static_cast<Target>(val);
  }

  /// Casts a numeric type into another, throwing exceptions, if the source type does not fit into the target type.
  /// This overload casts only integral types with target unsigned and source signed
  template<typename Target,
           typename Source,
           typename std::enable_if<!std::is_same<Target, Source>::value && std::is_integral<Source>::value
                                     && std::is_integral<Target>::value
                                     && (std::is_signed<Source>::value && std::is_unsigned<Target>::value),
                                   int>::type = 0>
  Target numeric_cast(Source val)
  {
    typedef typename std::make_unsigned<Source>::type unsigned_source_type;

    if(std::numeric_limits<Target>::max() < std::numeric_limits<Source>::max()) {
      if(val > std::numeric_limits<Target>::max()) {
        // does not fit in => exception
        ACATL_THROW(BadNumericCast, "Value out of target range: " << val << " to " << getTypename<Target>());
      }
    }
    if(val < 0) {
      // does not fit in => exception
      ACATL_THROW(BadNumericCast, "Value out of target range: " << val << " to " << getTypename<Target>());
    }
    if(static_cast<unsigned_source_type>(val) < std::numeric_limits<Target>::min()) {
      // does not fit in => exception
      ACATL_THROW(BadNumericCast, "Value out of target range: " << val << " to " << getTypename<Target>());
    }
    return static_cast<Target>(val);
  }

  /// Casts a numeric type into another, throwing exceptions, if the source type does not fit into the target type.
  /// This overload casts only integral types with target signed and source unsigned
  template<typename Target,
           typename Source,
           typename std::enable_if<!std::is_same<Target, Source>::value && std::is_integral<Source>::value
                                     && std::is_integral<Target>::value
                                     && (std::is_unsigned<Source>::value && std::is_signed<Target>::value),
                                   int>::type = 0>
  Target numeric_cast(Source val)
  {
    if(std::numeric_limits<Target>::max() < std::numeric_limits<Source>::max()) {
      if(val > std::numeric_limits<Target>::max()) {
        // does not fit in => exception
        ACATL_THROW(BadNumericCast, "Value out of target range: " << val << " to " << getTypename<Target>());
      }
    }
    return static_cast<Target>(val);
  }

  /// Casts a numeric type into another, throwing exceptions, if the source type does not fit into the target type.
  /// This overload casts mixed integral/floating point types or only floating point types.
  template<
    typename Target,
    typename Source,
    typename std::enable_if<!std::is_same<Target, Source>::value
                              && (std::is_floating_point<Source>::value || std::is_floating_point<Target>::value),
                            int>::type = 0>
  Target numeric_cast(Source val)
  {
    if(std::numeric_limits<Target>::max() < std::numeric_limits<Source>::max()) {
      if(val > static_cast<Source>(std::numeric_limits<Target>::max())) {
        // does not fit in => exception
        ACATL_THROW(BadNumericCast, "Value out of target range: " << val << " to " << getTypename<Target>());
      }
    }
    if(val < std::numeric_limits<Target>::lowest()) {
      // does not fit in => exception
      ACATL_THROW(BadNumericCast, "Value out of target range: " << val << " to " << getTypename<Target>());
    }
    return static_cast<Target>(val);
  }
}

#endif
