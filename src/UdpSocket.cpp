#include <UdpSocket.hpp>

namespace soc {
    /*------------------------------------------------------------------------------
    PUBLIC
    ------------------------------------------------------------------------------*/
    UdpSocket::UdpSocket(std::function<void(std::vector<uint8_t>& buffer, std::size_t length, std::string ip, int port)> inboundHandler, std::string ip, int port)
        : socket(context), rxBuffer(5 * 1024) {
        this->inboundHandler = inboundHandler;
        this->socketIp = ip;
        this->socketPort = port;
    }

    void UdpSocket::start() {
        asio::io_context::work idleWork(context);
        contextThread = std::thread([this]() { context.run(); });
        socket.open(asio::ip::udp::v4());
        socket.bind(
            asio::ip::udp::endpoint(
                asio::ip::make_address(socketIp),
                socketPort
            )
        );
        receive();
    }

    void UdpSocket::stop() {
        socket.close();
        context.stop();
        if (contextThread.joinable()) contextThread.join();
    }

    void UdpSocket::send(std::string ip, int port, std::vector<uint8_t> bytes) {
        TxMessage msg;
        msg.bytes = bytes;
        msg.endpoint = asio::ip::udp::endpoint(asio::ip::make_address(ip), port);
        bool queueEmpty = this->txQueue.empty();
        this->txQueue.push_back(msg);
        if (queueEmpty) {
            this->transmit();
        }
    }

    std::string UdpSocket::getIP() { return this->socketIp; }
    int UdpSocket::getPort() { return this->socketPort; }

    /*------------------------------------------------------------------------------
    PRIVATE
    ------------------------------------------------------------------------------*/
    void UdpSocket::receive() {
        socket.async_receive_from(
            asio::buffer(rxBuffer.data(), rxBuffer.size()),
            rxRemoteEndpoint,
            [this](std::error_code ec, std::size_t length) {
                if (!ec) {
                    std::string ip = rxRemoteEndpoint.address().to_string();
                    int port = rxRemoteEndpoint.port();
                    inboundHandler(rxBuffer, length, ip, port);
                    receive();
                }
            }
        );
    }

    void UdpSocket::transmit() {
        socket.async_send_to(
            asio::buffer(txQueue.front().bytes.data(), txQueue.front().bytes.size()),
            txQueue.front().endpoint,
            [this](std::error_code ec, std::size_t length) {
                if (!ec) {
                    txQueue.pop_front();
                    if (!txQueue.empty()) {
                        transmit();
                    }
                }
            }
        );
    }


}