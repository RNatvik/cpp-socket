#include <UdpSocket.hpp>

namespace soc {
    /*------------------------------------------------------------------------------
    PUBLIC
    ------------------------------------------------------------------------------*/
    UdpSocket::UdpSocket(std::function<void(std::vector<uint8_t>& buffer, std::size_t length, std::string ip, int port)> inboundHandler, std::string ip, int port)
        : socket(context), rxBuffer(5 * 1024) {
        this->inboundHandler = inboundHandler;
        this->errorHandler = [this] (std::error_code ec, std::string ip, int port) {return true;};
        this->socketIp = ip;
        this->socketPort = port;
    }

    void UdpSocket::start() {
        asio::io_context::work idleWork(context);
        this->contextThread = std::thread([this]() { context.run(); });
        this->socket.open(asio::ip::udp::v4());
        this->socket.bind(
            asio::ip::udp::endpoint(
                asio::ip::make_address(this->socketIp),
                this->socketPort
            )
        );
        this->receive();
    }

    void UdpSocket::stop() {
        this->socket.close();
        this->context.stop();
        if (this->contextThread.joinable()) this->contextThread.join();
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

    void UdpSocket::attachErrorHandler(std::function<bool(std::error_code ec, std::string ip, int port)> errorHandler) {
        this->errorHandler = errorHandler;
    }

    std::string UdpSocket::getIP() { return this->socketIp; }
    int UdpSocket::getPort() { return this->socketPort; }

    /*------------------------------------------------------------------------------
    PRIVATE
    ------------------------------------------------------------------------------*/
    void UdpSocket::receive() {
        this->socket.async_receive_from(
            asio::buffer(this->rxBuffer.data(), this->rxBuffer.size()),
            this->rxRemoteEndpoint,
            [this](std::error_code ec, std::size_t length) {
                std::string ip = this->rxRemoteEndpoint.address().to_string();
                int port = this->rxRemoteEndpoint.port();
                bool runReceive = true;
                if (!ec) {
                    this->inboundHandler(rxBuffer, length, ip, port);
                }
                else {
                    runReceive = this->errorHandler(ec, ip, port);
                }
                receive();
            }
        );
    }

    void UdpSocket::transmit() {
        this->socket.async_send_to(
            asio::buffer(this->txQueue.front().bytes.data(), this->txQueue.front().bytes.size()),
            this->txQueue.front().endpoint,
            [this](std::error_code ec, std::size_t length) {
                if (!ec) {
                    this->txQueue.pop_front();
                    if (!this->txQueue.empty()) {
                        this->transmit();
                    }
                }
            }
        );
    }


}