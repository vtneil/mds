#include <immintrin.h>
#include <iostream>
#include "macros_bootstrap.h"
#include "builtins_bootstrap.h"
#include "utils.h"
#include "container.h"
#include "allocator.h"

constexpr size_t REGION_SIZE = 512UL * 1024UL * 1024UL;  // 512 MiB
constexpr size_t REGION_ALIGNMENT = 4;
using region_t = memory::virtual_stack_region_t<REGION_SIZE, REGION_ALIGNMENT, memory::MallocAllocator>;

using large_array_t = container::array_t<int, 128UL * 1024UL * 1024UL>;  // 512 MiB

int main(int argc, types::pointer<types::pointer<char>> argv) {

    container::array_t<int, 32> arr;

    for (int i = 0; i < 32; ++i) {
        arr[i] = i + 1;
    }

    for (int i = 0; i < 32; ++i) {
        io::println(arr[i]);
    }

    io::println(arr.sum());
    io::println(arr.min());
    io::println(arr.max());

    return 0;
}
