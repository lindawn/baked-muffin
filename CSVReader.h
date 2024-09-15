#pragma once
#include <vector>
#include <string>

class CSVReader
{
public:
    static std::vector<std::vector<std::string>> readCSV(const std::string &filename);
};