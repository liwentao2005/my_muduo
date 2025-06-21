cmake_minimum_required(VERSION 3.22...3.28)

project(TestClient)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

add_executable(TestClient TestClient.cpp)
