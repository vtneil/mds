#include <immintrin.h>
#include <iostream>
#include "macros_bootstrap.h"
#include "builtins_bootstrap.h"
#include "utils.h"
#include "container.h"
#include "allocator.h"

constexpr size_t REGION_SIZE = 8UL * 1024UL * 1024UL;  // 8 MiB
using region_t = memory::virtual_stack_region_t<REGION_SIZE>;

int main(int argc, types::pointer<types::pointer<char>> argv) {
    io::unsync_stdio();

    using T = int;

    region_t stack;
    stack.info();

    auto n1 = stack.allocate<T>(2000);
    stack.info();

    auto n2 = stack.allocate<T>(500);
    stack.info();

    stack.deallocate<T>(128);
    stack.info();

    stack.clear();
    stack.info();

    return 0;
}
