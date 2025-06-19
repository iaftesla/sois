#include "kmp.hpp"

namespace kmp {

std::vector<int> buildPrefix(const std::string& pattern) {
    std::vector<int> pi(pattern.size(), 0);
    for (size_t i = 1; i < pattern.size(); ++i) {
        size_t j = pi[i-1];
        while (j > 0 && pattern[i] != pattern[j])
            j = pi[j-1];
        if (pattern[i] == pattern[j])
            ++j;
        pi[i] = j;
    }
    return pi;
}

std::vector<bool> detect(const std::string& pattern, const std::string& text) {
    std::vector<int> pi = buildPrefix(pattern);
    std::vector<bool> out(text.size(), false);
    size_t j = 0;
    for (size_t i = 0; i < text.size(); ++i) {
        while (j > 0 && text[i] != pattern[j])
            j = pi[j-1];
        if (text[i] == pattern[j])
            ++j;
        if (j == pattern.size()) {
            out[i] = true;
            j = pi[j-1];
        }
    }
    return out;
}

} // namespace kmp
