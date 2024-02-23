cmake_minimum_required(VERSION 3.11)

project(Tcp CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(Boost 1.78.0 REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(TCP_server src/tcp_server.cpp src/listener.cpp src/listener.h)
add_executable(TCP_client src/tcp_client.cpp )
target_link_libraries(TCP_server PRIVATE Threads::Threads)
target_link_libraries(TCP_client PRIVATE Threads::Threads)