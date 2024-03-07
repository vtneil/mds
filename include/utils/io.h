#ifndef HPA_2110452_MIN_DOM_SET_IO_H
#define HPA_2110452_MIN_DOM_SET_IO_H

#include <iostream>
#include "lib/builtins_bootstrap.h"
#include "lib/types.h"

namespace io {
    constexpr const char *endl = "\n";

    inline void print() {}

    template<typename T, typename... Ts>
    inline void print(const T &val, const Ts &... vals) {
        std::cout << val;
        print(vals...);
    }

    inline void println() {
        std::cout << endl;
    }

    template<typename T, typename... Ts>
    inline void println(const T &val, const Ts &... vals) {
        print(val, vals...);
        println();
    }

    FORCE_INLINE void unsync_stdio() {
        std::ios_base::sync_with_stdio(false);
        std::cin.tie(nullptr);
    }

    template<bool Cond>
    FORCE_INLINE constexpr types::pointer_to_const<char> static_str() {
        return "true";
    }

    template<>
    FORCE_INLINE constexpr types::pointer_to_const<char> static_str<false>() {
        return "false";
    }

    FORCE_INLINE constexpr types::pointer_to_const<char> to_str(bool value) {
        return value ? "true" : "false";
    }
}

#endif //HPA_2110452_MIN_DOM_SET_IO_H
