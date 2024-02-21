cmake_minimum_required(VERSION 3.24)
project(lab515)

link_libraries(ws2_32)

set(CMAKE_CXX_STANDARD 17)

add_executable(lab515 main.cpp network.cpp data.cpp)
