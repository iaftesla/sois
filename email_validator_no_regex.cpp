#include <iostream>
#include <string>
#include <cctype>

/** Validate e-mail address without regex. */
bool isValidEmail(const std::string& email) {
    auto atPos = email.find('@');
    if (atPos == std::string::npos || atPos == 0 || atPos + 1 >= email.size())
        return false;

    const std::string local = email.substr(0, atPos);
    const std::string domain = email.substr(atPos + 1);

    for (char c : local) {
        if (!std::isalnum(static_cast<unsigned char>(c)) &&
            c != '.' && c != '_' && c != '%' && c != '+' && c != '-')
            return false;
    }

    auto dotPos = domain.rfind('.');
    if (dotPos == std::string::npos || dotPos == 0 || dotPos + 1 >= domain.size())
        return false;

    for (char c : domain) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '-' && c != '.')
            return false;
    }

    return true;
}

/** Command line tool validating e-mails without regex. */
int main(int argc, char* argv[]) {
    std::string email;
    if (argc > 1)
        email = argv[1];
    else
        std::getline(std::cin, email);

    std::cout << (isValidEmail(email) ? "Valid email address"
                                      : "Invalid email address")
              << std::endl;
    return 0;
}
