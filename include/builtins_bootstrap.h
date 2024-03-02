#ifndef BUILTINS_BOOTSTRAP_H
#define BUILTINS_BOOTSTRAP_H

#include "macros_bootstrap.h"


namespace builtin {
    FORCE_INLINE __attribute__((noreturn)) void unreachable() {
        __builtin_unreachable();
    }

    FORCE_INLINE void assume(bool statement) {
        __builtin_assume(statement);
    }

    constexpr const char *endl = "\n";
}

#endif //BUILTINS_BOOTSTRAP_H
