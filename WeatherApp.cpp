#include "WeatherApp.h"
#include "WeatherData.h"
#include "TextPlotter.h"
#include "Commons.h"
#include <iostream>
#include <numeric>
#include <vector>
#include <sstream>

using namespace std;

WeatherApp::WeatherApp() : weatherData("data.csv") {}

void WeatherApp::init()
{
    while (true)
    {
        printMenu();
        int option = getUserOption();
        processUserOption(option);
    }
}

void WeatherApp::printMenu()
{
    std::cout << "1. Compute candlestick data\n";
    std::cout << "2. Create text-based plot\n";
    std::cout << "3. Filter and plot data\n";
    std::cout << "4. Predict and plot data\n";
    std::cout << "5. Exit\n";
}

// prints ctry list, parses user input
string WeatherApp::printCtryMenu()
{
    string ctry;
    bool valid = false;
    cout << "Select country code from list:\n\n";
    while (!valid)
    {
        vector<string> list = weatherData.getKnownCountries();
        stringstream ss;
        for (int it = 0; it < list.size(); it++)
        {
            ss << list[it] << "  ";
            if (it > 0 && (it % 5) == 0)
            {
                ss << "\n";
            }
        }
        cout << ss.str() << endl
             << "\n";
        cin >> ctry;
        if (find(list.begin(), list.end(), ctry) != list.end())
        {
            valid = true;
        }
        else
        {
            cout << "\nInvalid country code. Please enter a code from the list in upper case only.\n\n";
        }
    }
    return ctry;
}

Timeframe WeatherApp::printTimeframeMenu()
{
    Timeframe tf;
    bool valid = false;
    while (!valid)
    {
        cout << "Select timeframe:\n1. Yearly\n2. Monthly\n3. Daily\n";
        int inp;
        cin >> inp;
        if (inp < 1 || inp > 3)
        {
            cout << "\n Invalid option, please pick option 1, 2, or 3. \n";
        }
        else
        {
            valid = true;
            tf = static_cast<Timeframe>(inp);
        }
    }
    return tf;
}

int WeatherApp::getUserOption()
{
    int option;
    std::cin >> option;
    return option;
}

void WeatherApp::processUserOption(int userOption)
{
    switch (userOption)
    {
    case 1:
        computeCandlestickData();
        break;
    case 2:
        createTextPlot();
        break;
    case 3:
        filterAndPlotData();
        break;
    case 4:
        predictAndPlotData();
        break;
    case 5:
        exit(0);
    default:
        std::cout << "Invalid option\n";
    }
}

// Requests country choice, and calculates candlestick data for entire data set.
// Requires timeframe as it is required for each candlestick.
void WeatherApp::computeCandlestickData()
{
    string ctry = printCtryMenu();
    Timeframe tf = printTimeframeMenu();

    vector<WeatherEntry> filtered = weatherData.getEntries(ctry, "", "");
    computedCandlesticks = weatherData.computeCandlestickData(filtered, tf);
    cout << computedCandlesticks.size() << " candlesticks created. Execute (2) to see plot. \n";
}

// Creates candlestick plot for candlestick data that has been computed in Task 1
void WeatherApp::createTextPlot()
{
    if (!computedCandlesticks.size())
    {
        cout << "No candlestick data computed yet! Please execute (1) first." << "\n";
    }
    else
    {
        cout << "\n";
        textPlotter.plotCandlestick(computedCandlesticks);
    }
}

// filters by: country and date. Timeframe is requested, to facilitate clean date entry data.
// eg. if a yearly data plot is requested, start date will be 01 January of each year.
void WeatherApp::filterAndPlotData()
{

    string ctry = printCtryMenu();
    Timeframe tf = printTimeframeMenu();

    string startDate = "", endDate = "";

    bool valid = false;
    while (!valid)
    {
        switch (tf)
        {
        case 1:
            cout << "Enter starting year in format YYYY: \n";
            cin >> startDate;
            cout << "Enter ending year in format YYYY: \n";
            cin >> endDate;
            valid = (is_valid_year(endDate) && is_valid_year(endDate));
            break;
        case 2:
            cout << "Enter starting month in format YYYY/MM: \n";
            cin >> startDate;
            cout << "Enter ending month in format YYYY/MM: \n";
            cin >> endDate;
            valid = (is_valid_month(startDate) && is_valid_month(endDate));
            break;
        case 3:
            cout << "Enter starting date in format YYYY/MM/DD: \n";
            cin >> startDate;
            cout << "Enter ending date in format YYYY/MM: \n";
            cin >> endDate;
            valid = (is_valid_date(startDate) && is_valid_date(endDate));
            break;
        default:
            break;
        }
        if (!valid)
        {
            cout << "Invalid input: Enter date in requested format, 1980/01/01 - 2019/12/31. \n";
        }
    }

    vector<WeatherEntry> filtered = weatherData.getEntries(ctry, parse_to_earliest_utc_string(startDate, tf), parse_to_earliest_utc_string(endDate, tf));
    textPlotter.plotLine(weatherData.computeCandlestickData(filtered, tf));
}

void WeatherApp::predictAndPlotData()
{
    // Implementation for Task 4
}
