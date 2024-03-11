#include <fstream>
#include <algorithm>
#include "lib_hpa.h"

constexpr size_t REGION_SIZE = 64UL * (1024UL * 1024UL);  // Region Size in Bytes
constexpr size_t REGION_ALIGNMENT = 16;  // Alignment in Bytes

using Integral = types::default_aligned_t<int32_t, 16>;
using Bit = char;

static Integral::type num_vertex;
static Integral::type num_lines;
constexpr Integral::type MAX_VERTEX = 128;

using CombBitMask = container::array_t<Bit, MAX_VERTEX>;
using BitMask = container::bitset_t<MAX_VERTEX, int>;
using Graph = container::graph_t<Integral, MAX_VERTEX, MAX_VERTEX,
        container::array_t, container::dynamic_array_t, container::array_t,
        memory::NewAllocator>;

memory::virtual_stack_region_t<REGION_SIZE, REGION_ALIGNMENT, memory::NewAllocator> fast_region;


Integral::type smallest_subset(types::reference<CombBitMask> output_mask,
                               types::const_reference<Graph> graph) {
    // Padding to clear usable bitmask area for faster all operation
    alignas(64) BitMask pad_clear(num_vertex);
    pad_clear.fill_all();
    for (Integral::type i = 0; i < num_vertex; ++i)
        pad_clear.set(i, false);

    // Pre-allocation
    alignas(64) BitMask domination(num_vertex);

    CombBitMask comb_bitmask;

    Integral::type ans = -1;
    Integral::type low = 1;
    Integral::type high = num_vertex;
    Integral::type k;

    // Binary search k (combinatorial: num_vertex choose k)
    while (true) {
        if (LIKELY(low > high))
            break;

        Integral::type found = 0;
        k = low + (high - low) / 2;

        comb_bitmask.fill(1, 0, k);
        comb_bitmask.fill(0, k, num_vertex);

        // FOR EACH COMBINATION IN (N, K)
        do {
            domination.clear_all();
            domination |= pad_clear;

            // FOR EACH SUBSET USING COMBINATION (FIND WHERE = 1)
            for (Integral::type i = 0; i < num_vertex; ++i) {
                if (UNLIKELY(comb_bitmask[i]))
                    domination |= graph.matrix[i];
            }

            // Found: mark this and try searching smaller sets
            if (domination.padded_fast_all()) {
                found = 1;
                high = k - 1;

                ans = k;
                output_mask = comb_bitmask;
                break;
            }

        } while (std::prev_permutation(comb_bitmask.data, comb_bitmask.data + num_vertex));

        // Not found: try searching larger sets
        if (!found) {  // 50:50
            low = k + 1;
        }
    }

    return ans;
}

Integral::type smallest_subset_linear(types::reference<CombBitMask> output_mask,
                                      types::const_reference<Graph> graph) {
    alignas(64) BitMask pad_clear(num_vertex);
    pad_clear.fill_all();
    for (Integral::type i = 0; i < num_vertex; ++i)
        pad_clear.set(i, false);

    CombBitMask comb_bitmask;
    alignas(64) BitMask domination(num_vertex);

    // Iterate through all subsets from smallest to largest and return immediately once the subset is dominating.
    for (Integral::type k = 1; k <= num_vertex; ++k) {  // (combinatorial: num_vertex choose k)
        comb_bitmask.fill(1, 0, k);
        comb_bitmask.fill(0, k, num_vertex);

        // FOR EACH COMBINATION (N, K)
        do {
            domination.clear_all();
            domination |= pad_clear;

            // FOR EACH SUBSET USING COMBINATION (FIND WHERE = 1)
            for (Integral::type i = 0; i < num_vertex; ++i) {
                if (UNLIKELY(comb_bitmask[i]))
                    domination |= graph.matrix[i];
            }

            if (LIKELY(domination.all())) {
                output_mask = comb_bitmask;
                return k;
            }

        } while (std::prev_permutation(comb_bitmask.data, comb_bitmask.data + num_vertex));
    }

    return 0;
}

int main(int argc, types::pointer<types::pointer<char>> argv) {
    if (argc < 2)
        return -1;

    // Problem Instance

    auto &graph = fast_region.construct_unsafe<Graph>();
    auto &result = fast_region.construct_unsafe<CombBitMask>();

    // IO

    io::unsync_stdio();

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
    }, "I/O & Graph Initialization");

    stream.close();

    // Algorithm

    Integral::type n;

    benchmark::run_measure<1>([&]() -> void {
        n = smallest_subset(result, graph);
    }, "Binary Search");

    io::println("Smallest subset is ", n);
    for (Integral::type i = 0; i < num_vertex; ++i) {
        io::print(static_cast<int>(result[i]));
    }
    io::println();

    benchmark::run_measure<1>([&]() -> void {
        n = smallest_subset_linear(result, graph);
    }, "Linear Search");

    io::println("Smallest subset is ", n);
    for (Integral::type i = 0; i < num_vertex; ++i) {
        io::print(static_cast<int>(result[i]));
    }
    io::println();

    return 0;
}
