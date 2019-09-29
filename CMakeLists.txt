cmake_minimum_required(VERSION 2.8)

project(mm_examples)
set(CMAKE_CXX_STANDARD 11)

# find dependencies
find_package(OpenMP REQUIRED)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

find_package(Boost COMPONENTS program_options REQUIRED)

# build a helper(common) library
set(common_target "common")
set(common_dir "common")
add_library(${common_target} ${common_dir}/helper.cpp 
                             ${common_dir}/compute_kernels.cpp)

target_link_libraries(${common_target} PRIVATE -lglfw -lGL)


# targets: simd 
set(mm_simd_target "mm_simd")
set(mm_simd_dir "simd")

add_executable(${mm_simd_target} ${mm_simd_dir}/main.cpp
                                 ${mm_simd_dir}/chrome_keying.cpp 
                                 ${mm_simd_dir}/convolution.cpp)

target_include_directories(${mm_simd_target} PUBLIC ${common_dir} ${Boost_INCLUDE_DIR})
target_compile_options(${mm_simd_target} PUBLIC "-O2" "-march=native")
target_link_libraries(${mm_simd_target} ${common_target} ${Boost_LIBRARIES})


# targets: opencl 
set(mm_opencl_target "mm_opencl")
set(mm_opencl_dir "opencl")

add_executable(${mm_opencl_target} ${mm_opencl_dir}/main.cpp
                                   ${mm_opencl_dir}/compute_kernels.cpp)
                                   
target_include_directories(${mm_opencl_target} PUBLIC ${common_dir} ${Boost_INCLUDE_DIR})
target_compile_options(${mm_opencl_target} PUBLIC "-g")
target_link_libraries(${mm_opencl_target} ${common_target} ${Boost_LIBRARIES})