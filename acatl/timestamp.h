//
//  timestamp.h
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

#ifndef acatl_timestamp_h
#define acatl_timestamp_h

#include <acatl/inc.h>

#include <acatl/date_helper.h>
#include <acatl/exception.h>


namespace acatl
{
  ACATL_DECLARE_EXCEPTION(TimestampException, acatl::Exception);

  using TimePoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;


  constexpr inline TimePoint datetime_utc_to_timepoint(const date::year_month_day& ymd,
                                                       const std::chrono::hours& hours,
                                                       const std::chrono::minutes& minutes,
                                                       const std::chrono::seconds& seconds)
  {
    if(!ymd.ok()) {
      throw TimestampException("Invalid date", __FILE__, __LINE__);
    }

    return std::chrono::time_point_cast<std::chrono::milliseconds>(date::sys_days(ymd) + hours + minutes + seconds);
  }

  constexpr inline TimePoint datetime_utc_to_timepoint(const date::year_month_day& ymd,
                                                       const std::chrono::hours& hours,
                                                       const std::chrono::minutes& minutes,
                                                       const std::chrono::seconds& seconds,
                                                       const std::chrono::milliseconds& milliseconds)
  {
    if(!ymd.ok()) {
      throw TimestampException("Invalid date", __FILE__, __LINE__);
    }

    return std::chrono::time_point_cast<std::chrono::milliseconds>(date::sys_days(ymd) + hours + minutes + seconds
                                                                   + milliseconds);
  }

  constexpr inline TimePoint datetime_utc_to_timepoint(const date::year_month_day& ymd,
                                                       const date::time_of_day<std::chrono::milliseconds>& hmsm)
  {
    if(!ymd.ok()) {
      throw TimestampException("Invalid date", __FILE__, __LINE__);
    }

    return std::chrono::time_point_cast<std::chrono::milliseconds>(date::sys_days(ymd) + hmsm.to_duration());
  }

  class Timestamp
  {
  public:
    Timestamp() = default;

    Timestamp(const date::year_month_day& ymd,
              const std::chrono::hours& hours,
              const std::chrono::minutes& minutes,
              const std::chrono::seconds& seconds,
              const std::chrono::milliseconds& milliseconds)
    {
      ymd_ = ymd;
      hmsm_ = date::make_time(hours, minutes, seconds, milliseconds, 0);
    }

    Timestamp(const TimePoint& tp)
    {
      auto dp = date::floor<date::days>(tp);
      ymd_ = date::year_month_day{dp};
      hmsm_ = date::make_time(tp - dp);
    }

    constexpr date::year year() const
    {
      return ymd_.year();
    }

    constexpr date::month month() const
    {
      return ymd_.month();
    }

    constexpr date::day day() const
    {
      return ymd_.day();
    }

    constexpr std::chrono::hours hours() const
    {
      return hmsm_.hours();
    }

    constexpr std::chrono::minutes minutes() const
    {
      return hmsm_.minutes();
    }

    constexpr std::chrono::seconds seconds() const
    {
      return hmsm_.seconds();
    }

    constexpr std::chrono::milliseconds milliseconds() const
    {
      return hmsm_.subseconds();
    }

    constexpr TimePoint to_timepoint() const
    {
      return datetime_utc_to_timepoint(ymd_, hmsm_);
    }

    constexpr date::year_month_day ymd() const
    {
      return ymd_;
    }

    constexpr date::time_of_day<std::chrono::milliseconds> time() const
    {
      return hmsm_;
    }

    constexpr bool ok() const
    {
      return ymd_.ok() && hmsm_.in_conventional_range();
    }

  private:
    date::year_month_day ymd_;
    date::time_of_day<std::chrono::milliseconds> hmsm_;
  };

  inline acatl::TimePoint timepoint_from_string(const std::string& ts)
  {
    std::istringstream iss{ts};
    acatl::TimePoint tp;
    date::from_stream(iss, "%FT%T", tp);
    return tp;
  }


  inline std::string to_string(const char* fmt, const TimePoint& tp)
  {
    return date::format(fmt, tp);
  }

  inline std::string to_string(const char* fmt, const Timestamp& ts)
  {
    return date::format(fmt, ts.to_timepoint());
  }
}

#endif
