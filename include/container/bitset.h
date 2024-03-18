#ifndef HPA_2110452_MIN_DOM_SET_BITSET_H
#define HPA_2110452_MIN_DOM_SET_BITSET_H

#include "container/generic.h"
#include "container/array.h"

namespace container {
    enum class bitset_compare : int8_t {
        NO_COVER = 0,
        COVERS,
        COVERED_BY,
    };

    template<size_t Nb, typename WordT = int>
    struct bitset_t {
        using byte_t = int8_t;

        inline static constexpr size_t Alignment = sizeof(WordT);
        inline static constexpr size_t NumBytes = (Nb + 8 - 1) / 8;
        inline static constexpr size_t SizeActual = memory::nearest_alignment<byte_t, Alignment>(NumBytes);
        inline static constexpr size_t NumElements = SizeActual / Alignment;

        size_t size_;
        size_t num_bytes_;
        size_t size_actual_;
        size_t num_elements_;
        container::array_t<WordT, NumElements> data = {};

        constexpr bitset_t() : size_{Nb} {
            num_bytes_ = (size_ + 8 - 1) / 8;
            size_actual_ = memory::nearest_alignment<byte_t, Alignment>(num_bytes_);
            num_elements_ = size_actual_ / Alignment;
        }

        explicit constexpr bitset_t(size_t N) : size_{N} {
            num_bytes_ = (size_ + 8 - 1) / 8;
            size_actual_ = memory::nearest_alignment<byte_t, Alignment>(num_bytes_);
            num_elements_ = size_actual_ / Alignment;
        }

        constexpr bitset_t(const bitset_t &other) = default;

        constexpr bitset_t(bitset_t &&other) noexcept = default;

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

        FORCE_INLINE constexpr void reset() {
            memset(data.data, 0x00, SizeActual);
        }

        FORCE_INLINE constexpr void set() {
            memset(data.data, 0xFF, SizeActual);
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

        bitset_t operator&(const bitset_t &other) const {
            bitset_t dst{*this};
            return (dst &= other);
        }

        bitset_t operator|(const bitset_t &other) const {
            bitset_t dst{*this};
            return (dst |= other);
        }

        /**
         * Compare container, not data.\n
         * Unsafe, but fast.
         *
         * @param other
         * @return
         */
        bool operator==(const bitset_t &other) const {
            for (size_t i = 0; i < NumElements; ++i) {
                if (data[i] != other.data[i]) return false;
            }
            return true;
        }

        bool operator!=(const bitset_t &other) const {
            return !(this->operator==(other));
        }

        [[nodiscard]] constexpr size_t count_container() const noexcept {
            size_t res = 0;

            if CPP17_CONSTEXPR (sizeof(WordT) == sizeof(int) ||
                                sizeof(WordT) == sizeof(short) ||
                                sizeof(WordT) == sizeof(char)) {
                for (size_t i = 0; i < NumElements; ++i) {
                    res += __builtin_popcount(data[i]);
                }
            } else if CPP17_CONSTEXPR (sizeof(WordT) == sizeof(long)) {
                for (size_t i = 0; i < NumElements; ++i) {
                    res += __builtin_popcountl(data[i]);
                }
            } else if CPP17_CONSTEXPR (sizeof(WordT) == sizeof(long long)) {
                for (size_t i = 0; i < NumElements; ++i) {
                    res += __builtin_popcountll(data[i]);
                }
            } else {
                UNREACHABLE();
            }

            return res;
        }

        [[nodiscard]] constexpr bool all() const noexcept {
            if (size_ == size_actual_ * 8) {
                for (size_t i = 0; i < num_elements_; ++i) {
                    if (~data[i] != 0) return false;
                }
            } else {
                for (size_t i = 0; i < num_elements_ - 1; ++i) {
                    if (~data[i] != 0) return false;
                }
                for (size_t i = (num_elements_ - 1) * Alignment * 8; i < size_; ++i) {
                    if (!get(i)) return false;
                }
            }

            return true;
        }

        [[nodiscard]] constexpr bool popcount_all() const noexcept {
            return count_container() == size_;
        }

        [[nodiscard]] constexpr bool padded_fast_all() const noexcept {
            for (size_t i = 0; i < NumElements; ++i) {
                if (~data[i] != 0) return false;
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

        [[nodiscard]] bitset_compare compare(const bitset_t &other) const noexcept {
            bitset_t unionized(this->operator|(other));
            if (unionized == *this)
                return bitset_compare::COVERS;
            if (unionized == other)
                return bitset_compare::COVERED_BY;
            return bitset_compare::NO_COVER;
        }

        // Capacity

        [[nodiscard]] FORCE_INLINE constexpr types::size_type size() const noexcept {
            return size_;
        }

        [[nodiscard]] FORCE_INLINE constexpr types::size_type capacity() const noexcept {
            return Nb;
        }

        void info() const {
            io::print("CONTAINER: ");
            for (size_t i = 0; i < Nb; ++i) {
                io::print(get(i));
            }
            io::println();
            io::print("DATA:      ");
            for (size_t i = 0; i < size_; ++i) {
                io::print(get(i));
            }
            io::println();
        }
    };
}

#endif //HPA_2110452_MIN_DOM_SET_BITSET_H
