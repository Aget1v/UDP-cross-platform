#ifndef UDP_CLIENT_HPP
#define UDP_CLIENT_HPP

#include <boost/asio.hpp>
#include <vector>
#include <mutex>
#include <string>

class UdpClient {
public:
    UdpClient(boost::asio::io_context& io_context, const std::string& host, unsigned short port);
    void sendRequest(double value);
    void receiveData();
    static int determineOptimalThreads(); // Функция для определения оптимального количества потоков

private:
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint server_endpoint_;

    // Мьютексы и вектор для работы с многопоточностью
    static std::mutex coutMutex;
    static std::mutex dataMutex;
    static std::vector<double> receivedData;
    static size_t totalBytesReceived; // Общее количество полученных байт
};

#endif // UDP_CLIENT_HPP
