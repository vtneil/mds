#ifndef HPA_2110452_MIN_DOM_SET_UTILS_H
#define HPA_2110452_MIN_DOM_SET_UTILS_H

#include <iostream>

namespace io {
    constexpr const char *endl = "\n";

    void print() {}

    template<typename T, typename... Ts>
    void print(const T &val, const Ts &... vals) {
        std::cout << val;
        print(vals...);
    }

    inline void println() {
        std::cout << endl;
    }

    template<typename T, typename... Ts>
    void println(const T &val, const Ts &... vals) {
        print(val, vals...);
        println();
    }

    void untie() {

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
