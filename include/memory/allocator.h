#ifndef HPA_2110452_MIN_DOM_SET_MEMORY_H
#define HPA_2110452_MIN_DOM_SET_MEMORY_H

#include "lib/types.h"
#include "memory/generic.h"

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
        static pointer_type impl_allocate(Args...) {
            static_assert(false, "This allocator should not be used in the program.");
        }

        template<typename ...Args>
        static void impl_deallocate(Args...) {
            static_assert(false, "This allocator should not be used in the program.");
        }
    };
}

#endif //HPA_2110452_MIN_DOM_SET_MEMORY_H
