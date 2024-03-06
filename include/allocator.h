#ifndef HPA_2110452_MIN_DOM_SET_MEMORY_H
#define HPA_2110452_MIN_DOM_SET_MEMORY_H

#include "types.h"
#include "utils.h"

namespace memory {
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

    /**
     * A virtual stack memory region for extended fast stack-like allocation
     * using memory pool technique. It should be faster than using malloc/new if
     * the region is pre-allocated for future uses.
     *
     * Warning: It is the programmer's responsibility to pop the stack by deallocating manually in LIFO order.
     * The region will not check for illegal accesses for the sake of performance only but not safety.
     *
     * Also, it is easier to clear the stack if you know that any memory tied to the stack
     * will not be used later in the program.
     *
     * @tparam NumBytes
     * @tparam BaseAllocator
     */
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
        pointer_type region_limit;

    public:
        virtual_stack_region_t() : region_limit{reinterpret_cast<memory_type>(ByteAllocator::allocate(SizeActual))} {
            bp = region_limit + NumAlignments;
            sp = bp;
        }

        ~virtual_stack_region_t() {
            ByteAllocator::deallocate(reinterpret_cast<types::pointer<byte_t>>(region_limit));
        }

        template<typename T>
        [[nodiscard]] types::pointer<T> allocate(size_t n) noexcept {
            pointer_type nsp = sp - memory::nearest_alignment<T, AlignT>(n) / Alignment;

            // Check for stack overflow
            if (nsp <= region_limit) return nullptr;

            sp = nsp;
            return reinterpret_cast<types::pointer<T>>(sp);
        }

        template<typename T>
        [[nodiscard]] types::pointer<T> allocate_unsafe(size_t n) noexcept {
            sp -= memory::nearest_alignment<T, AlignT>(n) / Alignment;
            return reinterpret_cast<types::pointer<T>>(sp);
        }

        template<typename T>
        void deallocate(size_t n) noexcept {
            size_t to_decr = memory::nearest_alignment<T, AlignT>(n) / Alignment;

            ASSUME(bp >= sp);

            if (bp - sp < to_decr) {
                clear();
            } else {
                sp += to_decr;
            }
        }

        template<typename T>
        void deallocate_unsafe(size_t n) noexcept {
            sp += memory::nearest_alignment<T, AlignT>(n) / Alignment;
        }

        FORCE_INLINE void clear() noexcept { sp = bp; }

        [[nodiscard]] constexpr size_t size() const noexcept {
            return memory::addressof<uint8_t>(*bp) - memory::addressof<uint8_t>(*sp);
        }

        [[nodiscard]] constexpr size_t capacity() const noexcept {
            return memory::addressof<uint8_t>(*bp) - memory::addressof<uint8_t>(*region_limit);
        }

        void info() const noexcept {
            io::println("==================================================");
            io::println("The alignment is ", Alignment);
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
