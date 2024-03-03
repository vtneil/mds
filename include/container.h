#ifndef HPA_2110452_MIN_DOM_SET_CONTAINER_H
#define HPA_2110452_MIN_DOM_SET_CONTAINER_H

#include "macros_bootstrap.h"

#if __cplusplus >= 201700L
#define CPP17_CONSTEXPR     constexpr
#else
#define CPP17_CONSTEXPR
#endif

#include <type_traits>

namespace types {
    using size_type = size_t;

    template<typename Tp, size_type Alignment>
    struct ALIGNED(Alignment) aligned_t {
        Tp value;
    };

    template<typename Tp>
    using ptr = Tp *;

    template<typename Tp>
    using const_ptr = Tp *const;

    template<typename Tp>
    using restrict_ptr = Tp *RESTRICT;

    template<typename Tp>
    using restrict_const_ptr = Tp *const RESTRICT;

    template<typename Tp>
    using reference = Tp &;

    template<typename Tp>
    using const_reference = const Tp &;

    namespace _impl {
        template<typename Tp>
        struct is_pass_value {
            constexpr static bool value = sizeof(Tp) <= sizeof(void *) || std::is_fundamental<Tp>::value;
        };

        template<typename Tp>
        struct is_const_reference {
            constexpr static bool value = !is_pass_value<Tp>::value;
        };
    }

    template<typename Tp>
    constexpr bool use_const_reference() { return _impl::is_const_reference<Tp>::value; }

    template<typename Tp>
    using const_param = typename std::conditional<
            use_const_reference<Tp>(), const Tp &, Tp
    >::type;
}

namespace container {
    template<typename Tp, types::size_type Size>
    struct array_t {
        using value_type = Tp;
        using array_type = Tp[Size];
        using reference = types::reference<value_type>;
        using const_reference = types::const_reference<value_type>;

        // Data (access this way is the fastest way)
        array_type data;

        // Element access
        FORCE_INLINE CPP17_CONSTEXPR reference operator[](types::size_type index) noexcept {
            return data[index];
        }

        FORCE_INLINE constexpr const_reference operator[](types::size_type index) const noexcept {
            return data[index];
        }

        // Size
        FORCE_INLINE constexpr types::size_type size() const noexcept {
            return Size;
        }

        FORCE_INLINE constexpr types::size_type length() const noexcept {
            return Size;
        }
    };
}

#endif //HPA_2110452_MIN_DOM_SET_CONTAINER_H
