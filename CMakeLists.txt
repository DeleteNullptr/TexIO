cmake_minimum_required(VERSION 3.8)
project(TexIO C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../Bin/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../Bin/)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../Bin/)

set(SOURCE_FILES Inc/TGA_IO.h Inc/Common.h Inc/TexIO.h Inc/ColorFormats.h Inc/FormatIO.h)
add_library(TexIO ${SOURCE_FILES})

set_target_properties(TexIO PROPERTIES LINKER_LANGUAGE CXX)

set(TEST_FILES Tests/main.cpp)
add_executable(Test ${TEST_FILES})