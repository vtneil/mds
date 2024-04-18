#include <bitset>
#include <algorithm>
#include "lib_hpa.h"

constexpr size_t VECTOR_WIDTH = 256;
constexpr size_t REGION_SIZE = 64UL * (1024UL * 1024UL);  // Region Size in Bytes
constexpr size_t REGION_ALIGNMENT = 16;  // Alignment in Bytes

using Integral = types::default_aligned_t<int32_t, 16>;
using Bit = int8_t;

static Integral::type num_vertex;
constexpr Integral::type MAX_VERTEX = 128;

using CombBitMask = container::array_t<Bit, MAX_VERTEX>;
using BitMask = container::bitset_t<MAX_VERTEX, int>;
using Graph = container::graph_t<Integral, MAX_VERTEX, MAX_VERTEX,
        container::array_t, container::dynamic_array_t, container::array_t,
        memory::NewAllocator>;

//using Set = container::dynamic_array_t<Integral, MAX_VERTEX>;
//using SubsetBitMaskArr = container::array_t<BitMask, 1ul << MAX_VERTEX>;

memory::virtual_stack_region_t<REGION_SIZE, REGION_ALIGNMENT, memory::NewAllocator> fast_region;

template<typename T>
inline constexpr void bitset_or(T &dst, const T &src) {
    dst |= src;
}

Integral::type smallest_subset(types::reference<BitMask> output_mask,
                               types::const_reference<Graph> graph) {
    // Iterate through all subsets from smallest to largest and return immediately once the subset is dominating.

    // FOR EACH K IN 1..N
    for (Integral::type k = 1; k <= num_vertex; ++k) {  // (combinatorial: num_vertex choose k)
        auto &comb_bitmask = fast_region.construct_unsafe<CombBitMask>();
        comb_bitmask.fill(1, 0, k);

        // FOR EACH COMBINATION (N, K)
        do {
            Integral::type subset_size = 0;
            alignas(64) BitMask domination(num_vertex);

            // FOR EACH SUBSET USING COMBINATION (FIND WHERE = 1)
            for (Integral::type i = 0; i < num_vertex; ++i) {
                if (LIKELY(!comb_bitmask[i]))
                    continue;

                ++subset_size;

                // Graph matrix is implicitly created during graph construction
                bitset_or(domination, graph.matrix[i]);
            }

            if (LIKELY(domination.all())) {
                // todo: Fix return to correct v mask
                memcpy(&output_mask, &domination, sizeof(BitMask));
                return subset_size;
            }

        } while (std::prev_permutation(comb_bitmask.data, comb_bitmask.data + num_vertex));

        fast_region.deallocate_unsafe<CombBitMask>();
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
//    for (Integral k = 1; k <= num_vertex; ++k) {
//        comb_bitmask.fill(1, 0, k);
//        comb_bitmask.fill(0, k, num_vertex);
//
//        do {
//            Set &current_subset = fast_region.allocate<Set>();
//            Integral subset_size = 0;
//
//            for (Integral i = 0; i < num_vertex; ++i) {
//                if (!comb_bitmask[i])
//                    continue;
//
//                ++subset_size;
//                current_subset.push_back(i);
//
//                for (Integral b = 0; b < num_vertex; ++b) {  // There are `num_vertex` bits in each `subset`
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

int main(int argc, types::pointer<types::pointer<char>> argv) {

    if (argc > 1)
        num_vertex = static_cast<Integral::type>(strtol(argv[1], nullptr, 10));
    else
        num_vertex = 1;

    io::unsync_stdio();

    auto &graph = fast_region.construct_unsafe<Graph>();
    auto &result = fast_region.construct_unsafe<BitMask>();

//    new (std::nothrow, &graph) Graph;

//    // Complete graph (Best case)
//    benchmark::run_measure<1>([&]() -> void {
//        for (Integral::type i = 0; i < num_vertex; ++i) {
//            for (Integral::type j = 0; j < num_vertex; ++j) {
//                graph.push_nodes(container::graph_node<Integral>{i, {j}});
//            }
//        }
//    }, "Initialization");

//    // Unconnected graph, zero edge (Worst case, check until last)
//    benchmark::run_measure<1>([&]() -> void {
//        for (Integral::type i = 0; i < num_vertex; ++i) {
//            graph.push_nodes(container::graph_node<Integral>{i, {}});
//        }
//    }, "Initialization");

    // Simple path graph
    benchmark::run<1>([&]() -> void {
        for (Integral::type i = 0; i < num_vertex - 1; ++i) {
            graph.push_nodes(container::graph_node<Integral>{i, {i + 1}});
            graph.push_nodes(container::graph_node<Integral>{i + 1, {i}});
        }
    }, "Initialization");

//    // Custom graph
//    benchmark::run_measure<1>([&]() -> void {
//        graph.push_nodes(container::graph_node<Integral>{0, {}},
//                         container::graph_node<Integral>{1, {}},
//                         container::graph_node<Integral>{2, {3}},
//                         container::graph_node<Integral>{3, {2}});
//    }, "Initialization");

    fast_region.info();

    Integral::type n;

    benchmark::run<1>([&]() -> void {
        n = smallest_subset(result, graph);
    }, "Algorithm");

//    io::println("Bitmask: ", result.data);
    io::println("Smallest subset is ", n);


//    display_all_subset_covers(graph);

    return 0;
}
