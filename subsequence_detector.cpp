#include <iostream>
#include <vector>
#include <string>
#include "kmp.hpp"

/** CLI tool showcasing KMP subsequence detection. */
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <pattern> <text>\n";
        return 1;
    }

    const std::string pattern = argv[1];
    const std::string text = argv[2];
    std::vector<bool> output = kmp::detect(pattern, text);

    for (bool b : output)
        std::cout << (b ? "true" : "false") << ' ';
    std::cout << '\n';
    return 0;
}
