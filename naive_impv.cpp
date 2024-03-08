#include <climits>
#include <algorithm>
#include "lib_hpa.h"

constexpr size_t REGION_SIZE = 512UL * (1024UL * 1024UL);
constexpr size_t REGION_ALIGNMENT = sizeof(void *);

using Integral = int32_t;

constexpr Integral deg = 4;

using Graph = container::graph_adjacency_list<Integral, deg>;
using Set = container::dynamic_array_t<Integral, deg>;
using BitMap = container::array_t<int_fast32_t, deg>;
using GraphBitMapArr = container::array_t<BitMap, deg>;
using SubsetBitMapArr = container::array_t<BitMap, 1 << deg>;

memory::virtual_stack_region_t<REGION_SIZE, REGION_ALIGNMENT, memory::NewAllocator> glob_region;
memory::virtual_stack_region_t<REGION_SIZE, REGION_ALIGNMENT, memory::NewAllocator> fast_region;

types::pointer<GraphBitMapArr> bmp;

void comb(int N, int K) {
    std::string bitmask(K, 1); // K leading 1's
    bitmask.resize(N, 0); // N-K trailing 0's

    // print integers and permute bitmask
    do {
        for (int i = 0; i < N; ++i) // [0..N-1] integers
        {
            if (!bitmask[i])
                continue;

            std::cout << " " << i;
        }
        std::cout << std::endl;
    } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
}

types::reference<SubsetBitMapArr> generateSubsetBitMap() {
    SubsetBitMapArr &subsets = glob_region.allocate<SubsetBitMapArr>();
    BitMap &bitmask = glob_region.allocate<BitMap>();
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

//            io::println("num ", subset_num, " size ", subset_size, " with ", current_subset, " : ",
//                        subsets[subset_num]);
            ++subset_num;

            fast_region.deallocate<Set>();

        } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
    }

    return subsets;
}

Integral smallest_subset() {
    SubsetBitMapArr &subsets = glob_region.allocate<SubsetBitMapArr>();
    BitMap &bitmask = glob_region.allocate<BitMap>();
    Integral subset_num = 1;

    for (Integral k = 1; k <= deg; ++k) {
        bitmask.fill(1, 0, k);
        bitmask.fill(0, k, deg);

        do {
            Integral subset_size = 0;
            BitMap &domination = fast_region.allocate<BitMap>();

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

            fast_region.deallocate<BitMap>();

        } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
    }

    return 0;
}

int main() {

    Graph graph = {};

    benchmark::run_measure<1>([&]() -> void {
        graph.push_nodes(container::graph_node<Integral>{0, {}},
                         container::graph_node<Integral>{1, {}},
                         container::graph_node<Integral>{2, {3}},
                         container::graph_node<Integral>{3, {2}});
//        for (Integral i = 0; i < deg; ++i) {
//            for (Integral j = 0; j < deg; ++j) {
//                graph.push_nodes(container::graph_node<Integral>{i, {j}});
//            }
//        }
    });

    benchmark::run_measure<1>([&]() -> void {
        bmp = glob_region.allocate_ptr_unsafe<GraphBitMapArr>();
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

    return 0;
}
