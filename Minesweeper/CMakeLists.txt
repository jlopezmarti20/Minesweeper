

cmake_minimum_required(VERSION 3.14)
project(Minesweeper)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp Game.cpp Random.cpp TextureManager.cpp)
add_executable(Minesweeper ${SOURCE_FILES})
include_directories(/usr/local/include)

find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
include_directories(${SFML_INCLUDE_DIRS})
target_link_libraries(Minesweeper sfml-system sfml-window sfml-graphics sfml-audio sfml-network)