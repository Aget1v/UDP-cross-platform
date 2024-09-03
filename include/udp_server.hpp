#ifndef UDP_SERVER_HPP
#define UDP_SERVER_HPP

#include <boost/asio.hpp>
#include <vector>
#include <string>
#include <unordered_map>

class UdpServer {
public:
    UdpServer(boost::asio::io_context& io_context, unsigned short port);
    void start();

private:
    void doReceive();
    void processClient(const std::string& received_message, const std::string& client_id); // Обновлено
    void doSend(const std::vector<double>& data, const std::string& client_id); // Обновлено
    double generateRandomDouble(double min, double max);
    std::vector<double> generateDataArray(double X);

    boost::asio::io_context& io_context_;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint remote_endpoint_;
    std::array<char, 1024> recv_buffer_;

    // Добавляем мапу для хранения endpoint клиентов
    std::unordered_map<std::string, boost::asio::ip::udp::endpoint> client_endpoints_;
};

#endif // UDP_SERVER_HPP
