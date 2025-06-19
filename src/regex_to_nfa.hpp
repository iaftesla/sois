#ifndef REGEX_TO_NFA_HPP
#define REGEX_TO_NFA_HPP

#include <string>
#include <vector>
#include <set>
#include <iosfwd>

namespace regex_nfa {

struct State {
    int id;
    char c;        ///< transition symbol, 0 for epsilon
    State* out1{}; ///< first transition
    State* out2{}; ///< second transition
};

struct Fragment {
    State* start{};                ///< start state of fragment
    std::vector<State**> outs;     ///< dangling outgoing transitions
};

/** Build an NFA from a regular expression. */
Fragment buildFromRegex(const std::string& re);

/** Print the NFA starting from state \a s to output stream \a os. */
void printNFA(State* s, std::ostream& os);

} // namespace regex_nfa

#endif // REGEX_TO_NFA_HPP
