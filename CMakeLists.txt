cmake_minimum_required(VERSION 3.10)
project(krestik_nolik)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(krestik_nolik main.cpp)

if(WIN32)
    target_link_libraries(krestik_nolik ws2_32)
endif()