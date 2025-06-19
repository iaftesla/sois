#include "regex_to_nfa.hpp"
#include <iostream>

using namespace regex_nfa;

int main() {
    std::string re;
    std::cout << "Enter regex: ";
    std::getline(std::cin, re);

    Fragment nfa = buildFromRegex(re);
    printNFA(nfa.start, std::cout);
    return 0;
}
