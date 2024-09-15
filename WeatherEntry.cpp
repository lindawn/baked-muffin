#include "WeatherEntry.h"

WeatherEntry::WeatherEntry(const std::string& timestamp, const std::string& country, double temperature)
    : timestamp(timestamp), country(country), temperature(temperature) {}