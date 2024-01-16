cmake_minimum_required(VERSION 3.10)

project(nyanGBE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_subdirectory(src)
include_directories(src)