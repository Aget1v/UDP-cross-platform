# for Windows 10
add_definitions(-D_WIN32_WINNT=0x0A00)

cmake_minimum_required(VERSION 3.10)
project(UDP_Cross_Platform)

set(CMAKE_CXX_STANDARD 17)

set(BOOST_ROOT "C:/boost_1_86_0")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")

find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${BOOST_LIBRARYDIR})  
    add_definitions(-DBOOST_ALL_NO_LIB) 
else()
    message(FATAL_ERROR "Boost not found!")
endif()

# headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# server
add_executable(udp_server src/udp_server_main.cpp src/udp_server.cpp src/config.cpp)
target_link_libraries(udp_server ${Boost_LIBRARIES})

# client
add_executable(udp_client src/udp_client_main.cpp src/udp_client.cpp src/config.cpp)
target_link_libraries(udp_client ${Boost_LIBRARIES})



