#include <climits>
#include <algorithm>
#include "lib_hpa.h"

constexpr size_t REGION_SIZE = 512UL * (1024UL * 1024UL);
constexpr size_t REGION_ALIGNMENT = sizeof(void *);

using Integral = int32_t;
using Bit = int_fast8_t;

constexpr Integral deg = 20;

using Graph = container::graph_adjacency_list<Integral, deg>;
using Set = container::dynamic_array_t<Integral, deg>;
using BitMask = container::array_t<Bit, deg>;
using GraphBitMaskArr = container::array_t<BitMask, deg>;
using SubsetBitMaskArr = container::array_t<BitMask, 1ul << deg>;

memory::virtual_stack_region_t<REGION_SIZE, REGION_ALIGNMENT, memory::NewAllocator> glob_region;
memory::virtual_stack_region_t<REGION_SIZE, REGION_ALIGNMENT, memory::NewAllocator> fast_region;
types::pointer<GraphBitMaskArr> bmp;

types::reference<SubsetBitMaskArr> display_all_subset_covers() {
    SubsetBitMaskArr &subsets = glob_region.allocate<SubsetBitMaskArr>();
    BitMask &bitmask = glob_region.allocate<BitMask>();
    Integral subset_num = 1;

    for (Integral k = 1; k <= deg; ++k) {
        bitmask.fill(1, 0, k);
        bitmask.fill(0, k, deg);

        do {
            Set &current_subset = fast_region.allocate<Set>();
            Integral subset_size = 0;

            for (Integral i = 0; i < deg; ++i) {
                if (!bitmask[i])
                    continue;

                ++subset_size;
                current_subset.push_back(i);

                for (Integral b = 0; b < deg; ++b) {  // There are `deg` bits in each `subset`
                    subsets[subset_num][b] |= (*bmp)[i][b];
                }
            }

            io::println("num ", subset_num, " size ", subset_size, " with ", current_subset, " : ",
                        subsets[subset_num]);
            ++subset_num;

            fast_region.deallocate<Set>();

        } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
    }

    return subsets;
}

Integral smallest_subset() {
    SubsetBitMaskArr &subsets = glob_region.allocate<SubsetBitMaskArr>();
    BitMask &bitmask = glob_region.allocate<BitMask>();
    Integral subset_num = 1;

    for (Integral k = 1; k <= deg; ++k) {
        bitmask.fill(1, 0, k);
        bitmask.fill(0, k, deg);

        do {
            Integral subset_size = 0;
            BitMask &domination = fast_region.allocate<BitMask>();

            for (Integral i = 0; i < deg; ++i) {
                if (!bitmask[i])
                    continue;

                ++subset_size;

                for (Integral b = 0; b < deg; ++b) {  // There are `deg` bits in each `subset`
                    domination[b] |= (*bmp)[i][b];
                }
            }

            if (domination.all())
                return subset_size;

            ++subset_num;

            fast_region.deallocate<BitMask>();

        } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
    }

    return 0;
}

int main(int argc, char **argv) {
    io::unsync_stdio();

    Graph graph = {};

//    // Complete graph (Best case)
//    benchmark::run_measure<1>([&]() -> void {
//        for (Integral i = 0; i < deg; ++i) {
//            for (Integral j = 0; j < deg; ++j) {
//                graph.push_nodes(container::graph_node<Integral>{i, {j}});
//            }
//        }
//    });

    // Unconnected graph, zero edge (Worst case, check until last)
    benchmark::run_measure<1>([&]() -> void {
        for (Integral i = 0; i < deg; ++i) {
            graph.push_nodes(container::graph_node<Integral>{i, {}});
        }
    });

//    // Simple path graph
//    benchmark::run_measure<1>([&]() -> void {
//        for (Integral i = 0; i < deg - 1; ++i) {
//            graph.push_nodes(container::graph_node<Integral>{i, {i + 1}});
//            graph.push_nodes(container::graph_node<Integral>{i + 1, {i}});
//        }
//    });

//    // Custom graph
//    benchmark::run_measure<1>([&]() -> void {
//        graph.push_nodes(container::graph_node<Integral>{0, {}},
//                         container::graph_node<Integral>{1, {}},
//                         container::graph_node<Integral>{2, {3}},
//                         container::graph_node<Integral>{3, {2}});
//    });

    // Generate set cover using bitmask
    benchmark::run_measure<1>([&]() -> void {
        bmp = glob_region.allocate_ptr_unsafe<GraphBitMaskArr>();
        for (Integral vertex = 0; vertex < deg; ++vertex) {
            (*bmp)[vertex][vertex] = 1;
            for (const auto &edge: graph[vertex]) {
                (*bmp)[vertex][edge] = 1;
            }
//            io::println((*bmp)[vertex]);
        }
    });

    Integral n;

    benchmark::run_measure<1>([&]() -> void {
        n = smallest_subset();
    });

    io::println("Smallest subset is ", n);

//    display_all_subset_covers();

    return 0;
}
