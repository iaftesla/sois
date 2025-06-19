#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <set>
#include "tcp_sm.hpp"

#ifdef GENERATE_DOT
std::set<std::string> dotEdges; ///< collected edges for DOT output
#endif

/** Convert string to Event enum. */
Event strToEvent(const std::string& s) {
    if (s == "APP_PASSIVE_OPEN") return Event::APP_PASSIVE_OPEN;
    if (s == "APP_ACTIVE_OPEN")  return Event::APP_ACTIVE_OPEN;
    if (s == "APP_SEND")         return Event::APP_SEND;
    if (s == "APP_CLOSE")        return Event::APP_CLOSE;
    if (s == "RCV_SYN")          return Event::RCV_SYN;
    if (s == "RCV_ACK")          return Event::RCV_ACK;
    if (s == "RCV_SYN_ACK")      return Event::RCV_SYN_ACK;
    if (s == "RCV_FIN")          return Event::RCV_FIN;
    if (s == "RCV_FIN_ACK")      return Event::RCV_FIN_ACK;
    if (s == "TIMEOUT")          return Event::TIMEOUT;
    if (s == "RST")              return Event::RST;
    return Event::UNKNOWN;
}

/** Convert TCPState to string. */
std::string stateToStr(TCPState s) {
    switch (s) {
        case TCPState::CLOSED: return "CLOSED";
        case TCPState::LISTEN: return "LISTEN";
        case TCPState::SYN_SENT: return "SYN_SENT";
        case TCPState::SYN_RECEIVED: return "SYN_RECEIVED";
        case TCPState::ESTABLISHED: return "ESTABLISHED";
        case TCPState::FIN_WAIT_1: return "FIN_WAIT_1";
        case TCPState::FIN_WAIT_2: return "FIN_WAIT_2";
        case TCPState::CLOSING: return "CLOSING";
        case TCPState::CLOSE_WAIT: return "CLOSE_WAIT";
        case TCPState::LAST_ACK: return "LAST_ACK";
        case TCPState::TIME_WAIT: return "TIME_WAIT";
    }
    return "UNKNOWN";
}

/** Convert Event enum to string. */
std::string eventToStr(Event e) {
    switch (e) {
        case Event::APP_PASSIVE_OPEN: return "APP_PASSIVE_OPEN";
        case Event::APP_ACTIVE_OPEN: return "APP_ACTIVE_OPEN";
        case Event::APP_SEND: return "APP_SEND";
        case Event::APP_CLOSE: return "APP_CLOSE";
        case Event::RCV_SYN: return "RCV_SYN";
        case Event::RCV_ACK: return "RCV_ACK";
        case Event::RCV_SYN_ACK: return "RCV_SYN_ACK";
        case Event::RCV_FIN: return "RCV_FIN";
        case Event::RCV_FIN_ACK: return "RCV_FIN_ACK";
        case Event::TIMEOUT: return "TIMEOUT";
        case Event::RST: return "RST";
        default: return "UNKNOWN";
    }
}

TCPConnection::TCPConnection() : state(TCPState::CLOSED) {}

/** Handle incoming event and update state. */
void TCPConnection::onEvent(Event e) {
        std::lock_guard<std::mutex> lock(mtx);
        TCPState prev = state;

        switch (state) {
            case TCPState::CLOSED:
                if (e == Event::APP_PASSIVE_OPEN) state = TCPState::LISTEN;
                else if (e == Event::APP_ACTIVE_OPEN) state = TCPState::SYN_SENT;
                break;

            case TCPState::LISTEN:
                if (e == Event::RCV_SYN) state = TCPState::SYN_RECEIVED;
                else if (e == Event::APP_SEND) state = TCPState::SYN_SENT;
                else if (e == Event::APP_CLOSE) state = TCPState::CLOSED;
                break;

            case TCPState::SYN_SENT:
                if (e == Event::RCV_SYN) state = TCPState::SYN_RECEIVED;
                else if (e == Event::RCV_SYN_ACK) state = TCPState::ESTABLISHED;
                else if (e == Event::TIMEOUT) state = TCPState::CLOSED;
                break;

            case TCPState::SYN_RECEIVED:
                if (e == Event::APP_CLOSE) state = TCPState::FIN_WAIT_1;
                else if (e == Event::RCV_ACK) state = TCPState::ESTABLISHED;
                break;

            case TCPState::ESTABLISHED:
                if (e == Event::APP_CLOSE) state = TCPState::FIN_WAIT_1;
                else if (e == Event::RCV_FIN) state = TCPState::CLOSE_WAIT;
                break;

            case TCPState::FIN_WAIT_1:
                if (e == Event::RCV_FIN) state = TCPState::CLOSING;
                else if (e == Event::RCV_FIN_ACK) state = TCPState::TIME_WAIT;
                else if (e == Event::RCV_ACK) state = TCPState::FIN_WAIT_2;
                break;

            case TCPState::FIN_WAIT_2:
                if (e == Event::RCV_FIN) state = TCPState::TIME_WAIT;
                break;

            case TCPState::CLOSING:
                if (e == Event::RCV_ACK) state = TCPState::TIME_WAIT;
                break;

            case TCPState::CLOSE_WAIT:
                if (e == Event::APP_CLOSE) state = TCPState::LAST_ACK;
                break;

            case TCPState::LAST_ACK:
                if (e == Event::RCV_ACK) state = TCPState::CLOSED;
                break;

            case TCPState::TIME_WAIT:
                if (e == Event::TIMEOUT) state = TCPState::CLOSED;
                break;
        }

        if (prev != state) {
            std::cout << "Transition: " << stateToStr(prev) << " -> " << stateToStr(state) << '\n';
#ifdef GENERATE_DOT
            dotEdges.insert("\"" + stateToStr(prev) + "\" -> \"" + stateToStr(state) + "\"");
#endif
            if (state == TCPState::TIME_WAIT)
                startTimeWaitTimer();
        } else {
            std::cout << "No transition from " << stateToStr(state) << " on " << eventToStr(e) << '\n';
        }
}

/** Current FSM state. */
TCPState TCPConnection::getState() const { return state; }

void TCPConnection::startTimeWaitTimer() {
    std::thread([this]() {
        std::this_thread::sleep_for(std::chrono::seconds(30));
        this->onEvent(Event::TIMEOUT);
    }).detach();
}

