#include <iostream>
#include <fstream>
#include <regex>
#include <string>

/** Extract Russian phone numbers from text file. */
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    std::ifstream infile(argv[1]);
    if (!infile) {
        std::cerr << "Cannot open file\n";
        return 1;
    }

    const std::regex phone_pattern(R"((?:\+7|8)\(\d{3}\)\d{3}-\d{2}-\d{2})");
    std::string line;
    while (std::getline(infile, line)) {
        auto begin = line.cbegin();
        std::smatch match;
        while (std::regex_search(begin, line.cend(), match, phone_pattern)) {
            std::cout << match.str() << '\n';
            begin = match.suffix().first;
        }
    }
    return 0;
}
