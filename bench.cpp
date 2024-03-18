#include <iostream>
#include "lib_hpa.h"

int main() {
    using bs = container::bitset_t<32>;

    bs b1(25);

    b1.set();
    b1.set(3, false);
    b1.set(5, false);

    b1.info();

    bs b2(b1);
    b2.info();

    return 0;
}


