#ifndef CANDLESTICK_H
#define CANDLESTICK_H
#include <string>
#include "Commons.h"
using namespace std;

class Candlestick
{
private:
    string startDate;
    enum Timeframe timeframe;
    double open;
    double high;
    double low;
    double close;

public:
    Candlestick(string startDate, enum Timeframe timeframe, double open, double high, double low, double close);
    Candlestick(string startDate, enum Timeframe timeframe);
    double getOpen() { return open; }
    double getHigh() { return high; }
    double getLow() { return low; }
    double getClose() { return close; }
    enum Timeframe getTimeframe() { return timeframe; }
    string getStartDate() { return startDate; }
    void setOpen(double o) { open = o; }
    void setHigh(double h) { high = h; }
    void setLow(double l) { low = l; }
    void setClose(double c) { close = c; }
};

#endif