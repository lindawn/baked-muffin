#include "Commons.h"
#include <regex>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>

using namespace date;
using namespace std::chrono;
using namespace std;

// Function to validate a yearly date string (YYYY)
bool is_valid_year(const std::string &year_str)
{
    std::regex year_format_regex(R"(^\d{4}$)");
    if (!std::regex_match(year_str, year_format_regex))
    {
        return false;
    }
    int year = std::stoi(year_str);
    return year >= 1970 && year <= 2019;
}

// Function to validate a monthly date string (YYYY/MM)
bool is_valid_month(const std::string &month_str)
{
    std::regex month_format_regex(R"(^\d{4}/\d{2}$)");
    if (!std::regex_match(month_str, month_format_regex))
    {
        return false;
    }

    std::istringstream in{month_str};
    year_month ym;
    in >> parse("%Y/%m", ym);
    if (in.fail())
    {
        return false;
    }

    int year = int(ym.year());
    if (year < 1970 || year > 2019)
    {
        return false;
    }

    return ym.ok();
}

// Function to validate a daily date string (YYYY/MM/DD)
bool is_valid_date(const std::string &date_str)
{
    std::regex date_format_regex(R"(^\d{4}/\d{2}/\d{2}$)");
    if (!std::regex_match(date_str, date_format_regex))
    {
        return false;
    }

    std::istringstream in{date_str};
    sys_days date_point;
    in >> parse("%Y/%m/%d", date_point);
    if (in.fail())
    {
        return false;
    }

    year_month_day ymd{date_point};
    int year = int(ymd.year());
    if (year < 1970 || year > 2019)
    {
        return false;
    }

    return ymd.ok();
}

string utc_to_ymd(const string &utc_time_str)
{
    // Parse the UTC time string
    istringstream in{utc_time_str};
    sys_seconds tp;
    in >> parse("%Y-%m-%dT%H:%M:%SZ", tp);

    if (in.fail())
    {
        throw invalid_argument("Failed to parse the UTC time string");
    }

    // Convert to year_month_day
    auto ymd = year_month_day{floor<days>(tp)};

    // Format as YYYY-MM-DD
    ostringstream out;
    out << ymd;
    return out.str();
};

std::string parse_to_earliest_utc_string(const std::string &date_str, Timeframe tf)
{
    year y;
    year_month ym;
    year_month_day ymd;
    sys_seconds utc_time;

    std::istringstream in{date_str};

    switch (tf)
    {
    case Timeframe::YEARLY:
        in >> parse("%Y", y);
        utc_time = sys_days{y / January / 1};
        break;
    case Timeframe::MONTHLY:
        in >> parse("%Y/%m", ym);
        utc_time = sys_days{ym / 1};
        break;
    case Timeframe::DAILY:
        in >> parse("%Y/%m/%d", ymd);
        utc_time = sys_days{ymd};
        break;
    default:
        throw std::invalid_argument("Invalid date format");
    }

    // Convert sys_seconds to a string in UTC format
    auto dp = floor<days>(utc_time);
    auto time = make_time(utc_time - dp);
    year_month_day ymd_result{dp};

    std::ostringstream out;
    out << ymd_result << 'T' << time.hours().count() << ':' << time.minutes().count() << ':' << time.seconds().count() << 'Z';
    return out.str();
}