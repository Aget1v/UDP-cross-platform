#ifndef UDP_CLIENT_HPP
#define UDP_CLIENT_HPP

#include <boost/asio.hpp>
#include <vector>
#include <string>
#include <mutex>
#include <thread>

class UdpClient {
public:
    UdpClient(boost::asio::io_context& io_context, const std::string& host, unsigned short port);
    void sendRequest(double value);

private:
    void receiveData();
    int determineOptimalThreads();

    static std::mutex coutMutex;
    static std::mutex dataMutex;
    static std::vector<double> receivedData; // Обновлено на std::vector<double>
    static size_t totalBytesReceived;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint server_endpoint_;
    unsigned short port_; // Добавлено член данных для порта
};

#endif // UDP_CLIENT_HPP
