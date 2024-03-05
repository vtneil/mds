#include <immintrin.h>
#include <iostream>
#include "macros_bootstrap.h"
#include "builtins_bootstrap.h"
#include "utils.h"
#include "container.h"
#include "allocator.h"

#define SIZE (4096UL)

using array1d_t = container::array_t<int, SIZE>;
using array2d_t = container::array_t<array1d_t, SIZE>;

int main(int argc, types::pointer<types::pointer<char>> argv) {
    int sum = 0;

    array2d_t array2d;

    io::println(sizeof(array1d_t));
    io::println(sizeof(array2d_t));

    for (types::size_type i = 0; i < SIZE; ++i) {
        for (types::size_type j = 0; j < SIZE; ++j) {
            array2d[i][j] = i + j;
        }
    }

    for (types::size_type i = 0; i < SIZE; ++i) {
        sum += array2d[i].sum();
    }

    io::println(sum);

    return 0;
}
