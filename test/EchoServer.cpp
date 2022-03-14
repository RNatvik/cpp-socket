#include <UdpSocket.hpp>
#include <iostream>
#include <chrono>

int64_t timestamp() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

class EchoServer {
private:
    soc::UdpSocket socket;

    void ihandle(std::vector<uint8_t>& buffer, std::size_t length, std::string ip, int port) {
        int64_t ts = timestamp();
        int64_t incommingTs;
        uint8_t* ptr = (uint8_t*)&incommingTs;
        for (int i = 0; i < sizeof(ts); i++) {
            ptr[i] = buffer[i];
        }
        std::cout << "\n\nReceived " << length << " bytes: (" << ip << ":" << port << ")" << std::endl;
        std::cout << "Now: " << ts << " Prev: " << incommingTs << std::endl,
        std::cout << "diff: " << ts - incommingTs << std::endl;
        
        // for (int i = 0; i < length; i++) {
        //     std::cout << buffer[i];
        // }
        // std::vector<uint8_t> tx(buffer.begin(), buffer.begin() + length);
        ptr = (uint8_t*)&ts;
        std::vector<uint8_t> tx;
        for (int i = 0; i < sizeof(ts); i++) {
            tx.push_back(ptr[i]);
        }
        socket.send(ip, port, tx);
    }

public:

    EchoServer(std::string ip, int port) :
        socket(
            [this](std::vector<uint8_t>& buffer, std::size_t length, std::string ip, int port) {this->ihandle(buffer, length, ip, port);},
            ip,
            port
        )
    {}
    ~EchoServer() { stop(); }
    void start() { socket.start(); }
    void stop() { socket.stop(); }

};



int main() {
    EchoServer server("192.168.1.153", 6969);
    server.start();
    // 2 min delay before server shutdown
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(20000ms);

    server.stop();
    return 0;
}