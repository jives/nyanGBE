cmake_minimum_required(VERSION 3.10)

project(nyanGBE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

find_package(SDL2 REQUIRED)

add_subdirectory(src)
target_include_directories(nyanGBE PRIVATE src)
