#ifndef HPA_2110452_MIN_DOM_SET_CONTAINER_H
#define HPA_2110452_MIN_DOM_SET_CONTAINER_H

#include "macros_bootstrap.h"

#if __cplusplus >= 201700L
#define CPP17_CONSTEXPR     constexpr
#else
#define CPP17_CONSTEXPR
#endif

namespace container {
    using size_type = size_t;

    template<typename Tp, size_type Alignment>
    struct ALIGNED(Alignment) aligned_t {
        Tp value;
    };

    template<typename Tp, size_type Size>
    struct array_t {
        using value_type = Tp;
        using reference = value_type &;
        using const_reference = const value_type &;

        // Data (access this way is the fastest way)
        value_type data[Size];

        // Element access
        FORCE_INLINE CPP17_CONSTEXPR reference operator[](size_type index) {
            return data[index];
        }

        FORCE_INLINE constexpr const_reference operator[](size_type index) const {
            return data[index];
        }

        // Size
        FORCE_INLINE constexpr size_type size() const noexcept {
            return Size;
        }

        FORCE_INLINE constexpr size_type length() const noexcept {
            return Size;
        }
    };
}

#endif //HPA_2110452_MIN_DOM_SET_CONTAINER_H
