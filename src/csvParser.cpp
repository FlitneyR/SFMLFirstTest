#include "csvParser.hpp"

#include <iostream>
#include <string>
#include <vector>

std::string CSVParser::getNextLine(std::istream& is, char delim) {
    std::string s;
    
    for (char c; !is.get(c).eof();)
        if (c == delim) return s;
        else s += c;

    return s;
}

std::vector<std::string> CSVParser::splitString(const std::string& s, char delim) {
    std::vector<std::string> results;

    std::string str;
    for (auto& c : s) {
        if (c == delim) {
            results.push_back(str);
            str = "";
        } else str += c;
    }

    if (str != "") results.push_back(str);

    return results;
}

CSVParser::CSVParser(std::istream& is, bool columnNamesInFirstRow) {
    if (columnNamesInFirstRow)
        m_columnNames = splitString(getNextLine(is));
    
    while (!is.eof()) {
        m_rows.push_back(splitString(getNextLine(is)));

        while (m_rows.back().size() > m_columnNames.size())
            m_columnNames.push_back("");
    }

    if (m_rows.size() > 0 && m_rows.back().size() == 0) m_rows.pop_back();
}

unsigned int CSVParser::getColumnCount() const {
    return m_columnNames.size();
}

unsigned int CSVParser::getRowCount() const {
    return m_rows.size();
}

unsigned int CSVParser::getColumnIndex(const std::string& name) const {
    for (int i = 0; i < m_columnNames.size(); i++)
    if (m_columnNames[i] == name)
        return i;

    throw std::out_of_range("Unknown column name: " + name);
}

const std::vector<std::string>& CSVParser::getColumnNames() const {
    return m_columnNames;
}

std::vector<std::string>& CSVParser::getRow(unsigned int row) {
    return m_rows[row];
}

std::string& CSVParser::getCell(unsigned int row, unsigned int column) {
    return getRow(row)[column];
}

std::string& CSVParser::getCell(unsigned int row, const std::string& column) {
    return getRow(row)[getColumnIndex(column)];
}
