#ifndef COMMONS_H
#define COMMONS_H

#include <ctime>
#include <chrono>
#include "lib/date-master/include/date/date.h"
#include "lib/date-master/include/date/tz.h"

using namespace date;
using namespace std::chrono;
using namespace std;

enum Timeframe
{
    YEARLY = 1,
    MONTHLY = 2,
    DAILY = 3
};

bool is_valid_year(const std::string &year_str);
bool is_valid_month(const std::string &month_str);
bool is_valid_date(const std::string &date_str);
string utc_to_ymd(const string &utc_time_str);
string parse_to_earliest_utc_string(const std::string &date_str, Timeframe tf)

#endif
