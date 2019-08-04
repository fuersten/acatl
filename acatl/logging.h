//
//  logging.h
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

#ifndef acatl_logging_h
#define acatl_logging_h

#include <acatl/inc.h>

#include <acatl/json.h>
#include <acatl/timestamp.h>
#include <acatl/types.h>

#include <map>
#include <mutex>
#include <thread>

namespace acatl
{
  /// Specifies a event to log.
  struct ACATL_EXPORT LogEvent
  {
    TimePoint _time;          //!< time point of log event
    std::string _classname;   //!< class that triggered the event
    std::string _category;    //!< category of the event
    long _line;               //!< the line in the file where the event was triggered
    std::string _file;        //!< the file where the event was triggered
    std::string _message;     //!< the message of the event
    std::thread::id _tid;     //!< the thread id of the triggering thread
  };

  /// This class provides the acatl logging mechanism. Only the static Logging::init and Logging::close methods should
  /// be called once from user code. Log messages can be written to stderr or syslog.
  class ACATL_EXPORT Logging : NonCopyable
  {
  public:
    /// Inizializes the log levels. Should only be called once in an application. Multiple calls have no effect
    /// beyond the first initialization. The JSON format is:
    ///
    /// ~~~~~~~~~~~~~~~~~~~~~
    ///    {
    ///      "log" : {
    ///        "level" : [
    ///          { "test::TestCheck"  : 1 },
    ///          { "test::TestCheck1" : 2 }
    ///        ],
    ///        "separator" : "@",
    ///        "syslog" : false,
    ///        "facility" : "domain"
    ///      }
    ///    }
    /// ~~~~~~~~~~~~~~~~~~~~~
    /// The default log level for classes is `0` and the default separator is `|`, logging to syslog is `false` by
    /// default. The default syslog facility is `acatl`.
    /// @param config The JSON config to get log levels from
    /// @param force Will initialize the logging facility even if it was previously initialized. Use with care, as
    ///              race conditions can occur if called in an multithreaded environment.
    static void init(const json& config, bool force = false);

    /// Calles the specific device event log method using the GlobalConfiguration.
    /// @param event The event that shall be logged
    static void log(const LogEvent& event);

    /// Returns the level for the given classname.
    /// @param classname The classname to find the configured level for
    /// @return The level configured for the given classname
    static int32_t level(const std::string& classname);

  private:
    /// As the Logging class is a singleton, the constructor is private and deleted.
    Logging() = delete;
  };

  /// Logs events with the categorie INFO.
  /// @param arg An output stream object
#define ACATL_INFOLOG(arg)                                                                                             \
  do {                                                                                                                 \
    acatl::LogEvent event;                                                                                             \
    event._time = acatl::now();                                                                                        \
    event._category = "INFO";                                                                                          \
    event._tid = std::this_thread::get_id();                                                                           \
    std::ostringstream ss;                                                                                             \
    ss << arg;                                                                                                         \
    event._message = ss.str();                                                                                         \
    acatl::Logging::log(event);                                                                                        \
  } while(0)

  /// Logs events with the categorie DEBUG. Debug log events can be switched on and off via the level attribute.
  /// Depending on the level, the event will be logged or not. Higher levels stand for more detailed information.
  /// @param unitname The name of the unit, normally the class
  /// @param lvl The level
  /// @param arg An output stream object
#define ACATL_CLASSLOG(unitname, lvl, arg)                                                                             \
  do {                                                                                                                 \
    static std::once_flag flag;                                                                                        \
    static bool XX_test__(false);                                                                                      \
    std::call_once(flag, [&]() {                                                                                       \
      int l = acatl::Logging::level(acatl::getTypename<unitname>());                                                   \
      XX_test__ = l >= lvl;                                                                                            \
    });                                                                                                                \
    if(XX_test__) {                                                                                                    \
      static std::string XX_stripped__ = acatl::getTypename<unitname>();                                               \
      acatl::LogEvent XX_event__;                                                                                      \
      XX_event__._time = acatl::now();                                                                                 \
      XX_event__._classname = XX_stripped__;                                                                           \
      XX_event__._category = "DEBUG";                                                                                  \
      XX_event__._tid = std::this_thread::get_id();                                                                    \
      XX_event__._line = __LINE__;                                                                                     \
      XX_event__._file = __FILE__;                                                                                     \
      std::ostringstream XX_ss__;                                                                                      \
      XX_ss__ << arg;                                                                                                  \
      XX_event__._message = XX_ss__.str();                                                                             \
      acatl::Logging::log(XX_event__);                                                                                 \
    }                                                                                                                  \
  } while(0)

  /// Logs events with the categorie DEBUG. Debug log events can be switched on and off via the level attribute.
  /// Depending on the level, the event will be logged or not. Higher levels stand for more detailed information.
  /// @param unitname The name of the unit
  /// @param lvl The level
  /// @param arg An output stream object
#define ACATL_DEBUGLOG(unitname, lvl, arg)                                                                             \
  do {                                                                                                                 \
    static std::once_flag flag;                                                                                        \
    static bool XX_test__(false);                                                                                      \
    std::call_once(flag, [&]() {                                                                                       \
      int l = acatl::Logging::level(#unitname);                                                                        \
      XX_test__ = l >= lvl;                                                                                            \
    });                                                                                                                \
    if(XX_test__) {                                                                                                    \
      acatl::LogEvent XX_event__;                                                                                      \
      XX_event__._time = acatl::now();                                                                                 \
      XX_event__._classname = #unitname;                                                                               \
      XX_event__._category = "DEBUG";                                                                                  \
      XX_event__._tid = std::this_thread::get_id();                                                                    \
      XX_event__._line = __LINE__;                                                                                     \
      XX_event__._file = __FILE__;                                                                                     \
      std::ostringstream XX_ss__;                                                                                      \
      XX_ss__ << arg;                                                                                                  \
      XX_event__._message = XX_ss__.str();                                                                             \
      acatl::Logging::log(XX_event__);                                                                                 \
    }                                                                                                                  \
  } while(0)

  /// Logs events with the categorie ERROR.
  /// @param arg An output stream object
#define ACATL_ERRORLOG(arg)                                                                                            \
  do {                                                                                                                 \
    acatl::LogEvent event;                                                                                             \
    event._time = acatl::now();                                                                                        \
    event._category = "ERROR";                                                                                         \
    event._tid = std::this_thread::get_id();                                                                           \
    event._line = __LINE__;                                                                                            \
    event._file = __FILE__;                                                                                            \
    std::ostringstream ss;                                                                                             \
    ss << arg;                                                                                                         \
    event._message = ss.str();                                                                                         \
    acatl::Logging::log(event);                                                                                        \
  } while(0)

  /// Logs events with the categorie ERROR.
  /// @param arg An output stream object
  /// @param exception An acatl::Exception object
#define ACATL_EXCEPTIONLOG(arg, exception)                                                                             \
  do {                                                                                                                 \
    acatl::LogEvent event;                                                                                             \
    event._time = acatl::now();                                                                                        \
    event._category = "ERROR";                                                                                         \
    event._tid = std::this_thread::get_id();                                                                           \
    event._line = exception.line();                                                                                    \
    event._file = exception.file();                                                                                    \
    std::ostringstream ss;                                                                                             \
    ss << arg << "[" << exception.code().value() << "] " << exception.what();                                          \
    event._message = ss.str();                                                                                         \
    acatl::Logging::log(event);                                                                                        \
  } while(0)
}

#endif
