#include <fstream>
#include <algorithm>
#include "lib_hpa.h"

constexpr size_t REGION_SIZE = 128UL * (1024UL * 1024UL);  // Region Size in Bytes
constexpr size_t REGION_ALIGNMENT = 64;  // Alignment in Bytes

using Integral = types::default_aligned_t<int_fast32_t, 0>;
using Bit = int_fast8_t;

constexpr Integral::type MAX_VERTEX = 128;

using CombBitMask = container::array_t<Bit, MAX_VERTEX>;
using BitMask = container::bitset_t<MAX_VERTEX, int_fast32_t>;
using Graph = container::graph_t<Integral, MAX_VERTEX, MAX_VERTEX,
        container::array_t, container::dynamic_array_t, container::array_t,
        memory::NewAllocator>;

memory::virtual_stack_region_t<REGION_SIZE, REGION_ALIGNMENT, memory::NewAllocator> fast_region;


Integral::type smallest_subset_bsearch(types::reference<CombBitMask> output_mask,
                                       types::const_reference<Graph> graph) {
    // Pre-allocation
    ALIGNED(64) BitMask domination(graph.v);
    ALIGNED(64) BitMask mask_truth[2] = {BitMask(graph.v),
                                         BitMask(graph.v)};
    mask_truth[1].set();

    CombBitMask comb_bitmask;

    Integral::type ans = -1;
    Integral::type low = 1;
    Integral::type high = graph.v;
    Integral::type k;

    // Binary search k (combinatorial: graph.v choose k)
    while (true) {
        if (LIKELY(low > high))
            break;

        Integral::type found = 0;
        k = low + (high - low) / 2;

        comb_bitmask.fill(1, 0, k);
        comb_bitmask.fill(0, k, graph.v);

        // FOR EACH COMBINATION IN (N, K)
        do {
            domination.reset();

            // FOR EACH SUBSET USING COMBINATION (FIND WHERE = 1)
            for (Integral::type i = 0; i < graph.v; ++i) {
                domination |= graph.matrix[i] & mask_truth[comb_bitmask[i]];
            }

            // Found: mark this and try searching smaller sets
            // how to go branchless?
            if (domination.popcount_all()) {
                found = 1;
                high = k - 1;
                ans = k;

                output_mask = comb_bitmask;
                break;
            }

        } while (std::prev_permutation(comb_bitmask.data, comb_bitmask.data + graph.v));

        // Not found: try searching larger sets
        low = (found) ? low : k + 1;
    }

    return ans;
}


Integral::type smallest_subset_linear_ascending(types::reference<CombBitMask> output_mask,
                                                types::const_reference<Graph> graph) {
    ALIGNED(64) BitMask domination(graph.v);
    ALIGNED(64) BitMask mask_truth[2] = {BitMask(graph.v),
                                         BitMask(graph.v)};
    mask_truth[1].set();

    CombBitMask comb_bitmask;

    // Iterate through all subsets from smallest to largest and return immediately once the subset is dominating.
    for (Integral::type k = 1; k <= graph.v; ++k) {  // (combinatorial: graph.v choose k)
        comb_bitmask.fill(1, 0, k);
        comb_bitmask.fill(0, k, graph.v);

        // FOR EACH COMBINATION (N, K)
        do {
            domination.reset();

            // FOR EACH SUBSET USING COMBINATION (FIND WHERE = 1)
            for (Integral::type i = 0; i < graph.v; ++i) {
                domination |= graph.matrix[i] & mask_truth[comb_bitmask[i]];
            }

            if (domination.popcount_all()) {
                output_mask = comb_bitmask;
                return k;
            }

        } while (std::prev_permutation(comb_bitmask.data, comb_bitmask.data + graph.v));
    }

    return 0;
}

Integral::type smallest_subset_linear_descending(types::reference<CombBitMask> output_mask,
                                                 types::const_reference<Graph> graph) {
    ALIGNED(64) BitMask domination(graph.v);
    ALIGNED(64) BitMask mask_truth[2] = {BitMask(graph.v),
                                         BitMask(graph.v)};
    mask_truth[1].set();

    Integral::type is_dom;
    Integral::type ans;
    CombBitMask comb_bitmask;

    // Iterate through all subsets from smallest to largest and return immediately once the subset is dominating.
    for (Integral::type k = graph.v; k > 0; --k) {  // (combinatorial: graph.v choose k)
        comb_bitmask.fill(1, 0, k);
        comb_bitmask.fill(0, k, graph.v);

        // FOR EACH COMBINATION (N, K)
        do {
            domination.reset();

            // FOR EACH SUBSET USING COMBINATION (FIND WHERE = 1)
            for (Integral::type i = 0; i < graph.v; ++i) {
                domination |= graph.matrix[i] & mask_truth[comb_bitmask[i]];
            }

            is_dom = domination.popcount_all();

            if (is_dom) {
                ans = k;
                output_mask = comb_bitmask;
                break;
            }

        } while (std::prev_permutation(comb_bitmask.data, comb_bitmask.data + graph.v));

        if (!is_dom) break;
    }

    return ans;
}

int main(int argc, types::pointer<types::pointer<char>> argv) {
    if (argc < 2)
        return -1;

    // Problem Instance
    auto &graph = fast_region.construct_unsafe<Graph>();
    auto &result = fast_region.construct_unsafe<CombBitMask>();

    // IO
    io::unsync_stdio();

    bool success;
    benchmark::run<1>([&]() -> void {
        success = graph.from_file(argv[1]);
    }, "I/O & Graph Initialization");

    if (!success) {
        io::println("Error opening file!");
        return 255;
    }

    // Graph classes
    // int mode = ((graph.density() > 0.3f) << 1) | (graph.dispersion() > 0.5);
    // io::println(mode);

    // Algorithms
    Integral::type n;

    //
    benchmark::run<1>([&]() -> void {
        n = smallest_subset_bsearch(result, graph);
    }, "Binary Search");

    io::println("Smallest subset is ", n);
    for (Integral::type i = 0; i < graph.v; ++i) {
        io::print(static_cast<int>(result[i]));
    }
    io::println();

    //
    // benchmark::run<1>([&]() -> void {
    //     n = smallest_subset_linear_ascending(result, graph);
    // }, "Linear Ascending Search");
    //
    // io::println("Smallest subset is ", n);
    // for (Integral::type i = 0; i < graph.v; ++i) {
    //     io::print(static_cast<int>(result[i]));
    // }
    // io::println();

    //
    // benchmark::run<1>([&]() -> void {
    //     n = smallest_subset_linear_descending(result, graph);
    // }, "Linear Descending Search");
    //
    // io::println("Smallest subset is ", n);
    // for (Integral::type i = 0; i < graph.v; ++i) {
    //     io::print(static_cast<int>(result[i]));
    // }
    // io::println();

    return 0;
}
