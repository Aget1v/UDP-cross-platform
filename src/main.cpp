#include <iostream>
#include <boost/asio.hpp>
#include "udp_server.hpp"
#include "udp_client.hpp"
#include "config.hpp"

int main() {
    try {
        // Загрузка конфигурации
        Config config("../config/config.ini");
        unsigned short server_port = static_cast<unsigned short>(config.getInt("Server.port", 8080));
        std::string server_address = config.getString("Client.server_address", "127.0.0.1");
        double client_value = config.getDouble("Client.value", 100.0);

        // Инициализация сетевого контекста
        boost::asio::io_context io_context;

        // Создание и запуск UDP сервера
        UdpServer server(io_context, server_port);
        server.start();

        // Создание и запуск UDP клиента
        UdpClient client(io_context, server_address, server_port);
        client.sendRequest(client_value);

        // Запуск io_context для обработки асинхронных операций
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
