#include <iostream>
#include<random>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include "udp_client.hpp"
#include "config.hpp"

using namespace boost::placeholders;

int main() {
    std::random_device rd;
    std::mt19937 generator(rd());  // Mersenne Twister based generator
    std::uniform_real_distribution<double> distribution;
    double randomValue = distribution(generator);

    try {
        // config load
        Config config("../config/config.ini");
        std::string server_address = config.getString("Client.server_address", "127.0.0.1");
        unsigned short server_port = static_cast<unsigned short>(config.getInt("Server.port", 8080));
        double client_value = config.getDouble("Client.value", randomValue);

        // initializing the network context
        boost::asio::io_context io_context;

        // create and start client
        UdpClient client(io_context, server_address, server_port);

        std::cout << "Sending request to server at " << server_address << ":" << server_port << " with value: " << client_value << std::endl;

        client.sendRequest(client_value);

        std::cout << "Request sent successfully." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Client Exception: " << e.what() << std::endl;
    }

    return 0;
}
