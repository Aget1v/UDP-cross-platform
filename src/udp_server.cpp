#include "udp_server.hpp"
#include <iostream>
#include <random>
#include <thread>
#include <cstring> // для std::memcpy

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
                // Обработка полученных данных как double
                double received_value;
                std::memcpy(&received_value, recv_buffer_.data(), sizeof(received_value));
                std::string client_id = remote_endpoint_.address().to_string() + ":" + std::to_string(remote_endpoint_.port());
                
                // Сохраняем адрес клиента в мапе
                client_endpoints_[client_id] = remote_endpoint_;

                // Вывод IP и порта клиента
                std::cout << "Received request from client: " << received_value << std::endl;
                std::cout << "Client IP: " << remote_endpoint_.address().to_string() 
                          << " Port: " << remote_endpoint_.port() << std::endl;

                // Запуск обработки клиента в отдельном потоке
                std::thread([this, received_value, client_id]() {
                    processClient(received_value, client_id);
                }).detach();
            } else {
                std::cerr << "Error receiving data: " << ec.message() << std::endl;
            }
            doReceive();
        });
}

void UdpServer::processClient(double received_value, const std::string& client_id) {
    auto data = generateDataArray(received_value);

    std::cout << "Generated data array with values in range: [-" << received_value << ", " << received_value << "]" << std::endl;

    doSend(data, client_id);
}

void UdpServer::doSend(const std::vector<double>& data, const std::string& client_id) {
    const char* data_ptr = reinterpret_cast<const char*>(data.data());
    std::size_t total_size = data.size() * sizeof(double);
    std::size_t max_packet_size = 64000;

    std::size_t bytes_sent = 0;

    // Получаем endpoint клиента из мапы
    if (client_endpoints_.find(client_id) == client_endpoints_.end()) {
        std::cerr << "Client ID not found!" << std::endl;
        return;
    }
    auto client_endpoint = client_endpoints_[client_id];

    while (bytes_sent < total_size) {
        std::size_t chunk_size = std::min(max_packet_size, total_size - bytes_sent);
        auto buffer = boost::asio::buffer(data_ptr + bytes_sent, chunk_size);

        socket_.async_send_to(
            buffer, client_endpoint,
            [this, chunk_size, &bytes_sent, total_size](boost::system::error_code ec, std::size_t /*sent*/) {
                if (!ec) {
                    bytes_sent += chunk_size;
                    std::cout << "Sent " << bytes_sent << " bytes of " << total_size << std::endl;
                } else {
                    std::cerr << "Error sending data: " << ec.message() << std::endl;
                }
            });

        // Используйте реальную синхронизацию или таймер вместо `std::this_thread::sleep_for` в асинхронном коде
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

double UdpServer::generateRandomDouble(double min, double max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

std::vector<double> UdpServer::generateDataArray(double X) {
    std::vector<double> data(1'000'000);
    for (auto& d : data) {
        d = generateRandomDouble(-X, X);
    }
    return data;
}
