cmake_minimum_required(VERSION 3.10)

project(med VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_compile_options(-Wall -Wextra -pedantic -g)

file(GLOB SOURCES "src/*.cpp")

add_executable(med ${SOURCES})
install(TARGETS med RUNTIME DESTINATION bin)

