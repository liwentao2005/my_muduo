# Install script for directory: /home/user/wkspaces/projects/myMuduo

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}/usr/lib/libmymuduo.so.1.0.1"
      "$ENV{DESTDIR}/usr/lib/libmymuduo.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/lib/libmymuduo.so.1.0.1;/usr/lib/libmymuduo.so.1")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/lib" TYPE SHARED_LIBRARY FILES
    "/home/user/wkspaces/projects/myMuduo/lib/libmymuduo.so.1.0.1"
    "/home/user/wkspaces/projects/myMuduo/lib/libmymuduo.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}/usr/lib/libmymuduo.so.1.0.1"
      "$ENV{DESTDIR}/usr/lib/libmymuduo.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/usr/lib/libmymuduo.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/lib/libmymuduo.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/usr/lib/libmymuduo.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/lib/libmymuduo.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/lib" TYPE SHARED_LIBRARY FILES "/home/user/wkspaces/projects/myMuduo/lib/libmymuduo.so")
  if(EXISTS "$ENV{DESTDIR}/usr/lib/libmymuduo.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/lib/libmymuduo.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/usr/lib/libmymuduo.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/include/mymuduo/Acceptor.h;/usr/include/mymuduo/Buffer.h;/usr/include/mymuduo/Callbacks.h;/usr/include/mymuduo/Channel.h;/usr/include/mymuduo/Connector.h;/usr/include/mymuduo/CurrentThread.h;/usr/include/mymuduo/EPollPoller.h;/usr/include/mymuduo/EventLoop.h;/usr/include/mymuduo/EventLoopThread.h;/usr/include/mymuduo/EventLoopThreadPool.h;/usr/include/mymuduo/InetAddress.h;/usr/include/mymuduo/Logger.h;/usr/include/mymuduo/Poller.h;/usr/include/mymuduo/Socket.h;/usr/include/mymuduo/TcpClient.h;/usr/include/mymuduo/TcpConnection.h;/usr/include/mymuduo/TcpServer.h;/usr/include/mymuduo/Thread.h;/usr/include/mymuduo/Timestamp.h;/usr/include/mymuduo/copyable.h;/usr/include/mymuduo/noncopyable.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/include/mymuduo" TYPE FILE FILES
    "/home/user/wkspaces/projects/myMuduo/Acceptor.h"
    "/home/user/wkspaces/projects/myMuduo/Buffer.h"
    "/home/user/wkspaces/projects/myMuduo/Callbacks.h"
    "/home/user/wkspaces/projects/myMuduo/Channel.h"
    "/home/user/wkspaces/projects/myMuduo/Connector.h"
    "/home/user/wkspaces/projects/myMuduo/CurrentThread.h"
    "/home/user/wkspaces/projects/myMuduo/EPollPoller.h"
    "/home/user/wkspaces/projects/myMuduo/EventLoop.h"
    "/home/user/wkspaces/projects/myMuduo/EventLoopThread.h"
    "/home/user/wkspaces/projects/myMuduo/EventLoopThreadPool.h"
    "/home/user/wkspaces/projects/myMuduo/InetAddress.h"
    "/home/user/wkspaces/projects/myMuduo/Logger.h"
    "/home/user/wkspaces/projects/myMuduo/Poller.h"
    "/home/user/wkspaces/projects/myMuduo/Socket.h"
    "/home/user/wkspaces/projects/myMuduo/TcpClient.h"
    "/home/user/wkspaces/projects/myMuduo/TcpConnection.h"
    "/home/user/wkspaces/projects/myMuduo/TcpServer.h"
    "/home/user/wkspaces/projects/myMuduo/Thread.h"
    "/home/user/wkspaces/projects/myMuduo/Timestamp.h"
    "/home/user/wkspaces/projects/myMuduo/copyable.h"
    "/home/user/wkspaces/projects/myMuduo/noncopyable.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/user/wkspaces/projects/myMuduo/build/example/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/user/wkspaces/projects/myMuduo/build/tools/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/user/wkspaces/projects/myMuduo/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
