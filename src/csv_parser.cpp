#include "csv_parser.hpp"
#include <regex>
#include <string>

namespace csv {

/**
 * \brief Parse a simple CSV file.
 *
 * Quoted fields are supported. Returns 2D array of fields.
 */
std::vector<std::vector<std::string>> parseCSV(std::istream& is) {
    std::vector<std::vector<std::string>> table;
    const std::regex csv_regex(R"((?:\"([^\"]*)\"|([^,]+)|())(?:,|$))");
    std::string line;
    while (std::getline(is, line)) {
        std::vector<std::string> row;
        std::smatch m;
        auto it = line.cbegin();
        while (it != line.cend()) {
            if (std::regex_search(it, line.cend(), m, csv_regex)) {
                std::string field;
                if (m[1].matched) field = m[1].str();
                else if (m[2].matched) field = m[2].str();
                row.push_back(field);
                it = m.suffix().first;
            } else {
                break;
            }
        }
        table.push_back(row);
    }
    return table;
}

} // namespace csv
