#ifndef HPA_2110452_MIN_DOM_SET_ARRAY_H
#define HPA_2110452_MIN_DOM_SET_ARRAY_H

#include "container/generic.h"
#include "utils/io.h"

namespace container {
    template<template<typename, types::size_type, template<typename, size_t = 0> class> class ArrayForm, typename Tp, types::size_type Size>
    class array_container_t {
    private:
        using ConcreteArray = ArrayForm<Tp, Size, memory::UnusedAllocator>;

        types::reference<ConcreteArray> derived() {
            return static_cast<types::reference<ConcreteArray>>(*this);
        }

        types::const_reference<ConcreteArray> derived() const {
            return static_cast<types::const_reference<ConcreteArray>>(*this);
        }

    public:
        using reference = types::reference<Tp>;
        using const_reference = types::const_reference<Tp>;

        // Algorithm
        constexpr Tp sum() const noexcept {
            Tp accumulator{};

            for (types::size_type i = 0; i < Size; ++i) {
                accumulator += derived().data[i];
            }

            return accumulator;
        }

        constexpr Tp max() const noexcept {
            Tp max_val = derived().data[0];

            for (types::size_type i = 1; i < Size; ++i) {
                max_val = derived().data[i] > max_val ? derived().data[i] : max_val;
            }

            return max_val;
        }

        constexpr Tp min() const noexcept {
            Tp min_val = derived().data[0];

            for (types::size_type i = 1; i < Size; ++i) {
                min_val = derived().data[i] < min_val ? derived().data[i] : min_val;
            }

            return min_val;
        }

        // Element access
        [[nodiscard]] FORCE_INLINE CPP17_CONSTEXPR reference operator[](types::size_type index) noexcept {
            return derived().data[index];
        }

        [[nodiscard]] FORCE_INLINE constexpr const_reference operator[](types::size_type index) const noexcept {
            return derived().data[index];
        }

        // Iterator
        [[nodiscard]] FORCE_INLINE CPP17_CONSTEXPR types::pointer<Tp> begin() noexcept {
            return derived().data;
        }

        [[nodiscard]] FORCE_INLINE constexpr types::pointer_to_const<Tp> begin() const noexcept {
            return derived().data;
        }

        [[nodiscard]] FORCE_INLINE CPP17_CONSTEXPR types::pointer<Tp> end() noexcept {
            return derived().data + Size;
        }

        [[nodiscard]] FORCE_INLINE constexpr types::pointer_to_const<Tp> end() const noexcept {
            return derived().data + Size;
        }

        // Size
        [[nodiscard]] FORCE_INLINE constexpr types::size_type capacity() const noexcept {
            return Size;
        }

        [[nodiscard]] FORCE_INLINE constexpr types::size_type size() const noexcept {
            return Size;
        }

        [[nodiscard]] FORCE_INLINE constexpr types::size_type length() const noexcept {
            return size();
        }

        friend std::ostream &
        operator<<(types::reference<std::ostream> os, types::const_reference<array_container_t> &arr) {
            os << "{";
            types::size_type i = 0;
            for (; i < Size - 1; ++i) {
                os << arr[i] << ", ";
            }
            os << arr[i] << "}";
            return os;
        }
    };

    /**
     * Static stack-allocated array container
     */
    template<typename Tp, types::size_type Size, template<typename, size_t = 0> class = memory::UnusedAllocator>
    struct array_t : public array_container_t<array_t, Tp, Size> {
        types::static_array<Tp, Size> data;
    };


    /**
     * Static heap-allocated array container
     */
    template<typename Tp, types::size_type Size, template<typename, size_t = 0> class BaseAllocator = memory::MallocAllocator>
    struct heap_array_t : public array_container_t<heap_array_t, Tp, Size> {
        using array_type = array_t<Tp, Size>;
        using array_allocator = BaseAllocator<array_t<Tp, Size>>;

        types::reference<array_type> &data = *array_allocator::allocate();

        ~heap_array_t() {
            array_allocator::deallocate(&data);
        }
    };

    /**
     * Automatic stack/heap allocation selection for static array
     * based on array's size in bytes (default: 1 MiB)
     */
    template<typename Tp,
            types::size_type Size,
            template<typename, size_t = 0> class BaseAllocator = memory::MallocAllocator,
            size_t StackSizeThreshold = 1024UL * 1024UL  // 1 MiB limit
    >
    using auto_array_t = logical::conditional<(Size * sizeof(Tp) > StackSizeThreshold),
            heap_array_t<Tp, Size, BaseAllocator>,
            array_t<Tp, Size, BaseAllocator>
    >;
}

#endif //HPA_2110452_MIN_DOM_SET_ARRAY_H
