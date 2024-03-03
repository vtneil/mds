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

# Functions
function(new_target target_name target_file)
    add_executable(${target_name} ${sources} ${target_file})
    target_compile_options(${target_name} PRIVATE
            ${opt_base_compile_flags} ${ARGN}
    )
endfunction()

# Targets
new_target(program main.cpp)
new_target(benchmark bench.cpp)
new_target(naive naive.cpp)
new_target(test main.cpp ${omp_flag} "-DD1;-DD2;-DD3")