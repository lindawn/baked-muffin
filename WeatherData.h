#pragma once
#include <string>
#include <vector>
#include "WeatherEntry.h"
#include "Candlestick.h"

class WeatherData
{
public:
    WeatherData(const std::string &filename);
    std::vector<std::string> getKnownCountries() const;
    std::vector<WeatherEntry> getEntries(const std::string &country, std::string startDate, std::string endDate) const;
    // vector<WeatherEntry> WeatherData::getEntries(string country, string startDate, string endDate) const;
    std::vector<Candlestick> computeCandlestickData(const vector<WeatherEntry> data, const enum Timeframe &timeframe) const;

private:
    std::vector<WeatherEntry> entries; // all entries from csv
    std::vector<string> countries;
    string firstTimestamp;
    string lastTimestamp;
};