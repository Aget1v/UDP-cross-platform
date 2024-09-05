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
    void processClient(double received_value, const std::string& client_id);
    void doSend(const std::vector<double>& data, const std::string& client_id); 
    double generateRandomDouble(double min, double max);
    std::vector<double> generateDataArray(double X);

    boost::asio::io_context& io_context_;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint remote_endpoint_;
    std::array<char, 1024> recv_buffer_;

    std::unordered_map<std::string, boost::asio::ip::udp::endpoint> client_endpoints_;
};

#endif // UDP_SERVER_HPP
