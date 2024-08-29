#include "udp_client.hpp"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <boost/bind/bind.hpp>
using namespace boost::placeholders;

UdpClient::UdpClient(boost::asio::io_context& io_context, const std::string& host, unsigned short port)
    : socket_(io_context) {
    boost::asio::ip::udp::resolver resolver(io_context);
    server_endpoint_ = *resolver.resolve(boost::asio::ip::udp::v4(), host, std::to_string(port)).begin();
    socket_.open(boost::asio::ip::udp::v4());
}

void UdpClient::sendRequest(double value) {
    try {
        std::ostringstream request_stream;
        request_stream << value;
        std::string request = request_stream.str();

        // Отправляем запрос на сервер
        socket_.send_to(boost::asio::buffer(request), server_endpoint_);
        std::cout << "Request sent: " << request << std::endl;

        // Ожидаем и получаем данные от сервера
        const size_t max_length = 65000;
        std::vector<char> reply(max_length);
        boost::asio::ip::udp::endpoint sender_endpoint;
        size_t total_bytes_received = 0;

        // Получаем данные до тех пор, пока сервер отправляет их
        while (true) {
            size_t reply_length = socket_.receive_from(boost::asio::buffer(reply), sender_endpoint);
            total_bytes_received += reply_length;
            std::cout << "Received " << reply_length << " bytes, total: " << total_bytes_received << " bytes" << std::endl;
            if (reply_length < max_length) {
                // Предполагаем, что получение завершено
                break;
            }
        }

        std::cout << "Total bytes received: " << total_bytes_received << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Send request exception: " << e.what() << std::endl;
    }
}

void UdpClient::doReceive() {
    try {
        socket_.async_receive_from(
            boost::asio::buffer(recv_buffer_), server_endpoint_,
            [this](boost::system::error_code ec, std::size_t bytes_recvd) {
                if (!ec && bytes_recvd > 0) {
                    // Преобразуем полученные данные в массив double
                    std::vector<double> data(bytes_recvd / sizeof(double));
                    std::memcpy(data.data(), recv_buffer_.data(), bytes_recvd);
                    processData(data);
                } else if (ec) {
                    std::cerr << "Receive error: " << ec.message() << std::endl;
                }
            });
    } catch (const std::exception& e) {
        std::cerr << "Exception in doReceive: " << e.what() << std::endl;
    }
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