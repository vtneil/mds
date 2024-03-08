#include <climits>
#include "lib_hpa.h"

constexpr size_t REGION_SIZE = 1024UL * (1024UL * 1024UL);
constexpr size_t REGION_ALIGNMENT = sizeof(void *);

using Integral = int32_t;

constexpr Integral deg = 5;

using Graph = container::graph_adjacency_list<Integral, deg>;
using Set = container::dynamic_array_t<Integral, deg>;
using BitSet = container::array_t<int_fast16_t, deg>;
using GraphBitMap = container::array_t<BitSet, deg>;

memory::virtual_stack_region_t<REGION_SIZE, REGION_ALIGNMENT, memory::NewAllocator> glob_region;
memory::virtual_stack_region_t<REGION_SIZE, REGION_ALIGNMENT, memory::NewAllocator> region;

types::pointer<GraphBitMap> bmp;

// Function to check if a set of vertices is a dominating set
int_fast8_t isDominatingSet(types::const_reference<Set> set) {
    BitSet &dominated = region.allocate<BitSet>();

    for (Integral vertex: set) {
        for (Integral i = 0; i < deg; ++i) {
            dominated[i] += (*bmp)[vertex][i];
        }
    }

    auto val = static_cast<int_fast8_t>(dominated.all());
    io::println(dominated);

    region.deallocate<BitSet>();

    return val;
}

// Utility function to find the smallest dominating set by checking all subsets
Integral findSmallestDominatingSet() {
    Integral smallestSize = std::numeric_limits<Integral>::max();

    for (Integral i = 1; i < (1 << deg); ++i) {
        Set &currentSet = region.allocate<Set>();
        for (Integral j = 0; j < deg; ++j) {
            if (LIKELY(i & (1 << j))) {
                currentSet.push_back(j);
            }
        }

        // Check if the current set is a dominating set
        if (isDominatingSet(currentSet)) {
            smallestSize = std::min(smallestSize, static_cast<Integral>(currentSet.size()));
        }

        region.deallocate<Set>();
    }

    return smallestSize;
}

int main() {
    // Example graph represented as an adjacency list
    // Graph vertices are 0-indexed
    Graph graph = {};

    benchmark::run_measure<1>([&]() -> void {
        for (Integral i = 0; i < deg; ++i) {
            for (Integral j = 0; j < deg; ++j) {
                graph.push_nodes(container::graph_node<Integral>{i, {j}});
            }
        }
    });

    benchmark::run_measure<1>([&]() -> void {
        bmp = glob_region.allocate_ptr_unsafe<GraphBitMap>();
        for (Integral vertex = 0; vertex < deg; ++vertex) {
            (*bmp)[vertex][vertex] = 1;
            for (const auto &edge: graph[vertex]) {
                (*bmp)[vertex][edge] = 1;
            }
        }
    });

    Integral n;

    benchmark::run_measure<1>([&]() -> void {
        n = findSmallestDominatingSet();
    });

    std::cout << "The size of the smallest dominating set is: "
              << n << std::endl;

    return 0;
}
