#include "udp_server.hpp"
#include <iostream>
#include <random>
#include <thread>
#include <boost/asio.hpp> // Обязательно для использования Boost.Asio

UdpServer::UdpServer(boost::asio::io_context& io_context, unsigned short port)
    : io_context_(io_context), socket_(io_context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)) {
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
                std::string received_message(recv_buffer_.data(), bytes_recvd);
                
                // Вывод IP и порта клиента
                std::cout << "Received request from client: " << received_message << std::endl;
                std::cout << "Client IP: " << remote_endpoint_.address().to_string() 
                          << " Port: " << remote_endpoint_.port() << std::endl;

                // Запуск обработки клиента в отдельном потоке
                std::thread([this, received_message]() {
                    processClient(received_message);
                }).detach();
            } else {
                std::cerr << "Error receiving data: " << ec.message() << std::endl;
            }
            doReceive();
        });
}

void UdpServer::processClient(const std::string& received_message) {
    int X = std::stoi(received_message);
    auto data = generateDataArray(X);

    std::cout << "Generated data array with values in range: [-" << X << ", " << X << "]" << std::endl;

    doSend(data);
}

void UdpServer::doSend(const std::vector<double>& data) {
    const char* data_ptr = reinterpret_cast<const char*>(data.data());
    std::size_t total_size = data.size() * sizeof(double);
    std::size_t max_packet_size = 64000; // Уменьшено до безопасного размера для UDP пакетов

    std::size_t bytes_sent = 0;

    while (bytes_sent < total_size) {
        std::size_t chunk_size = std::min(max_packet_size, total_size - bytes_sent);

        // Создание буфера для передачи данных
        auto buffer = boost::asio::buffer(data_ptr + bytes_sent, chunk_size);

        socket_.async_send_to(
            buffer, remote_endpoint_,
            [this, chunk_size, &bytes_sent, total_size](boost::system::error_code ec, std::size_t /*sent*/) {
                if (!ec) {
                    bytes_sent += chunk_size;
                    std::cout << "Sent " << bytes_sent << " bytes of " << total_size << std::endl;
                } else {
                    std::cerr << "Error sending data: " << ec.message() << std::endl;
                }
            });

        // Подождем завершения отправки, чтобы не отправлять данные слишком быстро
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
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
