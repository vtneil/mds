#include <immintrin.h>
#include <iostream>
#include "lib_hpa.h"

constexpr size_t REGION_SIZE = 512UL * 1024UL * 1024UL;  // 512 MiB
constexpr size_t REGION_ALIGNMENT = sizeof(void *);

using region_t = memory::virtual_stack_region_t<REGION_SIZE, REGION_ALIGNMENT, memory::MallocAllocator>;

using large_array_t = container::array_t<int, 128UL * 1024UL * 1024U - 2UL>;  // 512 MiB

region_t stack;

int main(int argc, types::pointer<types::pointer<char>> argv) {

    using array3_t = container::array_t<int, 512>;
    types::reference<array3_t> arr = stack.allocate_unsafe<array3_t>();

    io::println("Array is at address ", memory::addressof(arr));

    arr[0] = 9;
    arr[1] = 8;
    arr[2] = 1;

    io::println("Array data is ", arr);
    io::print(stack);

    return 0;
}
