#ifndef TCP_SM_HPP
#define TCP_SM_HPP

#include <mutex>
#include <string>

enum class TCPState {
    CLOSED, LISTEN, SYN_SENT, SYN_RECEIVED, ESTABLISHED,
    FIN_WAIT_1, FIN_WAIT_2, CLOSING,
    CLOSE_WAIT, LAST_ACK, TIME_WAIT
};

enum class Event {
    APP_PASSIVE_OPEN, APP_ACTIVE_OPEN, APP_SEND, APP_CLOSE,
    RCV_SYN, RCV_ACK, RCV_SYN_ACK, RCV_FIN, RCV_FIN_ACK,
    TIMEOUT, RST, UNKNOWN
};

Event strToEvent(const std::string& s);
std::string stateToStr(TCPState s);
std::string eventToStr(Event e);

/** Simple TCP finite state machine. */
class TCPConnection {
public:
    TCPConnection();
    void onEvent(Event e);
    /** Current FSM state. */
    TCPState getState() const;

private:
    TCPState state;
    mutable std::mutex mtx;
    void startTimeWaitTimer();
};

#endif
