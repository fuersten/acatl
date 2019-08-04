//
//  date_helper.h
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

#ifndef acatl_date_helper_h
#define acatl_date_helper_h

#include <acatl/inc.h>

#include <date/date.h>
#include <date/iso_week.h>


namespace acatl
{
  /// Returns the current time in milliseconds.
  /// @return The current time in milliseconds.
  inline std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> now()
  {
    return std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
  }

  /// Calculates the day of the year.
  /// @param d The date to calculate the day of the year for.
  /// @return The day of the year in the range [0, 365].
  inline int day_of_year(const date::year_month_day& d)
  {
    return (std::chrono::duration_cast<date::days>(date::sys_days{d} - date::sys_days{d.year() / date::January / 1})
            + date::days{1})
      .count();
  }

  /// Adds the given days to the date.
  /// @param d The date to add days to.
  /// @param days The days to add.
  /// @return The date with days added.
  inline date::sys_days add_days(const date::sys_days& d, const date::days& days)
  {
    return d + days;
  }

  /// Adds the given months to the date. The day of the month will be truncated to the max days of the resulting month,
  /// if the days calculated are more than that month actually has.
  /// @param d The date to add months to.
  /// @param days The months to add.
  /// @return The date with months added.
  inline date::sys_days add_months(const date::year_month_day& d, const date::months& months)
  {
    auto res = date::year_month_day{d + months};

    if(!res.ok()) {
      auto mdl = date::year_month_day_last{res.year(), date::month_day_last{res.month()}};
      if(res.day() > mdl.day()) {
        res = mdl;
      }
    }
    return res;
  }

  /// Returns a formatted string representation of the date using the given format.
  /// @param fmt The format to use.
  /// @param d The date to format.
  /// @return The formatted date as string.
  inline std::string to_string(const char* fmt, const date::year_month_day& d)
  {
    std::stringstream ss;
    date::to_stream(ss, fmt, d);
    return ss.str();
  }

  /// Returns a formatted string representation of the time using the given format.
  /// @param fmt The format to use.
  /// @param tod The time to format.
  /// @return The formatted date as string.
  inline std::string to_string(const char* fmt, const date::time_of_day<std::chrono::milliseconds>& tod)
  {
    return date::format(fmt, tod.to_duration());
  }

  /// Returns the parsed date from the string.
  /// @param date The date to parse.
  /// @return The parsed date.
  inline date::year_month_day date_from_string(const std::string& date)
  {
    std::istringstream iss{date};
    date::year_month_day ymd;
    date::from_stream(iss, "%F", ymd);
    return ymd;
  }

  /// Calculates the date for the gregorian easter holiday.
  /// @param year The year to calculate the gregorian easter holiday.
  /// @return The gregorian easter holiday.
  inline date::sys_days calculateGregorianEaster(const date::year& year)
  {
    int32_t y = (int)year;
    uint16_t H = (24 + 19 * (y % 19)) % 30;
    uint16_t I = H - (H / 28);
    uint16_t J = (y + (y / 4) + I - 13) % 7;
    int16_t L = static_cast<int16_t>(I - J);
    uint16_t EM = static_cast<uint16_t>(3 + ((L + 40) / 44));
    uint16_t ED = static_cast<uint16_t>(L + 28 - 31 * (EM / 4));
    date::sys_days easterSunday = date::year_month_day{year / EM / ED};
    return easterSunday;
  }
}

#endif
