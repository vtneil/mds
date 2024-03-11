#include <fstream>
#include <algorithm>
#include "lib_hpa.h"

constexpr size_t REGION_SIZE = 64UL * (1024UL * 1024UL);  // Region Size in Bytes
constexpr size_t REGION_ALIGNMENT = 16;  // Alignment in Bytes

using Integral = types::default_aligned_t<int32_t, 16>;
using Bit = int8_t;

static Integral::type num_vertex;
static Integral::type num_lines;
constexpr Integral::type MAX_VERTEX = 128;

using CombBitMask = container::array_t<Bit, MAX_VERTEX>;
using BitMask = container::bitset_t<MAX_VERTEX, int>;
using Graph = container::graph_adjacency_list<Integral, MAX_VERTEX, MAX_VERTEX,
        container::array_t, container::dynamic_array_t, container::array_t,
        memory::NewAllocator>;

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

                // Graph cover is implicitly created during graph construction
                bitset_or(domination, graph.cover[i]);
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

int main(int argc, types::pointer<types::pointer<char>> argv) {
    if (argc < 2)
        return -1;

    io::unsync_stdio();

    auto &graph = fast_region.construct_unsafe<Graph>();
    auto &result = fast_region.construct_unsafe<BitMask>();

    std::ifstream stream(argv[1]);
    if (!stream.is_open()) {
        io::println("Error opening file!");
        return 1;
    }

    benchmark::run_measure<1>([&]() -> void {
        stream >> num_vertex;
        stream >> num_lines;

        Integral::type v1, v2;

        while (stream >> v1 >> v2) {
            graph.push_nodes(container::graph_node<Integral>{v1, {v2}});
            graph.push_nodes(container::graph_node<Integral>{v2, {v1}});
        }
    }, "Initialization");

    stream.close();

    Integral::type n;

    benchmark::run_measure<1>([&]() -> void {
        n = smallest_subset(result, graph);
    }, "Algorithm");

//    io::println("Bitmask: ", result.data);
    io::println("Smallest subset is ", n);

    return 0;
}
