cmake_minimum_required(VERSION 3.10)
project(xwing_assignments CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -pthread)

include_directories(src)

add_library(algos STATIC
    src/regex_to_nfa.cpp
    src/csv_parser.cpp
    src/kmp.cpp
    src/tcp_sm_lib.cpp
)

add_executable(email_validator email_validator.cpp)
add_executable(phone_extractor phone_extractor.cpp)
add_executable(csv_parser csv_parser.cpp)
add_executable(html_parser html_parser.cpp)
add_executable(email_validator_no_regex email_validator_no_regex.cpp)
add_executable(subsequence_detector subsequence_detector.cpp)
add_executable(regex_to_nfa src/regex_to_nfa_main.cpp)
add_executable(stop_and_wait stop_and_wait.cpp)
add_executable(tcp_sm tcp_sm.cpp)
add_executable(xwing_fsm xwing_fsm.cpp)

target_link_libraries(csv_parser algos)

target_link_libraries(subsequence_detector algos)

target_link_libraries(regex_to_nfa algos)
target_link_libraries(tcp_sm algos)

# tests
add_executable(tests
    tests/tests.cpp
)
target_link_libraries(tests algos)

enable_testing()
add_test(NAME run_tests COMMAND tests)
