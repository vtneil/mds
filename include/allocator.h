#ifndef HPA_2110452_MIN_DOM_SET_MEMORY_H
#define HPA_2110452_MIN_DOM_SET_MEMORY_H

#include "types.h"
#include "utils.h"

namespace memory {
    template<template<typename, size_t> class Form, typename Tp, size_t Alignment = sizeof(void *)>
    class Allocator {
    private:
        using pointer_type = types::pointer<Tp>;
        using ConcreteAllocator = Form<Tp, Alignment>;

    public:
        FORCE_INLINE static pointer_type allocate(const size_t size = 1) {
            return ConcreteAllocator::impl_allocate(size);
        }

        FORCE_INLINE static void deallocate(pointer_type object) {
            ConcreteAllocator::impl_deallocate(object);
        }
    };

    template<typename Tp, size_t Alignment = 0>
    class NewAllocator : public Allocator<NewAllocator, Tp, Alignment> {
    private:
        template<template<typename, size_t> class ConcreteAllocator, typename AllocatorTp, size_t AllocatorAlignment>
        friend
        class Allocator;

        using pointer_type = types::pointer<Tp>;

    protected:
        FORCE_INLINE static pointer_type impl_allocate(const size_t size = 1) {
            return new Tp[size];
        }

        FORCE_INLINE static void impl_deallocate(pointer_type object) {
            delete[] object;
        }
    };

    template<typename Tp, size_t Alignment = 0>
    class MallocAllocator : public Allocator<MallocAllocator, Tp, Alignment> {
    private:
        template<template<typename, size_t> class ConcreteAllocator, typename AllocatorTp, size_t AllocatorAlignment>
        friend
        class Allocator;

        using pointer_type = types::pointer<Tp>;

    public:
        FORCE_INLINE static pointer_type impl_allocate(const size_t size = 1) {
            return static_cast<pointer_type>(malloc(size * sizeof(Tp)));
        }

        FORCE_INLINE static void impl_deallocate(pointer_type object) {
            free(object);
        }
    };

    template<typename Tp, size_t Alignment = sizeof(void *)>
    class AlignedAllocator : public Allocator<AlignedAllocator, Tp, Alignment> {
    private:
        template<template<typename, size_t> class ConcreteAllocator, typename AllocatorTp, size_t AllocatorAlignment>
        friend
        class Allocator;

        using pointer_type = types::pointer<Tp>;

    public:
        FORCE_INLINE static pointer_type impl_allocate(const size_t size = 1) {
            static_assert(false, "Not implemented");
            // todo: Implement aligned allocation
            return static_cast<pointer_type>(malloc(size * sizeof(Tp)));
        }

        FORCE_INLINE static void impl_deallocate(pointer_type object) {
            static_assert(false, "Not implemented");
            free(object);
        }
    };

    template<typename Tp, size_t Alignment = 0>
    class UnusedAllocator : public Allocator<UnusedAllocator, Tp, Alignment> {
    private:
        template<template<typename, size_t> class ConcreteAllocator, typename AllocatorTp, size_t AllocatorAlignment>
        friend
        class Allocator;

        using pointer_type = types::pointer<Tp>;

    public:
        template<typename ...Args>
        FORCE_INLINE static pointer_type impl_allocate(Args...) {
            static_assert(false, "This allocator should not be used in the program.");
            UNREACHABLE();
        }

        template<typename ...Args>
        FORCE_INLINE static void impl_deallocate(Args...) {
            static_assert(false, "This allocator should not be used in the program.");
        }
    };

    /**
     * A virtual stack memory region for extended fast stack-like allocation
     * using memory pool technique. It should be substantially faster than using malloc/new if
     * the region is pre-allocated for future uses.
     * \n
     * This is just a simple implementation of memory pool designed to be fast.
     * \n
     * Warning: It is the programmer's responsibility to pop the stack by deallocating manually in LIFO order.
     * The region will not check for illegal accesses for the sake of performance only but not safety.
     * \n
     * Also, it is easier to clear the stack if you know that any memory tied to the stack
     * will not be used later in the program.
     *
     * @tparam NumBytes             Number of total bytes to be allocated (will be aligned to alignment)
     * @tparam Alignment            Alignment of allocation/de-allocation (should be >= machine's alignment and
     *                              is a power of 2.)
     * @tparam BaseAllocator        Memory allocator to be used to pre-allocate
     */
    template<size_t NumBytes, size_t Alignment = 8, template<typename, size_t = 0> class BaseAllocator = MallocAllocator>
    class virtual_stack_region_t {
    private:
        using byte_t = uint8_t;
        using pointer_type = types::pointer<byte_t>;
        using ByteAllocator = BaseAllocator<byte_t, Alignment>;

        inline static constexpr size_t SizeRequested = NumBytes;
        inline static constexpr size_t SizeActual = nearest_alignment<byte_t, Alignment>(NumBytes);
        inline static constexpr size_t NumAlignments = SizeActual / Alignment;

        pointer_type bp;
        pointer_type sp;
        pointer_type region_limit;

    public:
        virtual_stack_region_t() : region_limit{ByteAllocator::allocate(SizeActual)} {
            bp = region_limit + SizeActual;
            sp = bp;
        }

        ~virtual_stack_region_t() {
            ByteAllocator::deallocate(region_limit);
        }

        template<typename T>
        [[nodiscard]] types::pointer<T> allocate_ptr(size_t n = 1) noexcept {
            pointer_type nsp = sp - memory::nearest_alignment<T, Alignment>(n);

            // Check for stack overflow
            if (nsp < region_limit) return nullptr;

            sp = nsp;
            return reinterpret_cast<types::pointer<T>>(sp);
        }

        template<typename T>
        [[nodiscard]] types::reference<T> allocate(size_t n = 1) noexcept {
            return *allocate_ptr<T>(n);
        }

        template<typename T>
        [[nodiscard]] types::pointer<T> allocate_ptr_unsafe(size_t n = 1) noexcept {
            sp -= memory::nearest_alignment<T, Alignment>(n);
            return reinterpret_cast<types::pointer<T>>(sp);
        }

        template<typename T>
        [[nodiscard]] types::reference<T> allocate_unsafe(size_t n = 1) noexcept {
            return *allocate_ptr_unsafe<T>(n);
        }

        template<typename T>
        void deallocate(size_t n) noexcept {
            size_t to_decr = memory::nearest_alignment<T, Alignment>(n);

            ASSUME(bp >= sp);

            if (bp - sp < to_decr) {
                clear();
            } else {
                sp += to_decr;
            }
        }

        template<typename T>
        void deallocate_unsafe(size_t n = 1) noexcept {
            sp += memory::nearest_alignment<T, Alignment>(n);
        }

        FORCE_INLINE void clear() noexcept { sp = bp; }

        [[nodiscard]] constexpr size_t size() const noexcept {
            return memory::addressof<byte_t>(*bp) - memory::addressof<byte_t>(*sp);
        }

        [[nodiscard]] constexpr size_t capacity() const noexcept {
            return memory::addressof<byte_t>(*bp) - memory::addressof<byte_t>(*region_limit);
        }

        void info() const noexcept {
            io::println("==================================================");
            io::println("The region alignment is ", Alignment);
            io::println("The number of aligned blocks is ", NumAlignments);
            io::println("The requested size in bytes is ", SizeRequested);
            io::println("The actual size in bytes is ", SizeActual);
            io::println("The base pointer (bp) is at ", memory::addressof(*bp));
            io::println("The stack pointer (sp) is at ", memory::addressof(*sp));
            io::println("The region limit is at ", memory::addressof(*region_limit));
            io::println(
                    "base pointer (bp) - stack pointer (sp) is ",
                    size(),
                    " bytes"
            );
            io::println(
                    "base pointer (bp) - region limit is ",
                    capacity(),
                    " bytes"
            );
            io::println("==================================================");
        }
    };


}

#endif //HPA_2110452_MIN_DOM_SET_MEMORY_H
