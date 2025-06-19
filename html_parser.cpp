#include <iostream>
#include <fstream>
#include <regex>
#include <string>

/** Simple HTML tag parser. */
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <html_file>\n";
        return 1;
    }
    std::ifstream infile(argv[1]);
    if (!infile) {
        std::cerr << "Cannot open file\n";
        return 1;
    }

    std::string content((std::istreambuf_iterator<char>(infile)),
                         std::istreambuf_iterator<char>());

    const std::regex tag_regex(R"(<([A-Za-z0-9]+)([^>]*)>)");
    const std::regex attr_regex(
        R"(([A-Za-z0-9:-]+)(?:\s*=\s*(?:\"([^\"]*)\"|'([^']*)'|([^\\s\"'>]+)))?)");

    std::smatch tag_match;
    auto it = content.cbegin();
    while (std::regex_search(it, content.cend(), tag_match, tag_regex)) {
        std::string tag_name = tag_match[1];
        std::string attr_str = tag_match[2];
        std::cout << "Tag: " << tag_name << '\n'
                  << "Attributes:\n";
        std::smatch attr_match;
        auto ait = attr_str.cbegin();
        while (std::regex_search(ait, attr_str.cend(), attr_match, attr_regex)) {
            std::string name = attr_match[1];
            std::string value;
            if (attr_match[2].matched) value = attr_match[2];
            else if (attr_match[3].matched) value = attr_match[3];
            else if (attr_match[4].matched) value = attr_match[4];
            std::cout << "  " << name << ": " << value << '\n';
            ait = attr_match.suffix().first;
        }
        std::cout << '\n';
        it = tag_match.suffix().first;
    }
    return 0;
}
