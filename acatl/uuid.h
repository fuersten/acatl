//
//  uuid.h
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

#ifndef acatl_uuid_h
#define acatl_uuid_h

#include <acatl/inc.h>

#include <acatl/types.h>

#include <memory>

namespace acatl
{
  /// This class represents an instance of a uuid (see https://www.ietf.org/rfc/rfc4122.txt).
  class ACATL_EXPORT Uuid
  {
  public:
    /// ctor
    /// Constructs an empty Uuid.
    Uuid();

    /// ctor
    /// Constructs a Uuid from a formated string. The string must have the form
    /// 1b4e28ba-2fa1-11d2-883f-0016d3cca427. Will throw an Exception, if the conversion does not work.
    /// @param uuid String to convert into the internal uuid format
    explicit Uuid(const std::string& uuid);

    /// ctor
    /// Constructs a Uuid from another one
    /// @param rhs The Uuid to create this from
    Uuid(const Uuid& rhs);

    /// Assignes the uuid value from the right side Uuid.
    /// @param rhs The Uuid to assign to this
    /// @return This Uuid with the value of the right side Uuid
    Uuid& operator=(const Uuid& rhs);

    /// dtor
    ~Uuid();

    /// Compares the right side Uuid to this Uuid for equality.
    /// @param rhs The Uuid to compare to
    /// @return true if the right side Uuid is equal to this one, false otherwise
    bool operator==(const Uuid& rhs) const;

    /// Compares the right side Uuid to this Uuid for inequality.
    /// @param rhs The Uuid to compare to
    /// @return true if the right side Uuid is not equal to this one, false otherwise
    bool operator!=(const Uuid& rhs) const;

    /// Returns whether the Uuid is valid or not.
    /// @return true if it is not valid, otherwise false
    bool isNull() const;

    /// Returns a human readable form of the Uuid.
    /// @return The human string readable representation of the Uuid
    std::string toString() const;

  private:
    friend class UuidGenerator;

    struct Private;
    std::unique_ptr<Private> _p;
  };

  /// A generator for Uuids.
  class ACATL_EXPORT UuidGenerator
  {
  public:
    /// Generates a valid Uuid. Each generated Uuid is unique.
    /// @return A valid Uuid
    static Uuid generate();

  private:
    UuidGenerator() = delete;
  };
}

#endif
