#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "kmp.hpp"
#include "csv_parser.hpp"
#include "tcp_sm.hpp"
#include <sstream>

TEST_CASE("KMP detection") {
    std::string text = "ababcabcababc";
    std::string pat = "abc";
    auto result = kmp::detect(pat, text);
    std::vector<size_t> positions;
    for(size_t i=0;i<result.size();++i) if(result[i]) positions.push_back(i);
    REQUIRE(positions == std::vector<size_t>{4,7,12});
}

TEST_CASE("CSV parser") {
    std::stringstream ss("a,b,\"c,d\"\n1,2,3\n");
    auto table = csv::parseCSV(ss);
    REQUIRE(table.size() == 2);
    REQUIRE(table[0][2] == "c,d");
    REQUIRE(table[1][1] == "2");
}

TEST_CASE("TCP FSM transitions") {
    TCPConnection conn;
    conn.onEvent(Event::APP_ACTIVE_OPEN);
    REQUIRE(conn.getState() == TCPState::SYN_SENT);
    conn.onEvent(Event::RCV_SYN_ACK);
    REQUIRE(conn.getState() == TCPState::ESTABLISHED);
    conn.onEvent(Event::APP_CLOSE);
    REQUIRE(conn.getState() == TCPState::FIN_WAIT_1);
}
