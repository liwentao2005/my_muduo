cmake_minimum_required(VERSION 3.22...3.28)

project(Doip)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

add_executable(DoipServer doipServer.cpp)
target_link_libraries(DoipServer mymuduo)

add_executable(DoipClient doipClient.cpp)
target_link_libraries(DoipClient mymuduo)
