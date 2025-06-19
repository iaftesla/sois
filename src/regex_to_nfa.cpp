#include "regex_to_nfa.hpp"
#include <stack>
#include <iostream>

namespace regex_nfa {

namespace {
int stateCount = 0;

State* newState(char c) {
    return new State{stateCount++, c, nullptr, nullptr};
}

void patch(const std::vector<State**>& outs, State* target) {
    for (auto ptr : outs)
        *ptr = target;
}

std::vector<State**> join(std::vector<State**> a, const std::vector<State**>& b) {
    a.insert(a.end(), b.begin(), b.end());
    return a;
}
} // namespace

Fragment buildFromRegex(const std::string& re) {
    std::stack<Fragment> st;
    std::stack<char> ops;

    auto precedence = [](char op) {
        switch (op) {
            case '*': return 3;
            case '.': return 2;
            case '|': return 1;
            default:  return 0;
        }
    };

    auto applyOp = [&](char op) {
        if (op == '*') {
            auto e = st.top(); st.pop();
            State* s = newState(0);
            s->out1 = e.start;
            auto outs = e.outs;
            outs.push_back(&s->out2);
            st.push({s, outs});
        } else if (op == '.') {
            auto e2 = st.top(); st.pop();
            auto e1 = st.top(); st.pop();
            patch(e1.outs, e2.start);
            st.push({e1.start, e2.outs});
        } else if (op == '|') {
            auto e2 = st.top(); st.pop();
            auto e1 = st.top(); st.pop();
            State* s = newState(0);
            s->out1 = e1.start;
            s->out2 = e2.start;
            auto outs = join(e1.outs, e2.outs);
            st.push({s, outs});
        }
    };

    const std::string symbols =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    // insert explicit concatenation
    std::string exp;
    for (size_t i = 0; i < re.size(); ++i) {
        char c = re[i];
        exp += c;
        if (i + 1 < re.size()) {
            char d = re[i + 1];
            bool left = symbols.find(c) != std::string::npos || c == ')' || c == '*';
            bool right = symbols.find(d) != std::string::npos || d == '(';
            if (left && right) exp += '.';
        }
    }

    for (char c : exp) {
        if (symbols.find(c) != std::string::npos) {
            State* s = newState(c);
            st.push({s, {&s->out1}});
        } else if (c == '(') {
            ops.push(c);
        } else if (c == ')') {
            while (!ops.empty() && ops.top() != '(') {
                applyOp(ops.top());
                ops.pop();
            }
            if (!ops.empty()) ops.pop(); // pop '('
        } else { // operator
            while (!ops.empty() && precedence(ops.top()) >= precedence(c)) {
                applyOp(ops.top());
                ops.pop();
            }
            ops.push(c);
        }
    }
    while (!ops.empty()) {
        applyOp(ops.top());
        ops.pop();
    }
    return st.top();
}

void printNFA(State* s, std::ostream& os) {
    std::set<int> visited;
    std::stack<State*> st;
    st.push(s);
    while (!st.empty()) {
        State* cur = st.top();
        st.pop();
        if (!cur || visited.count(cur->id)) continue;
        visited.insert(cur->id);
        os << "State " << cur->id << " --" << (cur->c ? cur->c : 'e') << "--> ";
        if (cur->out1) os << cur->out1->id; else os << "-";
        os << ", ";
        if (cur->out2) os << cur->out2->id; else os << "-";
        os << '\n';
        st.push(cur->out1);
        st.push(cur->out2);
    }
}

} // namespace regex_nfa
