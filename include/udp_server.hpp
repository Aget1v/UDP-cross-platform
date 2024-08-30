#ifndef UDP_SERVER_HPP
#define UDP_SERVER_HPP

#include <boost/asio.hpp>
#include <vector>
#include <string>
#include <array>

class UdpServer {
public:
    UdpServer(boost::asio::io_context& io_context, unsigned short port);
    void start();

private:
    void doReceive();
    void processClient(const std::string& received_message);
    void doSend(const std::vector<double>& data);
    double generateRandomDouble(double min, double max);
    std::vector<double> generateDataArray(int X);

    boost::asio::io_context& io_context_;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint remote_endpoint_;
    std::array<char, 1024> recv_buffer_;
};

#endif // UDP_SERVER_HPP
