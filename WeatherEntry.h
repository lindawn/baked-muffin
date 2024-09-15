#pragma once
#include <string>

class WeatherEntry {
public:
    WeatherEntry(const std::string& timestamp, const std::string& country, double temperature);
    std::string timestamp;
    std::string country;
    double temperature;
};