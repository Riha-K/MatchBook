cmake_minimum_required(VERSION 3.16)
project(MatchBook LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The latency and throughput numbers are only meaningful in an optimised build.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

add_executable(matchbook
    src/OrderBook.cpp
    src/main.cpp
)

target_include_directories(matchbook PRIVATE include)
