#pragma once
#include <vector>
#include <string>
#include "Candlestick.h"

class TextPlotter
{
public:
    std::string plotCandlestick(const std::vector<Candlestick> &data);
    std::string plotLine(const std::vector<Candlestick> &data);
    TextPlotter();
};