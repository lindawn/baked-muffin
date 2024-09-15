#include "TextPlotter.h"
#include "Commons.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>

using namespace std;

int gridHeight = 41; // default : 41. A grid height that takes about 3/4 of a reasonable screen height
int gridWidth = 20;  // default : 20. num of plots allowed on one screen in x axis
int marginL = 1;
vector<string> xaxis;

int getRow(double value, double max, double coeff)
{
    return gridHeight - round((max - value) / coeff) - 1;
}

// init 2D vector to be filled, adding blank y axis
vector<vector<string>> getEmptyGrid(vector<Candlestick> sList)
{
    vector<vector<string>> grid(gridHeight);
    int colSize = sList.size() + 1;

    for (int i = 0; i < gridHeight; i++)
    {
        for (int j = 0; j < colSize; j++)
        {
            grid[i] = vector<string>(colSize);
            fill(grid[i].begin(), grid[i].end(), " ");
            grid[i][0] = "    |"; // 5 characters in total to allow for marking doubles on y axis
        }
    }

    return grid;
}

TextPlotter::TextPlotter() {};

// generates 2d vector for candlestick graph
vector<vector<string>> generateCandlestickGrid(vector<Candlestick> sList)
{
    vector<vector<string>> grid = getEmptyGrid(sList);

    // Generate dynamic Y axis by:
    // calculating top and bottom of graph (using min/max of candlestick set)
    double max = -9999, min = 9999;
    for (int i = 0; i < sList.size(); i++)
    {
        (sList[i].getHigh() > max) ? max = sList[i].getHigh() : max = max;
        (sList[i].getLow() < min) ? min = sList[i].getLow() : min = min;
    }

    double coeff = (max - min) / (gridHeight - 1);
    // mark y-axis
    for (int y = 0; y < gridHeight; y += 5)
    {
        grid[y][0] = to_string((max - min) - (y * coeff) + min).substr(0, 5);
    }

    // generates candlestick image
    for (int i = 0; i < sList.size(); i++)
    {
        Candlestick stick = sList[i];
        string empty = "       ";
        int c = i + marginL;
        if (stick.getOpen() != stick.getOpen())
        {
            // open is NAN, skip plot of open/close
            for (int n = 0; n < gridHeight; n++)
            {
                grid[n][c] = empty;
            }
        }
        else
        {
            // calculate rounded value
            int open = getRow(stick.getOpen(), max, coeff);
            int close = getRow(stick.getClose(), max, coeff);
            int low = getRow(stick.getLow(), max, coeff);
            int high = getRow(stick.getHigh(), max, coeff);

            string positiveCandle = "-------"; // used when close > open
            string negativeCandle = "xxxxxxx"; // used when open > close
            string stickStr = "   |   ";
            string point = "   *   ";

            string candle;
            candle = (stick.getOpen() > stick.getClose()) ? negativeCandle : positiveCandle; // nb: exact value in data entry is used, as graph precision might mark open/close on same row

            for (int n = 0; n < gridHeight; n++)
            {
                if (n == low || n == high)
                { // mark min/max points
                    grid[n][c] = point;
                    continue;
                }
                else if ((n >= open && n <= close) || (n <= open && n >= close))
                { // fill open/close portion
                    grid[n][c] = candle;
                }
                else if (n > high || n < low)
                {
                    grid[n][c] = empty;
                }
                else
                { // fill stick portion
                    grid[n][c] = stickStr;
                }
            };
        }
        if ((i % 4) == 0 && i != 0)
        {
            xaxis.push_back(stick.getStartDate());
        }
    }
    return grid;
}

// generates 2d vector for bar graph
vector<vector<string>> generateLineGrid(vector<Candlestick> sList)
{
    vector<vector<string>> grid = getEmptyGrid(sList);

    // Generate dynamic Y axis by:
    // calculating top and bottom of graph (using min/max close values)

    double max = -9999, min = 9999;
    for (int i = 0; i < sList.size(); i++)
    {
        (sList[i].getClose() > max) ? max = sList[i].getClose() : max = max;
        (sList[i].getClose() < min) ? min = sList[i].getClose() : min = min;
    }

    double coeff = (max - min) / (gridHeight - 1);
    // mark y-axis
    for (int y = 0; y < gridHeight; y += 5)
    {
        grid[y][0] = to_string((max - min) - (y * coeff) + min).substr(0, 5);
    }

    // populates bar chart in 2d vector
    for (int i = 0; i < sList.size(); i++)
    {
        Candlestick stick = sList[i];
        int c = i + marginL;

        int close = getRow(stick.getClose(), max, coeff);

        for (int n = 0; n < gridHeight; n++)
        {
            if (n >= close)
            {
                grid[n][c] = "xxxxxxx";
                continue;
            }
            else
            {
                grid[n][c] = "       ";
            }
        }
        if ((i % 4) == 0 && i != 0)
        {
            xaxis.push_back(stick.getStartDate());
        }
    }
    return grid;
}

// formats 2d vector into ascii graph, and creates x axis
string plotPage(vector<vector<string>> grid)
{
    string strPlot = "";
    stringstream ss;
    string xLine = "    +";
    stringstream labelLine;
    labelLine << "     ";
    // generate string to print
    for (int i = 0; i < grid.size(); i++)
    {
        string line = "";
        for (int j = 0; j < grid[0].size(); j++)
        {
            line.append(grid[i][j] + "    "); // 4 spaces
        }
        ss << line << "\n";
    }
    int ptr = 0;
    for (int i = 0; i < (grid[0].size() - 1); i++)
    {
        if ((i % 4) == 0 && i > 0)
        {
            xLine.append("-------+---"); // 11 total chars
            labelLine << utc_to_ymd(xaxis[ptr++]) << " ";
        }
        else
        {
            xLine.append("-----------");
            labelLine << "           ";
        }
    }
    ss << xLine << "\n";
    // generate x axis, tick every
    ss << labelLine.str() << "\n";

    return ss.str();
}

// generates grid for the current page with a sub vector and handles pagination
int plotGraph(const std::vector<Candlestick> &data, vector<vector<string>> (*callbackGridFn)(vector<Candlestick>))
{
    if (data.size() <= gridWidth)
    {
        xaxis = vector<string>(0);
        vector<vector<string>> grid = callbackGridFn(data);
        cout << plotPage(grid);
        return 0;
    }
    int page = 1;
    int maxPages = floor(data.size() / gridWidth) + 1;
    while (page != 0)
    {
        int want = page * gridWidth;
        int start = want - gridWidth;
        int diff = data.size() - want;
        int end = (diff <= 0) ? data.size() : want;

        vector<Candlestick> subVec = vector<Candlestick>(data.begin() + start, data.begin() + end);
        xaxis = vector<string>(0);
        vector<vector<string>> grid = callbackGridFn(subVec);

        cout << plotPage(grid);
        cout << "\nPage " << page << " (of " << data.size() << ").\n";
        bool valid = false;
        while (!valid)
        {
            cout << "\nEnter page number (1-" << maxPages << "), or 0 to exit viewing plots: \n";
            cin >> page;
            if (page < 0 || page > maxPages || !cin.good())
            {
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                cout << "\nInvalid input!\n";
            }
            else
            {
                valid = true;
            }
        }
    }
    return 0;
}

std::string TextPlotter::plotCandlestick(const std::vector<Candlestick> &data)
{
    plotGraph(data, *generateCandlestickGrid);
    return "";
}

std::string TextPlotter::plotLine(const std::vector<Candlestick> &data)
{
    plotGraph(data, *generateLineGrid);
    return "";
}
