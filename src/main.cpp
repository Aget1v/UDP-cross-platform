#include <iostream>
#include <boost/asio.hpp>
#include "udp_server.hpp"
#include "config.hpp"

int main() {
    try {
        // Загрузка конфигурационного файла
        Config config("../config/config.ini");
        unsigned short port = static_cast<unsigned short>(config.getInt("Server.port", 8080));

        // Инициализация сетевого контекста
        boost::asio::io_context io_context;

        // Создание и запуск UDP сервера
        UdpServer server(io_context, port);
        server.start();

        // Запуск io_context для обработки асинхронных операций
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
