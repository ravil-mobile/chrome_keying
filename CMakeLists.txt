cmake_minimum_required(VERSION 2.8)

project(keying)
set(CMAKE_CXX_STANDARD 11)

add_executable(keying main.cpp helper.cpp chrome_keying.cpp)
target_compile_options(keying PUBLIC "-O2")
target_link_libraries(keying -lglfw -lGL)
