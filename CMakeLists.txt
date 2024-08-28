# Устанавливаем версию Windows 10
add_definitions(-D_WIN32_WINNT=0x0A00)

# Остальной ваш CMakeLists.txt файл
cmake_minimum_required(VERSION 3.10)
project(UDP_Cross_Platform)

set(CMAKE_CXX_STANDARD 17)

# Подключаем Boost
set(BOOST_ROOT "C:/boost_1_86_0")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")

find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${BOOST_LIBRARYDIR})  # Добавляем директорию с библиотеками
    add_definitions(-DBOOST_ALL_NO_LIB) # предотвращает автоподключение библиотек
else()
    message(FATAL_ERROR "Boost not found! Include directory: ${BOOST_INCLUDEDIR}")
endif()

# Указание пути к заголовочным файлам проекта
include_directories(${CMAKE_SOURCE_DIR}/include)

# Цель для сборки сервера
add_executable(udp_server src/udp_server_main.cpp src/udp_server.cpp src/config.cpp)
target_link_libraries(udp_server ${Boost_LIBRARIES})

# Цель для сборки клиента
add_executable(udp_client src/udp_client_main.cpp src/udp_client.cpp src/config.cpp)
target_link_libraries(udp_client ${Boost_LIBRARIES})



