//
//  logging.cpp
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

#include "logging.h"
#include "string_helper.h"

#include <syslog.h>

namespace acatl
{
  static char _separator{'|'};
  static std::map<std::string, int32_t> _classLevel;
  static std::once_flag _initFlag;
  static bool _hasSyslog{false};

  static void doInitLogging(const json& config)
  {
    auto log = config.find("log");
    if(log != config.end()) {
      try {
        auto level = log->at("level");
        for(const auto& elem : level) {
          for(auto it = elem.begin(); it != elem.end(); ++it) {
            _classLevel.emplace(it.key(), it.value().get<long>());
          }
        }
      } catch(const std::exception&) {
        // just do nothing, if log level not specified
      }

      _separator = log->value("separator", "|")[0];
      if(log->value("syslog", false)) {
        std::string facility = log->value("facility", "acatl");
        ::openlog(facility.c_str(), LOG_CONS | LOG_PID | LOG_NDELAY, 0);
        _hasSyslog = true;
      }
    }
  }

  void Logging::init(const json& json, bool force)
  {
    if(force) {
      doInitLogging(json);
      // just set the flag, if it hasn't been set yet
      std::call_once(_initFlag, []() {});
    } else {
      std::call_once(_initFlag, doInitLogging, json);
    }
  }

  int32_t Logging::level(const std::string& classname)
  {
    const auto& entry = _classLevel.find(classname);
    return entry != _classLevel.end() ? entry->second : 0;
  }

  void Logging::log(const LogEvent& event)
  {
    static std::mutex s_mutex;

    std::ostringstream os;
    os << event._time << _separator;
    os << event._category << _separator;
    os << event._tid << _separator;
    if(!event._classname.empty()) {
      os << event._classname << _separator;
    }
    os << event._message;
    if(!event._file.empty()) {
      os << _separator << event._file << ":" << event._line;
    }
    os << "\n";

    if(_hasSyslog) {
      // todo lcf: the category really should be an enum
      if(event._category == "INFO") {
        ::syslog(LOG_NOTICE, "%s", os.str().c_str());
      } else if(event._category == "DEBUG") {
        ::syslog(LOG_NOTICE, "%s", os.str().c_str());
      } else if(event._category == "ERROR") {
        ::syslog(LOG_ERR, "%s", os.str().c_str());
      } else {
        ::syslog(LOG_NOTICE, "%s", os.str().c_str());
      }
    } else {
      {
        std::unique_lock<std::mutex> guard{s_mutex};
        std::cerr << os.str();
      }
      std::cerr.flush();
    }
  }
}
