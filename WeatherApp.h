#pragma once
#include <string>
#include <vector>
#include "WeatherData.h"
#include "TextPlotter.h"

class WeatherApp
{
public:
    WeatherApp();
    void init();

private:
    void printMenu();
    string printCtryMenu();
    Timeframe printTimeframeMenu();
    sys_time<seconds> printDateRangeMenu();
    void processUserOption(int userOption);
    int getUserOption();
    void computeCandlestickData();
    void createTextPlot();
    void filterAndPlotData();
    void predictAndPlotData();

    WeatherData weatherData;
    std::string currentTimestamp;
    vector<Candlestick> computedCandlesticks;
    TextPlotter textPlotter;
};