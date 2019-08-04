//
//  string_helper.h
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

#ifndef acatl_string_helper_h
#define acatl_string_helper_h

#include <acatl/inc.h>

#include <acatl/exception.h>
#include <acatl/filesystem.h>
#include <acatl/types.h>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <iterator>

namespace acatl
{
  /// Splits the given string into its elements using the delimiter.
  /// @param s The string to split
  /// @param delim The delimiter to use
  /// @param elems The collection to put the splitted elemts into. The collection will be cleared before inserting
  /// into it.
  /// @param addEmpty If empty elements shall be added. Default is true.
  /// @return The number of elements in elems.
  template<typename T>
  size_t split(const std::string& s, char delim, T& elems, bool addEmpty = true)
  {
    elems.clear();
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
      if(addEmpty || item.length()) {
        elems.push_back(item);
      }
    }
    return elems.size();
  }

  /// Joins the elements of container elems using the given separator. The separator will not be added to the last
  /// element. The code is based on: http://stackoverflow.com/a/5289170/7936507
  /// @param elems Container to elements to join
  /// @param separator String to separate elements with
  /// @return A string with all elements separated by the separator
  template<typename T>
  std::string join(const T& elems, const char* const separator)
  {
    switch(elems.size()) {
      case 0:
        return "";
      case 1: {
        std::ostringstream os;
        os << elems[0];
        return os.str();
      }
      default:
        std::ostringstream os;
        std::copy(elems.begin(), elems.end() - 1, std::ostream_iterator<typename T::value_type>(os, separator));
        os << *elems.rbegin();
        return os.str();
    }
  }

  /// Replace all occurences of 'from' to 'to' in the source string
  /// The code is based on: http://stackoverflow.com/a/29752943/7936507
  /// @param source The source string to make the changes inplace
  /// @param from The sub string to change
  /// @param to The sub string to change 'from' to
  /// @return The source string
  std::string& replaceAll(std::string& source, const std::string& from, const std::string& to);

  /// Stream overload for the output of std::chrono time points. Time points are outputted as iso time strings.
  /// @param stream The stream to output to
  /// @param time_point The time point to output
  /// @return The stream operator after the streaming of the time point.
  template<typename Clock, typename Duration>
  std::ostream& operator<<(std::ostream& stream, const std::chrono::time_point<Clock, Duration>& time_point)
  {
    const time_t time = Clock::to_time_t(time_point);
    struct tm ptime;
    localtime_r(&time, &ptime);
    return (stream << std::put_time(&ptime, "%Y-%m-%dT%H:%M:%S"));
  }

  /// Returns a string representation of the time point formatted as rfc1123 date.
  /// @param time_point The time point to format
  /// @return The formatted date representation.
  template<typename Clock, typename Duration>
  std::string formatDateRfc1123(const std::chrono::time_point<Clock, Duration>& time_point)
  {
    const time_t time = Clock::to_time_t(time_point);
    std::ostringstream ss;
    struct tm ptime;
    gmtime_r(&time, &ptime);
    ss << std::put_time(&ptime, "%a, %d %b %Y %H:%M:%S GMT");
    return ss.str();
  }

  /// Format a std::chrono::duration to a compact human readable string.
  /// The resulting string will have a dynamically chosen time unit of us, ms, s, min or h, so that the part before
  /// the decimal point is in the range of 0-100 and a precision of one is used. This might not be the best way to
  /// format time for end user output like UIs but is a helpful and compact representation for debug or log like
  /// output.
  /// @param duration Any type of std::chrono::duration, that is casted to different resolutions to find the best
  /// fitting
  ///                 one.
  /// @return A string containing a formatted representation of the duration.
  template<typename Duration>
  std::string formattedDuration(const Duration& duration)
  {
    std::ostringstream result;
    if(std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(duration).count() < 100) {
      result << std::fixed << std::setprecision(1)
             << std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(duration).count() << "us";
    } else if(std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(duration).count() < 100) {
      result << std::fixed << std::setprecision(1)
             << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(duration).count() << "ms";
    } else if(std::chrono::duration_cast<std::chrono::duration<double>>(duration).count() < 100) {
      result << std::fixed << std::setprecision(1)
             << std::chrono::duration_cast<std::chrono::duration<double>>(duration).count() << "s";
    } else if(std::chrono::duration_cast<std::chrono::duration<double, std::ratio<60>>>(duration).count() < 100) {
      result << std::fixed << std::setprecision(1)
             << std::chrono::duration_cast<std::chrono::duration<double, std::ratio<60>>>(duration).count() << "min";
    } else {
      result << std::fixed << std::setprecision(1)
             << std::chrono::duration_cast<std::chrono::duration<double, std::ratio<3600>>>(duration).count() << "h";
    }
    return result.str();
  }

  /// Converts the string inplace into upper characters only.
  /// @param s The string to convert
  /// @return Returns A reference to the converted string.
  ACATL_EXPORT std::string& toupper(std::string& s);

  /// Converts the string into upper characters only.
  /// @param s The string to convert
  /// @return Returns The converted string.
  ACATL_EXPORT std::string toupper_copy(const std::string& s);

  /// Converts the string inplace into lower characters only.
  /// @param s The string to convert
  /// @return Returns A reference to the converted string.
  ACATL_EXPORT std::string& tolower(std::string& s);

  /// Converts the string into lower characters only.
  /// @param s The string to convert
  /// @return Returns The converted string.
  ACATL_EXPORT std::string tolower_copy(const std::string& s);

  /// Platform independent case insensitive string compare function.
  /// @param str1 First string to compare
  /// @param str2 Second string to compare
  /// @return A value less than zero, if str1 is less than str2, a value of zero, if str1 matches str2 and a value
  /// greater than zero, if str1 is greater than str2.
  ACATL_EXPORT int stricmp(const char* str1, const char* str2);

  /// Platform independent case insensitive string compare function.
  /// @param str1 First string to compare
  /// @param str2 Second string to compare
  /// @param count Number of characters to compare
  /// @return A value less than zero, if str1 is less than str2, a value of zero, if str1 matches str2 and a value
  /// greater than zero, if str1 is greater than str2. This is true up to the first count bytes thereof.
  ACATL_EXPORT int strnicmp(const char* str1, const char* str2, size_t count);

  /// Trims right whitespaces from string.
  /// @param s String to trim whitespaces from
  /// @return Returns the trimmed string.
  ACATL_EXPORT std::string trim_right(const std::string& s);

  /// Trims left whitespaces from string.
  /// @param s String to trim whitespaces from
  /// @return Returns the trimmed string.
  ACATL_EXPORT std::string trim_left(const std::string& s);

  /// Copy contents of one stream to another.
  /// @param src Stream to copy from
  /// @param dst Stream to copy to
  ACATL_EXPORT void stream_copy(std::istream& src, std::ostream& dst);

  /// Loads a file into a string. Will throw an exception if an error occurs.
  /// @param filepath The path to the file to load
  /// @return A string with the files contents
  ACATL_EXPORT std::string load_file(const filesystem::path& filepath);

  /// Loads a file into a string.
  /// @param filepath The path to the file to load
  /// @param ec Error code to set if an error occurs
  /// @return A string with the files contents
  ACATL_EXPORT std::string load_file(const filesystem::path& filepath, std::error_code& ec);

  /**
   * Decode url encoded string.
   * @param in The string to url decode
   * @param out The url decoded string
   * @return true, if decoding was successful, otherwise false.
   */
  ACATL_EXPORT bool decode(const std::string& in, std::string& out);
}

#endif
