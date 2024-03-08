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

    container::graph_adjacency_list<int, 16, 16> adj;
    container::graph_node<int> n1 = {1, {9, 8, 9}};
    container::graph_node<int> n2 = {4, {8, 9, 8}};

    adj.push_nodes(n1, n2, container::graph_node<int>{1, {6, 6, 4}});

    for (auto &edges: adj) {
        edges.push_back(rand() % 10, rand() % 10, rand() % 10, rand() % 10, rand() % 10);
    }

    for (auto &edges: adj) {
        for (auto &vertex: edges) {
            io::print(vertex, " ");
        }
        io::println();
    }

    return 0;
}
