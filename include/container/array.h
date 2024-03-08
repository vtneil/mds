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
        [[nodiscard]] constexpr Tp sum() const noexcept {
            Tp accumulator{};

            for (types::size_type i = 0; i < Size; ++i) {
                accumulator += derived().data[i];
            }

            return accumulator;
        }

        [[nodiscard]] constexpr Tp max() const noexcept {
            Tp max_val = derived().data[0];

            for (types::size_type i = 1; i < Size; ++i) {
                max_val = derived().data[i] > max_val ? derived().data[i] : max_val;
            }

            return max_val;
        }

        [[nodiscard]] constexpr Tp min() const noexcept {
            Tp min_val = derived().data[0];

            for (types::size_type i = 1; i < Size; ++i) {
                min_val = derived().data[i] < min_val ? derived().data[i] : min_val;
            }

            return min_val;
        }

        [[nodiscard]] constexpr bool all() const noexcept {
            for (types::size_type i = 0; i < Size; ++i) {
                if (!derived().data[i]) return false;
            }

            return true;
        }

        [[nodiscard]] constexpr bool any() const noexcept {
            for (types::size_type i = 0; i < Size; ++i) {
                if (derived().data[i]) return true;
            }

            return false;
        }

        // Clear
        FORCE_INLINE void clear() noexcept {
            for (types::size_type i = 0; i < Size; ++i) {
                derived().data[i] = 0;
            }
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
            return memory::addressof<Tp>(derived().data[0]);
        }

        [[nodiscard]] FORCE_INLINE constexpr types::pointer_to_const<Tp> begin() const noexcept {
            return memory::addressof<Tp>(derived().data[0]);
        }

        [[nodiscard]] FORCE_INLINE CPP17_CONSTEXPR types::pointer<Tp> end() noexcept {
            return memory::addressof<Tp>(derived().data[0]) + Size;
        }

        [[nodiscard]] FORCE_INLINE constexpr types::pointer_to_const<Tp> end() const noexcept {
            return memory::addressof<Tp>(derived().data[0]) + Size;
        }

        // Capacity
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
                os << static_cast<Tp>(arr[i]) << ", ";
            }
            os << static_cast<Tp>(arr[i]) << "}";
            return os;
        }
    };

    /**
     * Static region-allocated data container
     */
    template<typename Tp, types::size_type Size, template<typename, size_t = 0> class = memory::UnusedAllocator>
    struct array_t : public array_container_t<array_t, Tp, Size> {
        types::static_array<Tp, Size> data;
    };


    /**
     * Static heap-allocated data container
     */
    template<typename Tp, types::size_type Size, template<typename, size_t = 0> class BaseAllocator = memory::DefaultAllocator>
    struct heap_array_t : public array_container_t<heap_array_t, Tp, Size> {
        using array_type = array_t<Tp, Size>;
        using array_allocator = BaseAllocator<array_t<Tp, Size>>;

        types::reference<array_type> &data = *array_allocator::allocate();

        ~heap_array_t() {
            array_allocator::deallocate(&data);
        }
    };

    /**
     * Automatic region/heap allocation selection for static data
     * based on data's size in bytes (default: 1 MiB)
     */
    template<typename Tp,
            types::size_type Size,
            template<typename, size_t = 0> class BaseAllocator = memory::DefaultAllocator,
            size_t StackSizeThreshold = 1024UL * 1024UL  // 1 MiB limit
    >
    using auto_array_t = logical::conditional<(Size * sizeof(Tp) > StackSizeThreshold),
            heap_array_t<Tp, Size, BaseAllocator>,
            array_t<Tp, Size, BaseAllocator>
    >;

    template<typename Tp, types::size_type Capacity, typename ArrayContainer>
    struct dynamic_array_container_t {
        ArrayContainer data = {};
        types::size_type idx = {};

        template<typename ...Ts>
        FORCE_INLINE constexpr void push_back(types::const_reference<Ts> ...ts) {
            push_back({ts...});
        }

        FORCE_INLINE constexpr void push_back(std::initializer_list<Tp> ts) {
            for (const auto &t: ts)
                push_back(t);
        }

        FORCE_INLINE constexpr void push_back(types::const_reference<Tp> t) {
            if (idx < Capacity)
                push_back_unsafe(t);
        }

        template<typename ...Ts>
        FORCE_INLINE constexpr void push_back_unsafe(types::const_reference<Ts> ...ts) {
            push_back_unsafe({ts...});
        }

        FORCE_INLINE constexpr void push_back_unsafe(std::initializer_list<Tp> ts) {
            for (const auto &t: ts)
                push_back_unsafe(t);
        }

        FORCE_INLINE constexpr void push_back_unsafe(types::const_reference<Tp> t) {
            data[idx++] = t;
        }

        template<typename ...Ts>
        FORCE_INLINE constexpr void emplace_back(types::const_reference<Ts> ...ts) {
            if (idx < Capacity)
                emplace_back_unsafe(ts...);
        }

        template<typename ...Ts>
        FORCE_INLINE constexpr void emplace_back_unsafe(types::const_reference<Ts> ...ts) {
            push_back_unsafe(Tp{ts...});
        }

        FORCE_INLINE constexpr void pop_back() {
            if (idx > 0)
                pop_back_unsafe();
        }

        FORCE_INLINE constexpr void pop_back_unsafe() {
            --idx;
        }

        // Element access
        [[nodiscard]] FORCE_INLINE CPP17_CONSTEXPR types::reference<Tp>
        operator[](types::size_type index) noexcept {
            return data[index];
        }

        [[nodiscard]] FORCE_INLINE constexpr types::const_reference<Tp>
        operator[](types::size_type index) const noexcept {
            return data[index];
        }

        // Iterator
        [[nodiscard]] FORCE_INLINE CPP17_CONSTEXPR types::pointer<Tp> begin() noexcept {
            return memory::addressof<Tp>(data[0]);
        }

        [[nodiscard]] FORCE_INLINE constexpr types::pointer_to_const<Tp> begin() const noexcept {
            return memory::addressof<Tp>(data[0]);
        }

        [[nodiscard]] FORCE_INLINE CPP17_CONSTEXPR types::pointer<Tp> end() noexcept {
            return memory::addressof<Tp>(data[0]) + idx;
        }

        [[nodiscard]] FORCE_INLINE constexpr types::pointer_to_const<Tp> end() const noexcept {
            return memory::addressof<Tp>(data[0]) + idx;
        }

        // Capacity
        [[nodiscard]] FORCE_INLINE constexpr types::size_type capacity() const noexcept {
            return Capacity;
        }

        [[nodiscard]] FORCE_INLINE constexpr types::size_type size() const noexcept {
            return idx;
        }

        [[nodiscard]] FORCE_INLINE constexpr types::size_type length() const noexcept {
            return size();
        }

        friend std::ostream &
        operator<<(types::reference<std::ostream> os, types::const_reference<dynamic_array_container_t> &arr) {
            os << "{";
            types::size_type i = 0;
            for (; i < arr.size() - 1; ++i) {
                os << static_cast<Tp>(arr[i]) << ", ";
            }
            os << static_cast<Tp>(arr[i]) << "}";
            return os;
        }
    };

    template<typename Tp,
            types::size_type Capacity,
            template<typename, size_t> class ContainerAllocator = memory::UnusedAllocator
    >
    using dynamic_array_t = dynamic_array_container_t<Tp, Capacity, array_t<Tp, Capacity, ContainerAllocator>>;

    template<typename Tp,
            types::size_type Capacity,
            template<typename, size_t> class ContainerAllocator = memory::DefaultAllocator
    >
    using heap_dynamic_array_t = dynamic_array_container_t<Tp, Capacity, heap_array_t<Tp, Capacity, ContainerAllocator>>;

    /**
     * Automatic region/heap allocation selection for dynamic array container with static data
     * based on data's size in bytes (default: 1 MiB)
     */
    template<typename Tp,
            types::size_type Size,
            template<typename, size_t = 0> class BaseAllocator = memory::DefaultAllocator,
            size_t StackSizeThreshold = 1024UL * 1024UL  // 1 MiB limit
    >
    using auto_dynamic_array_t = logical::conditional<(Size * sizeof(Tp) > StackSizeThreshold),
            heap_dynamic_array_t<Tp, Size, BaseAllocator>,
            dynamic_array_t<Tp, Size, BaseAllocator>
    >;
}

#endif //HPA_2110452_MIN_DOM_SET_ARRAY_H
