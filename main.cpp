#include <immintrin.h>
#include <iostream>
#include "lib_hpa.h"

//constexpr size_t REGION_SIZE = 16UL * (1024UL * 1024UL * 1024UL);  // GiB
constexpr size_t REGION_SIZE = 512UL * (1024UL * 1024UL);  // 512 MiB
constexpr size_t REGION_ALIGNMENT = sizeof(void *);

using region_t = memory::virtual_stack_region_t<REGION_SIZE, REGION_ALIGNMENT, memory::NewAllocator>;

using large_array_t = container::array_t<int, 128UL * 1024UL * 1024U - 2UL>;  // 512 MiB

region_t region;

int main(int argc, types::pointer<types::pointer<char>> argv) {

    constexpr size_t Size = 100;
    container::bitset_t<Size, int> b;
    b.set(0, true);
    b.set(2, true);
    b.set(3, true);
    b.set(7, true);
    b.set(8, true);
    b.set(9, true);
    b.set(20, true);
    io::println(b.data);
    io::println(b.all());
    io::println(b.any());

    container::bitset_t<Size, int> c{b};
    io::println(c.data);
    io::println(c.all());
    io::println(c.any());

    container::bitset_t<Size, int> d;
    for (size_t i = 0; i < Size; ++i) {
        d.set(i, true);
    }
    io::println(d.data);
    io::println(d.all());
    io::println(d.any());

    d.set(d.size() - 1, false);
    io::println(d.data);
    io::println(d.all());
    io::println(d.any());

    container::bitset_t<Size, int> e;
    io::println(e.data);
    io::println(e.all());
    io::println(e.any());

    return 0;
}
