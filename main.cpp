#include <immintrin.h>
#include <iostream>
#include "macros_bootstrap.h"
#include "builtins_bootstrap.h"
#include "utils.h"
#include "container.h"
#include "allocator.h"

constexpr size_t REGION_SIZE = 512UL * 1024UL * 1024UL;  // 512 MiB
constexpr size_t REGION_ALIGNMENT = sizeof(void *);

using region_t = memory::virtual_stack_region_t<REGION_SIZE, REGION_ALIGNMENT, memory::MallocAllocator>;

using large_array_t = container::array_t<int, 128UL * 1024UL * 1024U - 2UL>;  // 512 MiB

region_t stack;

int main(int argc, types::pointer<types::pointer<char>> argv) {

    types::reference<large_array_t> arr = stack.allocate_unsafe<large_array_t>(1);

    io::println(memory::addressof(arr));

    stack.info();

    return 0;
}
