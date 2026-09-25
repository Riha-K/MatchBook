cmake_minimum_required(VERSION 3.16)
project(MatchBook LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(matchbook
    src/OrderBook.cpp
    src/main.cpp
)

target_include_directories(matchbook PRIVATE include)
