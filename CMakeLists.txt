cmake_minimum_required(VERSION 2.8)

project(keying)
set(CMAKE_CXX_STANDARD 11)

find_package(OpenMP REQUIRED)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")



add_executable(keying main.cpp helper.cpp chrome_keying.cpp convolution.cpp)
target_compile_options(keying PUBLIC "-O2" "-mavx")
target_link_libraries(keying -lglfw -lGL)
