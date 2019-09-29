cmake_minimum_required(VERSION 2.8)

project(simd_examples)
set(CMAKE_CXX_STANDARD 11)


find_package(OpenMP REQUIRED)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

find_package(Boost COMPONENTS program_options REQUIRED)
message("boost includes: ${Boost_INCLUDE_DIR}")
message("boost libs: ${Boost_LIBRARIES}")

add_executable(${CMAKE_PROJECT_NAME} main.cpp helper.cpp chrome_keying.cpp convolution.cpp)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIR})
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC "-O2" "-march=native")

target_link_libraries(${CMAKE_PROJECT_NAME} -lglfw -lGL ${Boost_LIBRARIES})
