#include "tcp_sm.hpp"
#include <iostream>
#include <set>

/** Entry point for interactive TCP FSM demo. */
int main() {
    TCPConnection conn;
    std::cout << "Enter events, type EXIT to quit:\n";
    std::string input;
    while (std::getline(std::cin, input) && input != "EXIT") {
        conn.onEvent(strToEvent(input));
        std::cout << "Current state: " << stateToStr(conn.getState()) << '\n';
    }
#ifdef GENERATE_DOT
    std::cout << "digraph TCP {\n";
    extern std::set<std::string> dotEdges; // from lib
    for (const auto& e : dotEdges) {
        std::cout << "  " << e << ";\n";
    }
    std::cout << "}\n";
#endif
    return 0;
}
