#include <iostream>
#include <regex>
#include <string>

/** Simple email validator using regex. */
int main(int argc, char* argv[]) {
    std::string email;
    if (argc > 1) {
        email = argv[1];
    } else {
        std::getline(std::cin, email);
    }
    const std::regex pattern(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
    std::cout << (std::regex_match(email, pattern) ? "Valid email address"
                                                   : "Invalid email address")
              << std::endl;
    return 0;
}
