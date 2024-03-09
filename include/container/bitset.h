#ifndef HPA_2110452_MIN_DOM_SET_BITSET_H
#define HPA_2110452_MIN_DOM_SET_BITSET_H

#include "container/generic.h"
#include "container/array.h"

namespace container {

    template<size_t Nb, typename WordT = int>
    struct bitset_t {
        using byte_t = int8_t;

        inline static constexpr size_t Alignment = sizeof(WordT);
        inline static constexpr size_t NumBytes = (Nb + 8 - 1) / 8;
        inline static constexpr size_t SizeActual = memory::nearest_alignment<byte_t, Alignment>(NumBytes);
        inline static constexpr size_t NumElements = SizeActual / Alignment;

        container::array_t<WordT, NumElements> data = {};

        [[nodiscard]] FORCE_INLINE constexpr bool get(size_t index) const {
            size_t idx_word = index / (8 * Alignment);
            size_t idx_bit = index % (8 * Alignment);
            return (data[idx_word] >> idx_bit) & 1;
        }

        FORCE_INLINE constexpr void set(size_t index, bool value) {
            size_t idx_word = index / (8 * Alignment);
            size_t idx_bit = index % (8 * Alignment);

            data[idx_word] &= ~(static_cast<WordT>(1) << idx_bit);
            data[idx_word] |= (static_cast<WordT>(value) << idx_bit);
        }

        FORCE_INLINE constexpr void clear(size_t index) {
            size_t idx_word = index / (8 * Alignment);
            size_t idx_bit = index % (8 * Alignment);

            data[idx_word] &= ~(static_cast<WordT>(1) << idx_bit);
        }

        FORCE_INLINE constexpr void toggle(size_t index) {
            size_t idx_word = index / (8 * Alignment);
            size_t idx_bit = index % (8 * Alignment);

            data[idx_word] ^= (static_cast<WordT>(1) << idx_bit);
        }

        bitset_t &operator&=(const bitset_t &other) {
            for (size_t i = 0; i < NumElements; ++i) {
                data[i] &= other.data[i];
            }
            return *this;
        }

        bitset_t &operator|=(const bitset_t &other) {
            for (size_t i = 0; i < NumElements; ++i) {
                data[i] |= other.data[i];
            }
            return *this;
        }

        bitset_t operator&(const bitset_t &other) {
            bitset_t dst{*this};
            return (dst &= other);
        }

        bitset_t operator|(const bitset_t &other) {
            bitset_t dst{*this};
            return (dst |= other);
        }

        [[nodiscard]] constexpr bool all() const noexcept {
            if CPP17_CONSTEXPR (Nb == SizeActual * 8) {
                for (size_t i = 0; i < NumElements; ++i) {
                    if (~data[i] != 0) return false;
                }
            } else {
                for (size_t i = 0; i < NumElements - 1; ++i) {
                    if (~data[i] != 0) return false;
                }
                for (size_t i = (NumElements - 1) * Alignment * 8; i < Nb; ++i) {
                    if (!get(i)) return false;
                }
            }

            return true;
        }

        [[nodiscard]] constexpr bool any() const noexcept {
            for (size_t i = 0; i < NumElements; ++i) {
                if (data[i]) return true;
            }

            return false;
        }

        [[nodiscard]] constexpr bool none() const noexcept {
            return !any();
        }

        [[nodiscard]] FORCE_INLINE constexpr types::size_type size() const noexcept {
            return Nb;
        }
    };
}

#endif //HPA_2110452_MIN_DOM_SET_BITSET_H
