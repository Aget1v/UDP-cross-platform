#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include "udp_server.hpp"
#include "config.hpp"
using namespace boost::placeholders;

int main() {
    try {
        Config config("../config/config.ini");

        unsigned short server_port = static_cast<unsigned short>(config.getInt("Server.port", 8080));
        std::string server_address = config.getString("Server.address", "127.0.0.1");

        boost::asio::io_context io_context;

        UdpServer server(io_context, server_port);
        server.start();

        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
