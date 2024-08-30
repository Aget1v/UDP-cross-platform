#include <iostream>
#include<random>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include "udp_client.hpp"
#include "config.hpp"

using namespace boost::placeholders;

int main() {
    std::random_device rd;  // Источник истинной энтропии, используется для инициализации генератора
    std::mt19937 generator(rd());  // Генератор на основе алгоритма Mersenne Twister
    std::uniform_real_distribution<double> distribution;
    double randomValue = distribution(generator);

    try {
        // Загрузка конфигурации
        Config config("../config/config.ini");
        std::string server_address = config.getString("Client.server_address", "127.0.0.1");
        unsigned short server_port = static_cast<unsigned short>(config.getInt("Server.port", 8080));
        double client_value = config.getDouble("Client.value", randomValue);

        // Инициализация сетевого контекста
        boost::asio::io_context io_context;

        // Создание и запуск UDP клиента
        UdpClient client(io_context, server_address, server_port);

        // Добавление отладочных сообщений
        std::cout << "Sending request to server at " << server_address << ":" << server_port << " with value: " << client_value << std::endl;

        // Отправка запроса
        client.sendRequest(client_value);

        // Сообщение об успешной отправке
        std::cout << "Request sent successfully." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Client Exception: " << e.what() << std::endl;
    }

    return 0;
}
