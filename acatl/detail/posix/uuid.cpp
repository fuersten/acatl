//
//  uuid.cpp
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

#include <acatl/exception.h>
#include <acatl/uuid.h>

#include <uuid/uuid.h>

namespace acatl
{
  struct Uuid::Private
  {
    Private()
    {
      uuid_clear(_uuid);
    }

    uuid_t _uuid;
  };

  Uuid::Uuid()
  : _p(new Private)
  {
  }

  Uuid::Uuid(const std::string& uuid)
  : _p(new Private)
  {
    if(uuid.length() != 36) {
      ACATL_THROW(Exception, "not a uuid '" << uuid << "'");
    }
    if(uuid_parse(uuid.c_str(), _p->_uuid) != 0) {
      ACATL_THROW(Exception, "not a uuid '" << uuid << "'");
    }
  }

  Uuid::Uuid(const Uuid& rhs)
  : _p(new Private)
  {
    uuid_copy(_p->_uuid, rhs._p->_uuid);
  }

  Uuid& Uuid::operator=(const Uuid& rhs)
  {
    uuid_copy(_p->_uuid, rhs._p->_uuid);
    return *this;
  }

  Uuid::~Uuid()
  {
  }

  bool Uuid::operator==(const Uuid& rhs) const
  {
    return uuid_compare(_p->_uuid, rhs._p->_uuid) == 0;
  }

  bool Uuid::operator!=(const Uuid& rhs) const
  {
    int res = uuid_compare(_p->_uuid, rhs._p->_uuid);
    return res != 0;
  }

  bool Uuid::isNull() const
  {
    return uuid_is_null(_p->_uuid) == 1;
  }

  std::string Uuid::toString() const
  {
    char buf[37];
    uuid_unparse_lower(_p->_uuid, buf);
    return std::string(buf);
  }

  Uuid UuidGenerator::generate()
  {
    uuid_t uuid;
    uuid_generate(uuid);

    Uuid uid;
    uuid_copy(uid._p->_uuid, uuid);
    return uid;
  }
}
