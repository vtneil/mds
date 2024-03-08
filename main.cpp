#include <immintrin.h>
#include <iostream>
#include "lib_hpa.h"

//constexpr size_t REGION_SIZE = 16UL * (1024UL * 1024UL * 1024UL);  // GiB
constexpr size_t REGION_SIZE = 512UL * (1024UL * 1024UL);  // 512 MiB
constexpr size_t REGION_ALIGNMENT = sizeof(void *);

using region_t = memory::virtual_stack_region_t<REGION_SIZE, REGION_ALIGNMENT, memory::NewAllocator>;

using large_array_t = container::array_t<int, 128UL * 1024UL * 1024U - 2UL>;  // 512 MiB

region_t stack;

int main(int argc, types::pointer<types::pointer<char>> argv) {

    container::adjacency_list<int, 32, 32> adj;

    for (auto &edges: adj.graph_repr) {
        for (auto &vertex: edges.data) {
            io::print(vertex, " ");
        }
        io::println();
    }
    io::println(adj.graph_repr.data[0].data);

    return 0;
}
