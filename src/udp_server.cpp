#include "udp_server.hpp"
#include <iostream>
#include <random>
#include <boost/bind/bind.hpp>
using namespace boost::placeholders;

UdpServer::UdpServer(boost::asio::io_context& io_context, unsigned short port)
    : socket_(io_context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)) {
    doReceive();
}

void UdpServer::start() {
    std::cout << "Server started and listening on port: " << socket_.local_endpoint().port() << std::endl;
}

void UdpServer::doReceive() {
    socket_.async_receive_from(
        boost::asio::buffer(recv_buffer_), remote_endpoint_,
        [this](boost::system::error_code ec, std::size_t bytes_recvd) {
            if (!ec && bytes_recvd > 0) {
                int X = std::stoi(std::string(recv_buffer_.data(), bytes_recvd));
                auto data = generateDataArray(X);
                doSend(data);
            }
            doReceive();
        });
}

void UdpServer::doSend(const std::vector<double>& data) {
    socket_.async_send_to(
        boost::asio::buffer(data), remote_endpoint_,
        [this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/) {
            // После отправки можно добавить логику, если нужно
        });
}

double UdpServer::generateRandomDouble(double min, double max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

std::vector<double> UdpServer::generateDataArray(int X) {
    std::vector<double> data(1'000'000);
    for (auto& d : data) {
        d = generateRandomDouble(-X, X);
    }
    return data;
}
