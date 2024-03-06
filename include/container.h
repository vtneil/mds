#ifndef HPA_2110452_MIN_DOM_SET_CONTAINER_H
#define HPA_2110452_MIN_DOM_SET_CONTAINER_H

#include "utils.h"
#include "allocator.h"

#if __cplusplus >= 201700L
#define CPP17_CONSTEXPR     constexpr
#else
#define CPP17_CONSTEXPR
#endif

namespace container {
    template<typename Tp, types::size_type Size>
    struct array_t {
        using value_type = Tp;
        using array_type = types::static_array<Tp, Size>;
        using reference = types::reference<value_type>;
        using const_reference = types::const_reference<value_type>;

        // Data (access this way is the fastest way)
        array_type data;

        // Algorithm
        constexpr Tp sum() const noexcept {
            Tp accumulator{};

            for (types::size_type i = 0; i < Size; ++i) {
                accumulator += data[i];
            }

            return accumulator;
        }

        constexpr Tp max() const noexcept {
            Tp max_val{};

            for (types::size_type i = 0; i < Size; ++i) {
                max_val = data[i] > max_val ? data[i] : max_val;
            }

            return max_val;
        }

        constexpr Tp min() const noexcept {
            Tp min_val{};

            for (types::size_type i = 0; i < Size; ++i) {
                min_val = data[i] < min_val ? data[i] : min_val;
            }

            return min_val;
        }

        // Element access
        FORCE_INLINE CPP17_CONSTEXPR reference operator[](types::size_type index) noexcept {
            return data[index];
        }

        FORCE_INLINE constexpr const_reference operator[](types::size_type index) const noexcept {
            return data[index];
        }

        // Size
        FORCE_INLINE constexpr types::size_type capacity() const noexcept {
            return Size;
        }

        FORCE_INLINE constexpr types::size_type size() const noexcept {
            return Size;
        }

        FORCE_INLINE constexpr types::size_type length() const noexcept {
            return Size;
        }
    };

    template<typename Tp, types::size_type Size, template<typename> class BaseAllocator = memory::NewAllocator>
    struct heap_array_t {
    public:
        using value_type = Tp;
        using array_type = array_t<Tp, Size>;
        using reference = types::reference<value_type>;
        using const_reference = types::const_reference<value_type>;

        using array_allocator = BaseAllocator<array_t<Tp, Size>>;

    private:
        types::pointer<array_type> array_ptr;

    public:
        types::reference<array_type> &data = *array_ptr;

        heap_array_t() : array_ptr{array_allocator::allocate()} {}

        ~heap_array_t() {
            array_allocator::deallocate(array_ptr);
        }

        constexpr Tp const sum() { return data.sum(); }

        constexpr Tp const max() { return data.max(); }

        constexpr Tp const min() { return data.min(); }

        // Element access
        FORCE_INLINE CPP17_CONSTEXPR reference operator[](types::size_type index) noexcept {
            return data[index];
        }

        FORCE_INLINE constexpr const_reference operator[](types::size_type index) const noexcept {
            return data[index];
        }

        // Size
        FORCE_INLINE constexpr types::size_type capacity() const noexcept {
            return Size;
        }

        FORCE_INLINE constexpr types::size_type size() const noexcept {
            return Size;
        }

        FORCE_INLINE constexpr types::size_type length() const noexcept {
            return Size;
        }
    };

    template<typename Tp, types::size_type Size>
    struct deque_t {
        using value_type = Tp;
        using reference = types::reference<value_type>;
        using const_reference = types::const_reference<value_type>;

        array_t<Tp, Size> data;
        types::size_type pos_front = {};
        types::size_type pos_back = {};

        // Element access
        FORCE_INLINE CPP17_CONSTEXPR reference operator[](types::size_type index) noexcept {
            return data[index];
        }

        FORCE_INLINE constexpr const_reference operator[](types::size_type index) const noexcept {
            return data[index];
        }

        FORCE_INLINE CPP17_CONSTEXPR reference front() noexcept {
            return data[pos_front];
        }

        FORCE_INLINE constexpr const_reference front() const noexcept {
            return data[pos_front];
        }

        FORCE_INLINE CPP17_CONSTEXPR reference back() noexcept {
            return data[pos_back];
        }

        FORCE_INLINE constexpr const_reference back() const noexcept {
            return data[pos_back];
        }

        // Deque Modification
        // todo: pop_front, pop_back, push_front, push_back
    };
}

#endif //HPA_2110452_MIN_DOM_SET_CONTAINER_H
