#ifndef UDP_SERVER_HPP
#define UDP_SERVER_HPP

#include <boost/asio.hpp>
#include <vector>

class UdpServer {
public:
    UdpServer(boost::asio::io_context& io_context, unsigned short port);
    void start();

private:
    void doReceive();
    void doSend(const std::vector<double>& data);

    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint remote_endpoint_;
    std::array<char, 1024> recv_buffer_;
    double generateRandomDouble(double min, double max);
    std::vector<double> generateDataArray(int X);
};

#endif // UDP_SERVER_HPP
