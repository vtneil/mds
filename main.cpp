#include <iostream>
#include "macros_bootstrap.h"
#include "builtins_bootstrap.h"
#include "utils.h"


int main() {

    int x, y;

    std::cin >> x;

    utils::io::println("x is ", x, builtin::endl);

    if (LIKELY(x > 0)) {
        y = 20;
    } else {
        y = 50;
    }

    return y & 0xFF;
}
