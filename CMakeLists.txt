cmake_minimum_required(VERSION 3.22)
project(hpa_2110452_min_dom_set)

# Variables
set(c_cpp_std 17)
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(opt_compile_opt "-O3")
else ()
    set(opt_compile_opt "-O0")
endif ()
set(opt_base_compile_flags "${opt_compile_opt};-march=native;-v")
set(omp_flag "-fopenmp")

# C/C++ Language Standard
set(CMAKE_CXX_STANDARD ${c_cpp_std})
set(CMAKE_C_STANDARD ${c_cpp_std})

# Include local library and set sources
include_directories(include)
set(sources
        ""
)

add_executable(program ${sources} main.cpp)
target_compile_options(program PRIVATE
        ${opt_base_compile_flags}
)

add_executable(benchmark ${sources} bench.cpp)
target_compile_options(benchmark PRIVATE
        ${opt_base_compile_flags}
)
