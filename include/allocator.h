#ifndef HPA_2110452_MIN_DOM_SET_MEMORY_H
#define HPA_2110452_MIN_DOM_SET_MEMORY_H

#include "types.h"
#include "utils.h"

template<typename Tp>
class NewAllocator {
private:
    using pointer_type = types::pointer<Tp>;

public:
    FORCE_INLINE static pointer_type allocate(const size_t size = 1) {
        return new Tp[size];
    }

    FORCE_INLINE static void deallocate(pointer_type object) {
        delete[] object;
    }
};

template<typename Tp>
class MallocAllocator {
private:
    using pointer_type = types::pointer<Tp>;

public:
    FORCE_INLINE static pointer_type allocate(const size_t size = 1) {
        return static_cast<pointer_type>(malloc(size * sizeof(Tp)));
    }

    FORCE_INLINE static void deallocate(pointer_type object) {
        free(object);
    }
};

namespace memory {

    // Warning: It is the programmer's responsibility to pop the stack by deallocating manually in LIFO order.
    template<size_t NumBytes, template<typename> class BaseAllocator = MallocAllocator>
    class virtual_stack_region_t {
    private:
        using byte_t = uint8_t;
        using AlignT = types::pointer<size_t>;  // Always aligns to size_t
        using pointer_type = AlignT;
        using memory_type = pointer_type;
        using ByteAllocator = BaseAllocator<byte_t>;

        inline static constexpr size_t Alignment = sizeof(AlignT);
        inline static constexpr size_t SizeRequested = NumBytes;
        inline static constexpr size_t SizeActual = nearest_alignment<byte_t, AlignT>(NumBytes);
        inline static constexpr size_t NumAlignments = SizeActual / Alignment;

        pointer_type bp;
        pointer_type sp;
        pointer_type region_ceil;

    public:
        virtual_stack_region_t() : bp{reinterpret_cast<memory_type>(ByteAllocator::allocate(SizeActual))} {
            sp = bp;
            region_ceil = bp + NumAlignments;
        }

        ~virtual_stack_region_t() {
            ByteAllocator::deallocate(reinterpret_cast<types::pointer<byte_t>>(bp));
        }

        template<typename T>
        [[nodiscard]] types::pointer<T> allocate(size_t n) noexcept {
            pointer_type nsp = sp + memory::nearest_alignment<T, AlignT>(n) / Alignment;

            // Check for stack overflow
            if (nsp >= region_ceil) return nullptr;

            pointer_type rsp = sp;
            sp = nsp;
            return reinterpret_cast<types::pointer<T>>(rsp);
        }

        template<typename T>
        [[nodiscard]] types::pointer<T> allocate_unsafe(size_t n) noexcept {
            pointer_type rsp = sp;
            sp += memory::nearest_alignment<T, AlignT>(n) / Alignment;
            return reinterpret_cast<types::pointer<T>>(rsp);
        }

        template<typename T>
        void deallocate(size_t n) noexcept {
            size_t to_decr = memory::nearest_alignment<T, AlignT>(n) / Alignment;

            if (sp - bp < to_decr) {
                clear();
            } else {
                sp -= to_decr;
            }
        }

        template<typename T>
        void deallocate_unsafe(size_t n) noexcept {
            sp -= memory::nearest_alignment<T, AlignT>(n) / Alignment;
        }

        FORCE_INLINE void clear() noexcept { sp = bp; }

        [[nodiscard]] constexpr size_t size() const noexcept {
            return memory::addressof<uint8_t>(*sp) - memory::addressof<uint8_t>(*bp);
        }

        [[nodiscard]] constexpr size_t capacity() const noexcept {
            return memory::addressof<uint8_t>(*region_ceil) - memory::addressof<uint8_t>(*bp);
        }

        void info() const noexcept {
            io::println("==================================================");
            io::println("The alignment is ", Alignment);
            io::println("The requested size in bytes is ", SizeRequested);
            io::println("The actual size in bytes is ", SizeActual);
            io::println("The base pointer (bp) is at ", memory::addressof(*bp));
            io::println("The stack pointer (sp) is at ", memory::addressof(*sp));
            io::println("The ceiling pointer is at ", memory::addressof(*region_ceil));
            io::println(
                    "stack pointer (sp) - base pointer (bp) is ",
                    size(),
                    " bytes"
            );
            io::println(
                    "ceiling pointer - base pointer (bp) is ",
                    capacity(),
                    " bytes"
            );
            io::println("==================================================");
        }
    };
}

#endif //HPA_2110452_MIN_DOM_SET_MEMORY_H
