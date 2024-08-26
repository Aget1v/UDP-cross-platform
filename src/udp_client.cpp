#include "udp_client.hpp"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <boost/bind.hpp>

UdpClient::UdpClient(boost::asio::io_context& io_context, const std::string& host, unsigned short port)
    : socket_(io_context) {
    boost::asio::ip::udp::resolver resolver(io_context);
    server_endpoint_ = *resolver.resolve(boost::asio::ip::udp::v4(), host, std::to_string(port)).begin();
    socket_.open(boost::asio::ip::udp::v4());
}

void UdpClient::sendRequest(double X) {
    std::string request = std::to_string(X);
    socket_.send_to(boost::asio::buffer(request), server_endpoint_);
    doReceive();
}

void UdpClient::doReceive() {
    socket_.async_receive_from(
        boost::asio::buffer(recv_buffer_), server_endpoint_,
        [this](boost::system::error_code ec, std::size_t bytes_recvd) {
            if (!ec && bytes_recvd > 0) {
                // Преобразуем полученные данные в массив double
                std::vector<double> data(bytes_recvd / sizeof(double));
                std::memcpy(data.data(), recv_buffer_.data(), bytes_recvd);
                processData(data);
            } else {
                std::cerr << "Receive error: " << ec.message() << std::endl;
            }
        });
}

void UdpClient::processData(const std::vector<double>& data) {
    std::vector<double> sorted_data = data;
    std::sort(sorted_data.begin(), sorted_data.end(), std::greater<double>());

    // Запись данных в бинарный файл
    std::ofstream outfile("../logs/received_data.bin", std::ios::binary);
    outfile.write(reinterpret_cast<const char*>(sorted_data.data()), sorted_data.size() * sizeof(double));
    outfile.close();

    std::cout << "Data received and saved to ../logs/received_data.bin" << std::endl;
}
