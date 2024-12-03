cmake_minimum_required(VERSION 3.28.3)
project(GoQuant)

set(CMAKE_CXX_STANDARD 20)
set(INCLUDE_PATHS dependencies/Crow/include)

add_executable(GoQuant src/main.cpp)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 1.11.0)
FetchContent_MakeAvailable(cpr)

set(CMAKE_MESSAGE_LOG_LEVEL NOTICE)
target_link_libraries(GoQuant PRIVATE cpr::cpr)
target_include_directories(GoQuant PUBLIC ${INCLUDE_PATHS})