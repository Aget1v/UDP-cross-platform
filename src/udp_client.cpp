#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <algorithm>
#include <thread>
#include <mutex>
#include <boost/asio.hpp>

class UdpClient {
public:
    UdpClient(boost::asio::io_context& io_context, const std::string& host, unsigned short port);
    void sendRequest(double value);

private:
    void receiveData();
    int determineOptimalThreads();

    static std::mutex coutMutex;
    static std::mutex dataMutex;
    static std::vector<double> receivedData;
    static size_t totalBytesReceived;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint server_endpoint_;
    unsigned short port_;
};

std::mutex UdpClient::coutMutex;
std::mutex UdpClient::dataMutex;
std::vector<double> UdpClient::receivedData;
size_t UdpClient::totalBytesReceived = 0;

UdpClient::UdpClient(boost::asio::io_context& io_context, const std::string& host, unsigned short port)
    : socket_(io_context), port_(port) {
    boost::asio::ip::udp::resolver resolver(io_context);
    server_endpoint_ = *resolver.resolve(boost::asio::ip::udp::v4(), host, std::to_string(port)).begin();
    socket_.open(boost::asio::ip::udp::v4());
}

void UdpClient::sendRequest(double value) {
    try {
        // Отправляем значение в бинарном формате
        boost::asio::streambuf buf;
        std::ostream os(&buf);
        os.write(reinterpret_cast<const char*>(&value), sizeof(value));
        socket_.send_to(buf.data(), server_endpoint_);
        std::cout << "Request sent: " << value << std::endl;

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
        std::sort(receivedData.begin(), receivedData.end(), std::greater<double>());

        // Генерируем уникальное имя файла
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm now_tm = *std::localtime(&now_time_t);
        
        std::ostringstream fileNameStream;
        fileNameStream << "data_" 
                       << std::put_time(&now_tm, "%Y%m%d_%H%M%S") 
                       << "_" << port_ << ".bin";
        std::string fileName = fileNameStream.str();

        std::cout << "Generated file name: " << fileName << std::endl;

        // Записываем отсортированные данные в бинарный файл
        std::ofstream outFile(fileName, std::ios::binary);
        if (outFile) {
            if (receivedData.empty()) {
                std::cerr << "No data to write to file." << std::endl;
            } else {
                outFile.write(reinterpret_cast<const char*>(receivedData.data()), receivedData.size() * sizeof(double));
                outFile.close();
                std::cout << "Data successfully written to file: " << fileName << std::endl;
            }
        } else {
            std::cerr << "Error opening file for writing: " << fileName << std::endl;
        }

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
        std::vector<double> data(len / sizeof(double));
        std::memcpy(data.data(), reply.data(), len);

        {
            std::lock_guard<std::mutex> lock(coutMutex);
            totalBytesReceived += len;
            std::cout << "Bytes received: " << len << " Total: " << totalBytesReceived << std::endl;
        }

        // Добавляем полученные данные в общий вектор
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            receivedData.insert(receivedData.end(), data.begin(), data.end());
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
