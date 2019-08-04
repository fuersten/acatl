//
//  main.cpp
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

#include <algorithm>
#include <iostream>
#include <iomanip>

#include <acatl/date_helper.h>

#include <acatl_application/application.h>
#include <acatl_application/command_line_options.h>


class Calendar : public acatl::Application
{
public:
  Calendar(int argc, char** argv)
  : acatl::Application(argc, argv)
  {
  }

private:
  using holidayType = std::pair<std::string, date::year_month_day>;

  bool setUp(const acatl::StringVector& args) override
  {
    // clang-format off
        acatl::CommandLineOptions options("calendar", {
            {
                "help", {
                    {"", "help", 1, 1, "display this help and exit"}
                }
            },
            {
                "year options", {
                    {"y", "year", "<YEAR>", 0, 1, "show the complete year"},
                    {"j", "", 0, 1, "show the current year"},
                    {"h", "holidays", 0, 1, "show the holidays"}
                }
            },
            {
                "month options", {
                    {"m", "month", "<MONTH>", 0, 1, "show the specified month [1..12]"},
                    {"h", "holidays", 0, 1, "show the holidays"}
            }
          }
        });
    // clang-format on

    std::stringstream ss;
    auto ret = options.parse(args, ss);

    if(!ret) {
      std::cerr << ss.str() << std::endl;
      options.usage(std::cerr);
      return false;
    }

    if(options.count("help") > 0) {
      options.usage(std::cerr);
      return false;
    }

    if(options.count("j") > 0) {
      _showYear = true;
    }

    if(options.count("year") > 0) {
      _showYear = true;
      _currentDate = date::year{options.option("year").value<int>()} / date::January / 1;
    }

    if(options.count("month") > 0) {
      _currentDate = _currentDate.year() / date::month{options.option("month").value<unsigned int>()} / 1;
    }

    if(options.count("holidays") > 0) {
      _showHolidays = true;
    }

    return true;
  }

  int doRun() override
  {
    if(_showYear) {
      printYear();
    } else {
      printSingleMonth();
    }

    return 0;
  }

  void tearDown(int exitCode) override
  {
  }

  void printSingleMonth()
  {
    std::cout << std::setw(9) << " " << _currentDate.month() << " " << _currentDate.year() << std::endl << std::endl;
    std::cout << " So  Mo  Di  Mi  Do  Fr  Sa " << std::endl;

    date::year_month_day thisMonth{_currentDate.year() / _currentDate.month() / 1};
    while(printWeek(thisMonth.month(), thisMonth)) {
      std::cout << std::endl;
    };

    std::cout << std::endl;

    if(_showHolidays) {
      std::cout << std::endl;
      printHolidaysForMonth(_currentDate.month());
    }
  }

  void printYear()
  {
    std::vector<std::pair<date::month, date::year_month_day>> months;
    auto date{_currentDate.year() / date::January / 1};
    months.emplace_back(date::January, date);
    months.emplace_back(date::February, date + date::months{1});
    months.emplace_back(date::March, date + date::months{2});
    months.emplace_back(date::April, date + date::months{3});
    months.emplace_back(date::May, date + date::months{4});
    months.emplace_back(date::June, date + date::months{5});
    months.emplace_back(date::July, date + date::months{6});
    months.emplace_back(date::August, date + date::months{7});
    months.emplace_back(date::September, date + date::months{8});
    months.emplace_back(date::October, date + date::months{9});
    months.emplace_back(date::November, date + date::months{10});
    months.emplace_back(date::December, date + date::months{11});

    std::cout << std::setw(40) << " " << _currentDate.year() << std::endl << std::endl;

    printThreeMonthWeeks(0, months);
    std::cout << std::endl;
    printThreeMonthWeeks(3, months);
    std::cout << std::endl;
    printThreeMonthWeeks(6, months);
    std::cout << std::endl;
    printThreeMonthWeeks(9, months);
    std::cout << std::endl;

    std::cout << std::endl;

    if(_showHolidays) {
      std::cout << std::endl;
      printAllHolidays();
    }
  }

  void printThreeMonthWeeks(uint16_t startMonthIndex, std::vector<std::pair<date::month, date::year_month_day>>& months)
  {
    std::cout << std::setw(12) << " " << months[startMonthIndex].first << " " << std::setw(25) << " "
              << months[startMonthIndex + 1].first << " " << std::setw(25) << " " << months[startMonthIndex + 2].first
              << std::endl
              << std::endl;
    std::cout << " So  Mo  Di  Mi  Do  Fr  Sa "
              << " "
              << " So  Mo  Di  Mi  Do  Fr  Sa "
              << " "
              << " So  Mo  Di  Mi  Do  Fr  Sa " << std::endl;

    bool res{true};
    while(res) {
      res = false;
      res |= printWeek(months[startMonthIndex].first, months[startMonthIndex].second);
      std::cout << " ";
      res |= printWeek(months[startMonthIndex + 1].first, months[startMonthIndex + 1].second);
      std::cout << " ";
      res |= printWeek(months[startMonthIndex + 2].first, months[startMonthIndex + 2].second);
      std::cout << std::endl;
    }
  }

  bool printWeek(date::month month, date::year_month_day& date)
  {
    if(date.month() != month) {
      std::cout << std::setw(28) << " ";
      return false;
    }
    auto weekday = date::weekday{date};
    if(date.day() == date::day{1}) {
      printStartOffset(weekday);
    }
    printDay(date);
    while(weekday != date::Saturday) {
      date = acatl::add_days(date, date::days{1});
      if(date.month() != month) {
        auto diff = date::Saturday - date::weekday{date};
        std::cout << std::setw((diff.count() * 4) + 4) << " ";
        return false;
      }

      printDay(date);
      weekday = date::weekday{date};
    }
    date = acatl::add_days(date, date::days{1});
    return date.month() == month;
  }

  void printStartOffset(date::weekday weekday)
  {
    auto offset = weekday.operator unsigned int() * 4;
    if(offset) {
      std::cout << std::setw(static_cast<int>(offset)) << " ";
    }
  }

  void printDay(const date::year_month_day& date)
  {
    if(_showHolidays && isHoliday(date)) {
      std::cout << "[" << date.day() << "]";
    } else {
      std::cout << std::setw(1) << " " << date.day() << std::setw(1) << " ";
    }
  }

  bool isHoliday(const date::year_month_day& date)
  {
    return std::find_if(
             _holidays.begin(), _holidays.end(), [&date](const holidayType& holiday) { return holiday.second == date; })
           != _holidays.end();
  }

  void printHolidaysForMonth(date::month month)
  {
    for(const auto& holiday : _holidays) {
      if(holiday.second.month() == month) {
        std::cout << holiday.first << ": " << acatl::to_string("%d.%m.", holiday.second) << std::endl;
      }
    }
  }

  void printAllHolidays()
  {
    for(const auto& holiday : _holidays) {
      std::cout << holiday.first << ": " << acatl::to_string("%d.%m.", holiday.second) << std::endl;
    }
  }

  static std::vector<holidayType> calculateHolidays(date::year year)
  {
    std::vector<holidayType> holidays;

    holidays.emplace_back("Neujahr", date::year_month_day{year / date::January / 1});
    holidays.emplace_back("Heilige Drei Koenige", date::year_month_day{year / date::January / 6});
    holidays.emplace_back("Erster Mai", date::year_month_day{year / date::May / 1});
    holidays.emplace_back("Mariae Himmelfahrt", date::year_month_day{year / date::August / 15});
    holidays.emplace_back("Tag der deutschen Einheit", date::year_month_day{year / date::October / 3});
    holidays.emplace_back("Reformationstag", date::year_month_day{year / date::October / 31});
    holidays.emplace_back("Allerheiligen", date::year_month_day{year / date::November / 11});
    holidays.emplace_back("Erster Weihnachtstag", date::year_month_day{year / date::December / 25});
    holidays.emplace_back("Zweiter Weihnachtstag", date::year_month_day{year / date::December / 26});

    auto easter = acatl::calculateGregorianEaster(year);

    holidays.emplace_back("Rosenmontag", acatl::add_days(easter, date::days{-48}));
    holidays.emplace_back("Fastnacht", acatl::add_days(easter, date::days{-47}));
    holidays.emplace_back("Aschermittwoch", acatl::add_days(easter, date::days{-46}));
    holidays.emplace_back("Palmsonntag", acatl::add_days(easter, date::days{-7}));
    holidays.emplace_back("Karfreitag", acatl::add_days(easter, date::days{-2}));
    holidays.emplace_back("Ostersonntag", easter);
    holidays.emplace_back("Ostermontag", acatl::add_days(easter, date::days{1}));
    holidays.emplace_back("Christi Himmelfahrt", acatl::add_days(easter, date::days{39}));
    holidays.emplace_back("Pfingstsonntag", acatl::add_days(easter, date::days{49}));
    holidays.emplace_back("Pfingstmontag", acatl::add_days(easter, date::days{50}));
    holidays.emplace_back("Fronleichnam", acatl::add_days(easter, date::days{60}));

    holidays.emplace_back("Sommerzeit-Umstellung",
                          date::year_month_weekday_last{year / date::March / date::Sunday[date::last]});
    holidays.emplace_back("Winterzeit-Umstellung",
                          date::year_month_weekday_last{year / date::October / date::Sunday[date::last]});

    std::sort(holidays.begin(), holidays.end(), [](const holidayType& lhs, const holidayType& rhs) {
      return lhs.second < rhs.second;
    });

    return holidays;
  }

  bool _showYear;
  date::year_month_day _currentDate{date::floor<date::days>(std::chrono::system_clock::now())};
  bool _showHolidays{false};
  std::vector<holidayType> _holidays = calculateHolidays(_currentDate.year());
};

int main(int argc, char** argv)
{
  return Calendar{argc, argv}.run();
}
