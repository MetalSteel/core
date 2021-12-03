cmake_minimum_required(VERSION 3.10)
project(core)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

add_executable(core main.cc
        src/tcp_server.cc
        src/tcp_server.h
        src/socket_address.cc
        src/socket_address.h
        src/event_loop.cc
        src/event_loop.h
        src/event.h
        src/tcp_connection.cc
        src/tcp_connection.h
        src/event_loop.cc
        src/event_loop.h src/misc.h)
