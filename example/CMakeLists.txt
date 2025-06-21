cmake_minimum_required(VERSION 3.22...3.28)

project(EchoServer)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

add_executable(EchoServer EchoServer.cpp)
target_link_libraries(EchoServer mymuduo)

add_executable(linEchoServer epoll_linux.cpp)

# add_subdirectory(doip)
add_subdirectory(discard)
