#include <UdpSocket.hpp>
#include <iostream>
#include <chrono>

int64_t timestamp() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

class EchoClient {
private:
    soc::UdpSocket socket;

    void ihandle(std::vector<uint8_t>& buffer, std::size_t length, std::string ip, int port) {
        std::cout << "\n\nReceived " << length << " bytes:\n";
        for (int i = 0; i < length; i++) {
            std::cout << buffer[i];
        }
    }

public:

    EchoClient(std::string ip, int port) :
        socket(
            [this](std::vector<uint8_t>& buffer, std::size_t length, std::string ip, int port) {this->ihandle(buffer, length, ip, port);},
            ip,
            port
        )
    {}
    ~EchoClient() { stop(); }
    void start() { socket.start(); }
    void stop() { socket.stop(); }
    void send(std::string ip, int port, std::vector<uint8_t> buffer) { socket.send(ip, port, buffer); }

};



int main() {
    EchoClient client("127.0.0.1", -1);
    client.start();

    // std::string message = "Hello!";
    // std::vector<uint8_t> buffer(message.begin(), message.end());

    int64_t ts = timestamp();
    uint8_t *ptr = (uint8_t*)&ts;
    std::vector<uint8_t> buffer;
    for (int i = 0; i < sizeof(ts); i++) {
        buffer.push_back(ptr[i]);
    }
    client.send("192.168.1.153", 6969, buffer);

    // 1 second delay before client shutdown
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1000ms);

    client.stop();
    return 0;
}