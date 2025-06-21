cmake_minimum_required(VERSION 3.22)

project(mymuduo)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -fPIC")

aux_source_directory(. SRC_LIST)

add_library(mymuduo SHARED ${SRC_LIST})

set_target_properties(mymuduo PROPERTIES
    VERSION 1.0.1
    SOVERSION 1
)

set(_include_dir "/usr/include/mymuduo")
# if(APPEND_PROJECT_NAME_TO_INCLUDEDIR)
#   string(APPEND _include_dir "${PROJECT_NAME}/")
# endif()
set(INCLUDE_INSTALL_DIR "${_include_dir}" CACHE PATH "Installation directory for C++ headers")

install(TARGETS mymuduo
    LIBRARY DESTINATION /usr/lib
)

# set(HEADERS
#   Buffer.h
#   Callbacks.h
#   Channel.h
#   EventLoop.h
#   EventLoopThread.h
#   EventLoopThreadPool.h
#   InetAddress.h
#   TcpClient.h
#   TcpConnection.h
#   TcpServer.h
#   )
# install(FILES ${HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR})

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR})

add_subdirectory(example)
add_subdirectory(tools)
