#include <iostream>
#include "lib_hpa.h"

int main() {
    int x = 3;
    int y = 3;

    io::println(__builtin_popcount(x));
    io::println(__builtin_popcount(y));

    return 0;
}


