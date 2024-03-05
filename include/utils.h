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

namespace memory {
    template<typename R = void, typename Tp>
    FORCE_INLINE constexpr auto addressof(types::reference<Tp> value) {
        return reinterpret_cast<types::pointer<R>>(&value);
    }

    template<typename R = void, typename Tp>
    FORCE_INLINE constexpr auto addressof(types::const_reference<Tp> value) {
        return reinterpret_cast<types::pointer<const R>>(&value);
    }

    template<typename Tp, size_t Alignment>
    FORCE_INLINE constexpr size_t nearest_alignment(size_t n) {
        return (((n * sizeof(Tp)) + Alignment - 1) / Alignment) * Alignment;
    }

    template<typename Tp, typename AlignT>
    FORCE_INLINE constexpr size_t nearest_alignment(size_t n) {
        return nearest_alignment<Tp, sizeof(AlignT)>(n);
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
