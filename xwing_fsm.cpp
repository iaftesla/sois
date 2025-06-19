#include <iostream>
#include <string>
#include <mutex>
#include <fstream>
#include <vector>
#include "json.hpp"
using json = nlohmann::json;

enum class State {
    IDLE, TARGET_ACQUIRE, TRACKING, AIMING,
    FIRING, EVASIVE_MANEUVERS, ENERGY_MANAGEMENT
};

enum class Event {
    TARGET_DETECTED, TARGET_LOCKED, TARGET_LOST,
    TARGET_RANGE_CLOSE, TARGET_RANGE_MEDIUM, TARGET_RANGE_FAR,
    TARGET_PRIORITY_HIGH, TARGET_PRIORITY_LOW,
    ENEMY_EVASIVE_ACTION, SHIELD_LOW, ENERGY_LOW,
    AIM_READY, FIRE_COMPLETE, EVASION_COMPLETE,
    ENERGY_ADJUSTED, NONE
};

/** Convert string to Event value. */
Event toEvent(const std::string& s) {
    if (s == "TARGET_DETECTED") return Event::TARGET_DETECTED;
    if (s == "TARGET_LOCKED") return Event::TARGET_LOCKED;
    if (s == "TARGET_LOST") return Event::TARGET_LOST;
    if (s == "TARGET_RANGE_CLOSE") return Event::TARGET_RANGE_CLOSE;
    if (s == "TARGET_RANGE_MEDIUM") return Event::TARGET_RANGE_MEDIUM;
    if (s == "TARGET_RANGE_FAR") return Event::TARGET_RANGE_FAR;
    if (s == "TARGET_PRIORITY_HIGH") return Event::TARGET_PRIORITY_HIGH;
    if (s == "TARGET_PRIORITY_LOW") return Event::TARGET_PRIORITY_LOW;
    if (s == "ENEMY_EVASIVE_ACTION") return Event::ENEMY_EVASIVE_ACTION;
    if (s == "SHIELD_LOW") return Event::SHIELD_LOW;
    if (s == "ENERGY_LOW") return Event::ENERGY_LOW;
    if (s == "AIM_READY") return Event::AIM_READY;
    if (s == "FIRE_COMPLETE") return Event::FIRE_COMPLETE;
    if (s == "EVASION_COMPLETE") return Event::EVASION_COMPLETE;
    if (s == "ENERGY_ADJUSTED") return Event::ENERGY_ADJUSTED;
    return Event::NONE;
}

/** Convert State enum to string. */
std::string stateToStr(State s) {
    switch (s) {
        case State::IDLE: return "IDLE";
        case State::TARGET_ACQUIRE: return "TARGET_ACQUIRE";
        case State::TRACKING: return "TRACKING";
        case State::AIMING: return "AIMING";
        case State::FIRING: return "FIRING";
        case State::EVASIVE_MANEUVERS: return "EVASIVE_MANEUVERS";
        case State::ENERGY_MANAGEMENT: return "ENERGY_MANAGEMENT";
    }
    return "UNKNOWN";
}

/** Convert Event enum to string. */
std::string eventToStr(Event e) {
    switch (e) {
        case Event::TARGET_DETECTED: return "TARGET_DETECTED";
        case Event::TARGET_LOCKED: return "TARGET_LOCKED";
        case Event::TARGET_LOST: return "TARGET_LOST";
        case Event::TARGET_RANGE_CLOSE: return "TARGET_RANGE_CLOSE";
        case Event::TARGET_RANGE_MEDIUM: return "TARGET_RANGE_MEDIUM";
        case Event::TARGET_RANGE_FAR: return "TARGET_RANGE_FAR";
        case Event::TARGET_PRIORITY_HIGH: return "TARGET_PRIORITY_HIGH";
        case Event::TARGET_PRIORITY_LOW: return "TARGET_PRIORITY_LOW";
        case Event::ENEMY_EVASIVE_ACTION: return "ENEMY_EVASIVE_ACTION";
        case Event::SHIELD_LOW: return "SHIELD_LOW";
        case Event::ENERGY_LOW: return "ENERGY_LOW";
        case Event::AIM_READY: return "AIM_READY";
        case Event::FIRE_COMPLETE: return "FIRE_COMPLETE";
        case Event::EVASION_COMPLETE: return "EVASION_COMPLETE";
        case Event::ENERGY_ADJUSTED: return "ENERGY_ADJUSTED";
        default: return "NONE";
    }
}

/** Finite state machine controlling an X-Wing fighter. */
class XWingFSM {
public:
    /** Process one FSM event. */
    void onEvent(Event e) {
        std::lock_guard<std::mutex> lg(mtx);
        std::cout << "Event: " << eventToStr(e) << ", State: " << stateToStr(state) << '\n';
        State prev = state;

        switch (state) {
            case State::IDLE:
                if (e == Event::TARGET_DETECTED) {
                    actionSelectTarget();
                    state = State::TARGET_ACQUIRE;
                }
                break;

            case State::TARGET_ACQUIRE:
                if (e == Event::TARGET_LOCKED) {
                    actionLockTarget();
                    state = State::TRACKING;
                } else if (e == Event::TARGET_LOST) {
                    state = State::IDLE;
                }
                break;

            case State::TRACKING:
                if (e == Event::TARGET_RANGE_FAR) {
                    actionAdjustAim();
                    state = State::AIMING;
                } else if (e == Event::TARGET_RANGE_CLOSE || e == Event::ENEMY_EVASIVE_ACTION) {
                    actionInitiateEvasion();
                    state = State::EVASIVE_MANEUVERS;
                }
                break;

            case State::AIMING:
                if (e == Event::AIM_READY) {
                    actionFireLasers();
                    state = State::FIRING;
                } else if (e == Event::TARGET_LOST) {
                    state = State::IDLE;
                }
                break;

            case State::FIRING:
                if (e == Event::FIRE_COMPLETE) {
                    state = State::TRACKING;
                } else if (e == Event::TARGET_LOST) {
                    state = State::IDLE;
                }
                break;

            case State::EVASIVE_MANEUVERS:
                if (e == Event::EVASION_COMPLETE) {
                    state = State::TRACKING;
                }
                break;

            case State::ENERGY_MANAGEMENT:
                if (e == Event::ENERGY_ADJUSTED) {
                    state = State::TRACKING;
                }
                break;
        }

        // Global transition
        if (e == Event::SHIELD_LOW || e == Event::ENERGY_LOW) {
            actionAdjustEnergy();
            state = State::ENERGY_MANAGEMENT;
        }

        if (prev != state)
            std::cout << "Transition: " << stateToStr(prev) << " -> " << stateToStr(state) << '\n';
        std::cout << "New State: " << stateToStr(state) << "\n\n";
    }

private:
    State state{State::IDLE};
    std::mutex mtx;

    void actionSelectTarget() { std::cout << "Action: SELECT_TARGET\n"; }
    void actionLockTarget()   { std::cout << "Action: LOCK_TARGET\n"; }
    void actionAdjustAim()    { std::cout << "Action: ADJUST_AIM\n"; }
    void actionFireLasers()   { std::cout << "Action: FIRE_LASERS\n"; }
    void actionInitiateEvasion() { std::cout << "Action: INITIATE_EVASIVE_MANEUVERS\n"; }
    void actionAdjustEnergy() { std::cout << "Action: ADJUST_ENERGY_DISTRIBUTION\n"; }
};

/** Entry point; optionally takes JSON file with event sequence. */
int main(int argc, char* argv[]) {
    XWingFSM fsm;
    std::vector<Event> script;
    if (argc > 1) {
        std::ifstream jsFile(argv[1]);
        if (jsFile) {
            json j; jsFile >> j;
            for (const auto& s : j) script.push_back(toEvent(s.get<std::string>()));
        }
    }

    std::cout << "Enter events (EXIT to quit):\n";
    std::string input;
    auto it = script.begin();
    while (true) {
        if (it != script.end()) {
            input = eventToStr(*it++);
            std::cout << input << '\n';
        } else if (!std::getline(std::cin, input) || input == "EXIT") {
            break;
        }
        Event e = toEvent(input);
        if (e == Event::NONE) {
            std::cout << "Unknown event: " << input << '\n';
            continue;
        }
        fsm.onEvent(e);
    }
    return 0;
}
