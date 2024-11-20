cmake_minimum_required(VERSION 3.13)

project(redis-starter-cpp)

enable_language(CXX)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(folly CONFIG REQUIRED)

add_executable(server ${SOURCE_FILES})

target_link_libraries(server PRIVATE asio asio::asio)
target_link_libraries(server PRIVATE Threads::Threads)
target_link_libraries(server PRIVATE Folly::folly Folly::folly_deps Folly::follybenchmark Folly::folly_test_util)