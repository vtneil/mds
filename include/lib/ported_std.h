#ifndef HPA_2110452_MIN_DOM_SET_PORTED_STD_H
#define HPA_2110452_MIN_DOM_SET_PORTED_STD_H

namespace ported {
    template<typename Tp, Tp v>
    struct integral_constant {
        static constexpr const Tp value = v;
        typedef Tp value_type;
        typedef integral_constant type;

        explicit inline constexpr operator value_type() const noexcept { return value; }
    };

    typedef integral_constant<bool, true> true_type;

    typedef integral_constant<bool, false> false_type;

    namespace _impl {
        template<typename Tp>
        struct i_is_const : public false_type {
        };

        template<typename Tp>
        struct i_is_const<const Tp> : public true_type {
        };
    }
}

namespace logical {
    namespace _impl {
        template<bool Cond, typename Iftrue, typename Iffalse>
        struct i_conditional {
            using type = Iftrue;
        };

        template<typename Iftrue, typename Iffalse>
        struct i_conditional<false, Iftrue, Iffalse> {
            using type = Iffalse;
        };

        template<bool Cond1, bool ...Conds>
        struct i_or {
            inline static constexpr const bool value = Cond1 || i_or<Conds...>::value;
        };

        template<bool Cond>
        struct i_or<Cond> {
            inline static constexpr const bool value = Cond;
        };

        template<bool Cond1, bool ...Conds>
        struct i_and {
            inline static constexpr const bool value = Cond1 && i_or<Conds...>::value;
        };

        template<bool Cond>
        struct i_and<Cond> {
            inline static constexpr const bool value = Cond;
        };
    }

    template<bool Cond, typename Iftrue, typename Iffalse>
    using conditional = typename _impl::i_conditional<Cond, Iftrue, Iffalse>::type;

    template<bool ...Conds>
    FORCE_INLINE constexpr const bool static_or() {
        return _impl::i_or<Conds...>::value;
    }

    template<bool ...Conds>
    FORCE_INLINE constexpr const bool static_and() {
        return _impl::i_and<Conds...>::value;
    }
}

#endif //HPA_2110452_MIN_DOM_SET_PORTED_STD_H
