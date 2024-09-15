#include "Candlestick.h"
#include "Commons.h"

using namespace std;

Candlestick::Candlestick(string startDate, enum Timeframe timeframe, double open, double high, double low,
                         double close)
    : startDate{startDate}, timeframe{timeframe}, open{open}, high{high}, low{low}, close{close} {}

Candlestick::Candlestick(string startDate, enum Timeframe timeframe)
    : startDate{startDate}, timeframe{timeframe} {}
