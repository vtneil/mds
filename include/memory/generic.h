#ifndef HPA_2110452_MIN_DOM_SET_MEMORY_UTILS_H
#define HPA_2110452_MIN_DOM_SET_MEMORY_UTILS_H

#include "lib/builtins_bootstrap.h"
#include "lib/types.h"

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

#endif //HPA_2110452_MIN_DOM_SET_MEMORY_UTILS_H
