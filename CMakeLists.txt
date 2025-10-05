cmake_minimum_required(VERSION 3.10)

project(med VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wall -Wextra -pedantic -g)

file(GLOB SOURCES "src/*.cpp")

add_executable(med ${SOURCES})

