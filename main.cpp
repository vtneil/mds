#include <immintrin.h>
#include <iostream>
#include "macros_bootstrap.h"
#include "builtins_bootstrap.h"
#include "utils.h"
#include "container.h"
#include "allocator.h"

constexpr size_t REGION_SIZE = 512UL * 1024UL * 1024UL;  // 512 MiB
constexpr size_t REGION_ALIGNMENT = 4;
using region_t = memory::virtual_stack_region_t<REGION_SIZE, REGION_ALIGNMENT>;

using large_array_t = container::array_t<int, 128UL * 1024UL * 1024UL>;  // 64 MiB

int main(int argc, types::pointer<types::pointer<char>> argv) {
    io::unsync_stdio();

    region_t stack;
    stack.info();

    types::reference<large_array_t> arr = stack.allocate<large_array_t>();
    stack.info();

    io::println(arr.max());

    return 0;
}
