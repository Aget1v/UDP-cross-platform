#ifndef UDP_CLIENT_HPP
#define UDP_CLIENT_HPP

#include <boost/asio.hpp>
#include <vector>

class UdpClient {
public:
    UdpClient(boost::asio::io_context& io_context, const std::string& host, unsigned short port);
    void sendRequest(double X);
    void receiveResponse();

private:
    void doReceive();
    void processData(const std::vector<double>& data);

    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint server_endpoint_;
    std::array<char, 1024> recv_buffer_;
};

#endif // UDP_CLIENT_HPP
