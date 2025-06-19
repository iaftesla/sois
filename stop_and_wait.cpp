#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <mutex>
#include <condition_variable>

struct Packet {
    int sequence_number;
    std::string data;
};

std::mutex mtx;
std::condition_variable cv;
bool packet_ready = false;
bool ack_ready = false;
Packet channel_pkt;
int channel_ack = 0;
bool done = false; ///< set when sender finished sending all packets

std::default_random_engine rng(std::random_device{}());
/**
 * \brief Randomly decide whether a packet is lost.
 * \param p probability of loss in range [0,1].
 */
bool isLost(double p) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng) < p;
}

/**
 * \brief Sender side of Stop-and-Wait protocol.
 * \param loss_prob probability of packet/ACK loss.
 * \param num_packets total packets to send.
 * \param timeout_ms timeout for ACK in milliseconds.
 */
void sender(double loss_prob, int num_packets, int timeout_ms) {
    enum { READY, WAIT_ACK } state = READY;
    int seq = 0;
    int sent_count = 0;

    while (sent_count < num_packets) {
        std::unique_lock<std::mutex> lock(mtx);
        if (state == READY) {
            Packet pkt{seq, "DATA" + std::to_string(sent_count)};
            std::cout << "Sender: Sending packet #" << sent_count << " seq=" << seq;
            if (!isLost(loss_prob)) {
                channel_pkt = pkt;
                packet_ready = true;
                std::cout << " (delivered)";
                cv.notify_all();
            } else {
                std::cout << " (lost)";
            }
            std::cout << '\n';
            state = WAIT_ACK;

            bool ack_received = cv.wait_for(lock, std::chrono::milliseconds(timeout_ms),
                                            [] { return ack_ready; });
            if (ack_received && channel_ack == seq) {
                std::cout << "Sender: ACK received seq=" << channel_ack << '\n';
                ack_ready = false;
                seq ^= 1;
                ++sent_count;
                state = READY;
            } else {
                std::cout << "Sender: Timeout or wrong ACK, resending\n";
            }
        }
    }
    std::cout << "Sender: All packets sent\n";
    done = true;
    cv.notify_all();
}

/**
 * \brief Receiver side of Stop-and-Wait protocol.
 * \param loss_prob probability of ACK loss.
 * Exits when sender finishes and no packet is pending.
 */
void receiver(double loss_prob) {
    int expected_seq = 0;
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return packet_ready || done; });
        if (done && !packet_ready)
            break;
        Packet pkt = channel_pkt;
        packet_ready = false;
        std::cout << "Receiver: Packet received seq=" << pkt.sequence_number;
        if (pkt.sequence_number == expected_seq) {
            std::cout << " (correct)";
            if (!isLost(loss_prob)) {
                channel_ack = expected_seq;
                ack_ready = true;
                std::cout << ", ACK=" << expected_seq;
                cv.notify_all();
            } else {
                std::cout << ", ACK lost";
            }
            expected_seq ^= 1;
        } else {
            std::cout << " (duplicate)";
            int ack = 1 - expected_seq;
            if (!isLost(loss_prob)) {
                channel_ack = ack;
                ack_ready = true;
                std::cout << ", duplicate ACK=" << ack;
                cv.notify_all();
            } else {
                std::cout << ", duplicate ACK lost";
            }
        }
        std::cout << '\n';
    }
}

/** Entry point demonstrating Stop-and-Wait protocol simulation. */
int main() {
    double loss_prob;
    int num_packets, timeout_ms;
    std::cout << "Loss probability (0-1): ";
    std::cin >> loss_prob;
    std::cout << "Number of packets: ";
    std::cin >> num_packets;
    std::cout << "Timeout (ms): ";
    std::cin >> timeout_ms;

    std::thread recv_thread(receiver, loss_prob);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread send_thread(sender, loss_prob, num_packets, timeout_ms);

    send_thread.join();
    {
        std::lock_guard<std::mutex> lock(mtx);
        done = true;
        cv.notify_all();
    }
    recv_thread.join();
    return 0;
}
