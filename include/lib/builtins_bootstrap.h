#ifndef BUILTINS_BOOTSTRAP_H
#define BUILTINS_BOOTSTRAP_H

#include "lib/macros_bootstrap.h"


namespace builtin {
    inline unsigned long long next_power_of_two(const unsigned long long x) {
        if (x == 0) return 1;
        if ((x & (x - 1)) == 0) return x;
        return 1ull << (8ull * sizeof(unsigned long long) - __builtin_clzll(x));
    }
}

#endif //BUILTINS_BOOTSTRAP_H
