#ifndef SOC_SOCKET_HPP
#define SOC_SOCKET_HPP

#include <TSQueue.hpp>

#include <cstdint>
#include <vector>
#include <thread>
#include <functional>

// #ifdef _WIN32
// #define _WIN32_WINNT 0x0A00
// #endif

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

namespace soc {
    struct TxMessage {
        asio::ip::udp::endpoint endpoint;
        std::vector<uint8_t> bytes;
    };

    class UdpSocket {
    private:
        std::string socketIp;
        int socketPort;

        asio::io_context context;
        std::thread contextThread;
        asio::ip::udp::endpoint rxRemoteEndpoint;
        asio::ip::udp::endpoint txRemoteEndpoint;
        asio::ip::udp::socket socket;
        std::vector<uint8_t> rxBuffer;
        TSQueue<TxMessage> txQueue;

        std::function<void(std::vector<uint8_t>& buffer, std::size_t length, std::string ip, int port)> inboundHandler;
        void receive();
        void transmit();

    public:
        UdpSocket(std::function<void(std::vector<uint8_t>& buffer, std::size_t length, std::string ip, int port)> inboundHandler, std::string ip, int port);
        void start();
        void stop();
        void send(std::string ip, int port, std::vector<uint8_t> bytes);
    };
}

#endif