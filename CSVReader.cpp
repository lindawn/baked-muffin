#include "CSVReader.h"
#include <fstream>
#include <sstream>

std::vector<std::vector<std::string>> CSVReader::readCSV(const std::string &filename)
{
    std::vector<std::vector<std::string>> data;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line))
    {
        std::vector<std::string> row;
        std::stringstream lineStream(line);
        std::string cell;

        while (std::getline(lineStream, cell, ','))
        {
            row.push_back(cell);
        }

        data.push_back(row);
    }

    return data;
}