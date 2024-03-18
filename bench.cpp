#include <iostream>
#include "lib_hpa.h"

int main() {

    container::array_t<int, 32> comb_bitmask = {};
    int k = 3;
    int v = 6;

    comb_bitmask.fill(1, 0, k);
    comb_bitmask.fill(0, k, v);

    do {
        io::println(comb_bitmask);
    } while (std::prev_permutation(comb_bitmask.data, comb_bitmask.data + v));

    return 0;
}


