#include <bitset>
#include <algorithm>
#include "lib_hpa.h"

constexpr size_t REGION_SIZE = 1024UL * (1024UL * 1024UL);  // Region Size in Bytes
constexpr size_t REGION_ALIGNMENT = 16;  // Alignment in Bytes

using Integral = types::default_aligned_t<int32_t, 16>;
using Bit = int_fast8_t;

// todo: Number of Vertex might not equal MAX_VERTEX
constexpr Integral::type MAX_VERTEX = 24;

using Graph = container::graph_adjacency_list<Integral, MAX_VERTEX, MAX_VERTEX, container::array_t, container::dynamic_array_t, container::array_t>;
using CombBitMask = container::array_t<Bit, MAX_VERTEX>;
using BitMask = std::bitset<MAX_VERTEX>;
//using Set = container::dynamic_array_t<Integral, MAX_VERTEX>;
//using SubsetBitMaskArr = container::array_t<BitMask, 1ul << MAX_VERTEX>;

memory::virtual_stack_region_t<REGION_SIZE, REGION_ALIGNMENT, memory::NewAllocator> fast_region;

Integral::type smallest_subset(const Graph &graph) {

    // 1. Pre-allocate region in closest call order to ensure fastest access
    auto &comb_bitmask = fast_region.allocate_unsafe<CombBitMask>();

    // 2. Iterate through all subsets from smallest to largest and return immediately once the subset is dominating.

    for (Integral::type k = 1; k <= MAX_VERTEX; ++k) {  // (combinatorial: MAX_VERTEX choose k)
        comb_bitmask.fill(1, 0, k);
        comb_bitmask.fill(0, k, MAX_VERTEX);

        do {
            Integral::type subset_size = 0;
            auto &domination = fast_region.allocate_unsafe<BitMask>();

            for (Integral::type i = 0; i < MAX_VERTEX; ++i) {
                if (LIKELY(!comb_bitmask[i]))
                    continue;

                ++subset_size;

                // Graph cover is implicitly created during graph construction
                domination |= graph.cover[i];
            }

            if (LIKELY(domination.all()))
                return subset_size;

            fast_region.deallocate_unsafe<BitMask>();

        } while (std::prev_permutation(comb_bitmask.data, comb_bitmask.data + MAX_VERTEX));
    }

    return 0;
}

//types::reference<SubsetBitMaskArr> display_all_subset_covers(const Graph &graph) {
//    types::reference<GraphBitMaskArr> bitmask = fast_region.allocate<GraphBitMaskArr>();
//    generate_bitmask(bitmask, graph);
//
//    SubsetBitMaskArr &subsets = fast_region.allocate<SubsetBitMaskArr>();
//    BitMask &comb_bitmask = fast_region.allocate<BitMask>();
//    Integral subset_num = 1;
//
//    for (Integral k = 1; k <= MAX_VERTEX; ++k) {
//        comb_bitmask.fill(1, 0, k);
//        comb_bitmask.fill(0, k, MAX_VERTEX);
//
//        do {
//            Set &current_subset = fast_region.allocate<Set>();
//            Integral subset_size = 0;
//
//            for (Integral i = 0; i < MAX_VERTEX; ++i) {
//                if (!comb_bitmask[i])
//                    continue;
//
//                ++subset_size;
//                current_subset.push_back(i);
//
//                for (Integral b = 0; b < MAX_VERTEX; ++b) {  // There are `MAX_VERTEX` bits in each `subset`
//                    subsets[subset_num][b] |= bitmask[i][b];
//                }
//            }
//
//            io::println("num ", subset_num, " size ", subset_size, " with ", current_subset, " : ",
//                        subsets[subset_num]);
//            ++subset_num;
//
//            fast_region.deallocate<Set>();
//
//        } while (std::prev_permutation(comb_bitmask.begin(), comb_bitmask.end()));
//    }
//
//    return subsets;
//}

int main() {
    io::unsync_stdio();

    auto &graph = fast_region.allocate_unsafe<Graph>();

//    // Complete graph (Best case)
//    benchmark::run_measure<1>([&]() -> void {
//        for (Integral::type i = 0; i < MAX_VERTEX; ++i) {
//            for (Integral::type j = 0; j < MAX_VERTEX; ++j) {
//                graph.push_nodes(container::graph_node<Integral>{i, {j}});
//            }
//        }
//    }, "Initialization");

    // Unconnected graph, zero edge (Worst case, check until last)
    benchmark::run_measure<1>([&]() -> void {
        for (Integral::type i = 0; i < MAX_VERTEX; ++i) {
            graph.push_nodes(container::graph_node<Integral>{i, {}});
        }
    }, "Initialization");

//    // Simple path graph
//    benchmark::run_measure<1>([&]() -> void {
//        for (Integral i = 0; i < MAX_VERTEX - 1; ++i) {
//            graph.push_nodes(container::graph_node<Integral>{i, {ii + 1}});
//            graph.push_nodes(container::graph_node<Integral>{i + 1, {i}});
//        }
//    }, "Initialization");

//    // Custom graph
//    benchmark::run_measure<1>([&]() -> void {
//        graph.push_nodes(container::graph_node<Integral>{0, {}},
//                         container::graph_node<Integral>{1, {}},
//                         container::graph_node<Integral>{2, {3}},
//                         container::graph_node<Integral>{3, {2}});
//    }, "Initialization");

    Integral::type n;

    benchmark::run_measure<1>([&]() -> void {
        n = smallest_subset(graph);
    }, "Algorithm");

    io::println("Smallest subset is ", n);

    fast_region.info();

//    display_all_subset_covers(graph);

    return 0;
}
