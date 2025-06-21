cmake_minimum_required(VERSION 3.22...3.28)

project(discardClient)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

add_executable(discardClient discardClient.cpp)
target_link_libraries(discardClient mymuduo)
