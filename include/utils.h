#ifndef HPA_2110452_MIN_DOM_SET_UTILS_H
#define HPA_2110452_MIN_DOM_SET_UTILS_H

#include <iostream>
#include "builtins_bootstrap.h"
#include "types.h"

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
    }

    template<bool Cond>
    FORCE_INLINE constexpr const char *static_str() {
        return "true";
    }

    template<>
    FORCE_INLINE constexpr const char *static_str<false>() {
        return "false";
    }

    FORCE_INLINE const char *to_str(bool value) {
        return value ? "true" : "false";
    }
}

namespace memory {
    template<typename R = void, typename Tp>
    FORCE_INLINE constexpr auto addressof(types::reference<Tp> value) {
        return static_cast<types::pointer<R>>(&value);
    }

    template<typename R = void, typename Tp>
    FORCE_INLINE constexpr auto addressof(types::const_reference<Tp> value) {
        return static_cast<types::pointer<const R>>(&value);
    }
}

namespace bench {
    template<class Tp>
    FORCE_INLINE void no_opt(Tp const &value) {
        asm volatile("" : : "r,m"(value) : "memory");
    }

    template<class Tp>
    FORCE_INLINE void no_opt(Tp &value) {
#if defined(__clang__)
        asm volatile("" : "+r,m"(value) : : "memory");
#else
        asm volatile("" : "+m,r"(value) : : "memory");
#endif
    }
}


#endif //HPA_2110452_MIN_DOM_SET_UTILS_H
