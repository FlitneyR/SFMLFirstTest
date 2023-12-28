#pragma once

#include <string>
#include <vector>
#include <iostream>

class CSVParser {
    std::vector<std::string> m_columnNames;
    std::vector<std::vector<std::string>> m_rows;

    static std::string getNextLine(std::istream& is, char delim = '\n');
    static std::vector<std::string> splitString(const std::string& s, char delim = ',');

public:
    CSVParser() = default;
    CSVParser(std::istream& is, bool columnNamesInFirstRow = true);

    unsigned int getColumnCount() const;
    unsigned int getRowCount() const;
    unsigned int getColumnIndex(const std::string& name) const;

    const std::vector<std::string>& getColumnNames() const;

    std::vector<std::string>& getRow(unsigned int row);
    std::string& getCell(unsigned int row, unsigned int column);
    std::string& getCell(unsigned int row, const std::string& columnName);
};
