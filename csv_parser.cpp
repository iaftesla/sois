#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include "csv_parser.hpp"

/** Simple CSV file parser demo. */
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <csv_file>\n";
        return 1;
    }
    std::ifstream infile(argv[1]);
    if (!infile) {
        std::cerr << "Cannot open file\n";
        return 1;
    }

    std::vector<std::vector<std::string>> table = csv::parseCSV(infile);

    // Compute column widths
    std::vector<size_t> widths;
    for (const auto& row : table) {
        if (row.size() > widths.size()) widths.resize(row.size(), 0);
        for (size_t i = 0; i < row.size(); ++i) {
            widths[i] = std::max(widths[i], row[i].size());
        }
    }

    // Print table
    for (const auto& row : table) {
        for (size_t i = 0; i < row.size(); ++i) {
            std::cout << std::left << std::setw(widths[i] + 2) << row[i];
        }
        std::cout << '\n';
    }
    return 0;
}
