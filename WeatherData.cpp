#include "WeatherData.h"
#include "WeatherEntry.h"
#include "CSVReader.h"
#include <iostream>
#include "Commons.h"
#include <algorithm>
#include <vector>
#include <chrono>
#include "lib/date-master/include/date/date.h"
#include "lib/date-master/include/date/tz.h"
#include <limits>

using namespace std;
using namespace std::chrono;
using date_time = std::chrono::system_clock::time_point;

sys_time<seconds> strToDate(const std::string &datetime_str)
{
    std::istringstream in{datetime_str};
    sys_time<seconds> tp;
    in >> parse("%Y-%m-%dT%H:%M:%SZ", tp);
    return tp;
}

sys_time<seconds> incrementDate(Timeframe tf, const sys_time<seconds> &currDate)
{ //  handles year/month/day separately to maintain accuracy with UTC
    auto dp = floor<days>(currDate);
    auto ymd = year_month_day{dp};
    auto time_of_day = currDate - dp;

    switch (tf)
    {
    case YEARLY:
        ymd += years(1);
        break;
    case MONTHLY:
        ymd += months(1);
        break;
    case DAILY:
        return currDate + days(1);
        break;
    }
    return sys_days(ymd) + time_of_day;
}

WeatherData::WeatherData(const std::string &filename)
{
    // Read data from CSV file and populate entries
    CSVReader reader;
    std::vector<std::vector<std::string>> data = reader.readCSV(filename);

    // process header row for list of countries
    std::vector<std::string> ctries;
    std::vector<std::string> heading = data[0];
    for (int i = 1; i < heading.size(); i++) // ignore timestamp column: start with index 1
    {
        ctries.push_back(heading[i].substr(0, 2));
    }
    countries = ctries;
    for (int r = 1; r < data.size(); r++) // ignore heading row: start with index 1
    {
        string timestamp = data[r][0];

        for (int c = 1; c < data[r].size(); c++) // ignore timestamp column: start with index 1
        {
            string ctry = data[0][c].substr(0, 2);
            double temp = stod(data[r][c]);
            WeatherEntry we(timestamp, ctry, temp);
            entries.push_back(we);
        }
    }
    firstTimestamp = entries[0].timestamp;
    lastTimestamp = entries[entries.size() - 1].timestamp;
}

std::vector<std::string> WeatherData::getKnownCountries() const
{
    return countries;
}

std::vector<WeatherEntry> WeatherData::getEntries(const std::string &country, std::string startDate, std::string endDate) const
{
    vector<WeatherEntry> filtered;
    if (startDate == "" && endDate == "")
    {
        // do not filter by dates
        copy_if(entries.begin(), entries.end(), back_inserter(filtered), [&](WeatherEntry we)
                { return (we.country == country); });
        return filtered;
    }

    startDate = (startDate == "") ? firstTimestamp : startDate;
    endDate = (endDate == "") ? lastTimestamp : endDate;
    copy_if(entries.begin(), entries.end(), back_inserter(filtered), [&](WeatherEntry we)
            { return (we.country == country) && (we.timestamp >= startDate) && (we.timestamp <= endDate); });

    return filtered;
}

// Given: timeframe(yearly/monthly/daily), country code
// Compute: candlesticks for entire data set using moving window algorithm to generate candlesticks.
std::vector<Candlestick> WeatherData::computeCandlestickData(const vector<WeatherEntry> data, const enum Timeframe &timeframe) const
{

    vector<Candlestick> stickList = {};
    vector<WeatherEntry> filtered = data;

    sys_time<seconds> startDate;
    startDate = strToDate(filtered[0].timestamp);
    sys_time<seconds> endDate = incrementDate(timeframe, startDate);

    vector<int> windowVec = {0};

    // 1. Generate array of indices for points to take for start/end
    for (int i; i < filtered.size(); i++)
    {
        sys_time<seconds> currDate = strToDate(filtered[i].timestamp);
        if (currDate >= endDate)
        {
            endDate = incrementDate(timeframe, currDate);
            windowVec.push_back(i);
        };
    }

    // 2. iterate through windowVec array and generate candlestick for each window
    double open = numeric_limits<double>::quiet_NaN();
    for (int l = 0; l < (windowVec.size() - 1); l++)
    {
        int left = windowVec[l], right = windowVec[l + 1];
        double close, high, low, sum = 0.0;
        Candlestick stick(filtered[left].timestamp, timeframe); // init candlestick with start date and timeframe
        stick.setOpen(open);
        high = -9999, low = 9999;
        for (int i = left; i < right; i++)
        {
            sum += filtered[i].temperature;
            (filtered[i].temperature > high) ? high = filtered[i].temperature : high = high;
            (filtered[i].temperature < low) ? low = filtered[i].temperature : low = low;
        }
        close = sum / (windowVec[l + 1] - windowVec[l] + 1);
        stick.setClose(close);
        stick.setHigh(high);
        stick.setLow(low);
        open = close;
        stickList.push_back(stick);

        string sd = stick.getStartDate();
    }
    return stickList;
}
