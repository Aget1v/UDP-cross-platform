#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include "udp_server.hpp"
#include "config.hpp"
using namespace boost::placeholders;

int main() {
    try {
        // confing file load
        Config config("../config/config.ini");

        // config parameters receiving
        unsigned short server_port = static_cast<unsigned short>(config.getInt("Server.port", 8080));
        std::string server_address = config.getString("Server.address", "127.0.0.1");

        // initializing the network context
        boost::asio::io_context io_context;

        // server start
        UdpServer server(io_context, server_port);
        server.start();

        // io_context to handle asynchronous operations
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
