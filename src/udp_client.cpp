#include "udp_client.hpp"
#include <iostream>
#include <algorithm>
#include <thread>
#include <sstream>
#include <fstream>
#include <chrono>

// Инициализация статических членов класса
std::mutex UdpClient::coutMutex;
std::mutex UdpClient::dataMutex;
std::vector<std::string> UdpClient::receivedData;
size_t UdpClient::totalBytesReceived = 0;

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

        // Определяем оптимальное количество потоков
        int numThreads = determineOptimalThreads();
        std::vector<std::thread> threads;

        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back(&UdpClient::receiveData, this);
        }

        // Ждем завершения всех потоков
        for (auto& t : threads) {
            t.join();
        }

        // Сортируем полученные данные от большего к меньшему
        std::lock_guard<std::mutex> lock(dataMutex);
        std::sort(receivedData.begin(), receivedData.end(), std::greater<std::string>());

        // Записываем отсортированные данные в текстовый файл
        std::ofstream outFile("sorted_data.txt");
        for (const auto& data : receivedData) {
            outFile << data << std::endl;
        }
        outFile.close();

    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void UdpClient::receiveData() {
    const size_t max_length = 65000;
    std::vector<char> reply(max_length);

    while (true) {
        boost::system::error_code error;
        size_t len = socket_.receive_from(boost::asio::buffer(reply), server_endpoint_, 0, error);

        if (error && error != boost::asio::error::message_size) {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cerr << "Receive failed: " << error.message() << std::endl;
            break;
        }

        // Логирование полученного пакета
        std::string data(reply.data(), len);

        {
            std::lock_guard<std::mutex> lock(coutMutex);
            totalBytesReceived += len;
            std::cout << "Bytes received: " << len << " Total: " << totalBytesReceived << std::endl;
        }

        // Добавляем полученные данные в общий вектор
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            receivedData.push_back(data);
        }
    }
}

int UdpClient::determineOptimalThreads() {
    // Получаем количество логических ядер
    unsigned int numCores = std::thread::hardware_concurrency();

    if (numCores == 0) {
        numCores = 2; // По умолчанию, если количество ядер не определено
    }

    // Логируем количество ядер
    std::cout << "Number of logical cores: " << numCores << std::endl;

    // Допустим, мы добавим еще 50% потоков для обработки I/O задержек
    int optimalThreads = static_cast<int>(numCores * 1.5);

    std::cout << "Optimal number of threads: " << optimalThreads << std::endl;

    return optimalThreads;
}