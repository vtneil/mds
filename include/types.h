#ifndef HPA_2110452_MIN_DOM_SET_TYPES_H
#define HPA_2110452_MIN_DOM_SET_TYPES_H

#include <cstdint>
#include <cstdlib>
#include <type_traits>
#include "builtins_bootstrap.h"
#include "ported_std.h"

namespace types {
    using size_type = size_t;

    template<typename Tp>
    FORCE_INLINE constexpr bool is_const() { return ported::_impl::i_is_const<Tp>::value; }

    template<typename Tp, size_type Alignment>
    struct ALIGNED(Alignment) aligned_t {
        Tp value;
    };

    template<typename Tp, size_type Size>
    using static_array = Tp[Size];

    template<typename Tp>
    using pointer = Tp *;

    template<typename Tp>
    using pointer_to_const = const Tp *;

    template<typename Tp>
    using const_pointer = Tp *const;

    template<typename Tp>
    using const_pointer_to_const = const Tp *const;

    template<typename Tp>
    using restrict_pointer = Tp *RESTRICT;

    template<typename Tp>
    using restrict_const_pointer = Tp *const RESTRICT;

    template<typename Tp>
    using restrict_const_pointer_to_const = const Tp *const RESTRICT;

    template<typename Tp>
    using reference = Tp &;

    template<typename Tp>
    using const_reference = const Tp &;

    template<typename Tp>
    struct remove_reference {
        using type = Tp;
    };

    template<typename Tp>
    struct remove_reference<Tp &> {
        using type = Tp;
    };

    template<typename Tp>
    struct remove_reference<Tp &&> {
        using type = Tp;
    };

    template<typename Tp>
    using remove_reference_t = typename remove_reference<Tp>::type;

    template<typename Tp>
    struct use_pass_value {
        inline constexpr static bool value = sizeof(Tp) <= sizeof(void *) || std::is_fundamental<Tp>::value;
    };

    template<typename Tp>
    struct use_const_reference {
        inline constexpr static bool value = !use_pass_value<Tp>::value;
    };

    template<typename Tp>
    using copy_param_t = remove_reference_t<Tp>;

    template<typename Tp>
    using const_param_t = logical::conditional<use_const_reference<Tp>::value, const Tp &, Tp>;

    template<typename Tp>
    struct remove_const {
        using type = Tp;
    };
    template<typename Tp>
    struct remove_const<const Tp> {
        using type = Tp;
    };

    template<typename Tp>
    using remove_const_t = typename remove_const<Tp>::type;

    template<typename Tp>
    struct remove_volatile {
        using type = Tp;
    };
    template<typename Tp>
    struct remove_volatile<volatile Tp> {
        using type = Tp;
    };

    template<typename Tp>
    using remove_volatile_t = typename remove_volatile<Tp>::type;

    template<typename Tp>
    struct remove_cv {
        using type = Tp;
    };

    template<typename Tp>
    struct remove_cv<const Tp> {
        using type = Tp;
    };

    template<typename Tp>
    struct remove_cv<volatile Tp> {
        using type = Tp;
    };

    template<typename Tp>
    struct remove_cv<const volatile Tp> {
        using type = Tp;
    };

    template<typename Tp>
    using remove_cv_t = typename remove_cv<Tp>::type;

    template<typename Tp1, typename Tp2>
    struct is_same : public ported::false_type {
        inline static constexpr bool value = false;
    };

    template<typename Tp>
    struct is_same<Tp, Tp> : public ported::true_type {
        inline static constexpr bool value = true;
    };

    template<bool Cond, typename Tp = void>
    struct enable_if {
    };

    template<typename Tp>
    struct enable_if<true, Tp> {
        using type = Tp;
    };

    template<bool Cond, typename Tp = void>
    using enable_if_t = typename enable_if<Cond, Tp>::type;
}

typedef int bar;

template<class T>
struct is_bar {
    template<class Q = T>
    typename std::enable_if<std::is_same<Q, bar>::value, bool>::type check() {
        return true;
    }

    template<class Q = T>
    typename std::enable_if<!std::is_same<Q, bar>::value, bool>::type check() {
        return false;
    }
};

#endif //HPA_2110452_MIN_DOM_SET_TYPES_H
