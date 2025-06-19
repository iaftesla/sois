#ifndef CSV_PARSER_HPP
#define CSV_PARSER_HPP

#include <istream>
#include <string>
#include <vector>

namespace csv {

/** Parse CSV from input stream. */
std::vector<std::vector<std::string>> parseCSV(std::istream& is);

}

#endif
