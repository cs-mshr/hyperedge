cmake_minimum_required(VERSION 3.28.3)
project(GoQuant)

set(CMAKE_CXX_STANDARD 20)
set(INCULDE_PATHS dependencies/Crow/include)

add_executable(GoQuant src/main.cpp)

target_include_directories(GoQuant PUBLIC ${INCLUDE_PATHS})

