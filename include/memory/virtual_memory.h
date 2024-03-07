#ifndef HPA_2110452_MIN_DOM_SET_VIRTUAL_MEMORY_H
#define HPA_2110452_MIN_DOM_SET_VIRTUAL_MEMORY_H

#include "memory/allocator.h"
#include "utils/io.h"

namespace memory {
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
            return bp - sp;
        }

        [[nodiscard]] constexpr size_t capacity() const noexcept {
            return bp - region_limit;
        }

        [[nodiscard]] constexpr size_t remaining() const noexcept {
            return sp - region_limit;
        }

        void info() const noexcept { std::cout << *this; }

        friend std::ostream &
        operator<<(types::reference<std::ostream> os, types::const_reference<virtual_stack_region_t> &arr) {
            os << "==================================================\n";
            os << "The region alignment is " << Alignment << "\n";
            os << "The number of aligned blocks is " << NumAlignments << "\n";
            os << "The requested size in bytes is " << SizeRequested << "\n";
            os << "The derived size in bytes is " << SizeActual << "\n";
            os << "The base pointer (bp) is at " << memory::addressof(*arr.bp) << "\n";
            os << "The stack pointer (sp) is at " << memory::addressof(*arr.sp) << "\n";
            os << "The region limit is at " << memory::addressof(*arr.region_limit) << "\n";
            os << "Stack max capacity is " << arr.capacity() << " bytes\n";
            os << "Stack size consumed is " << arr.size() << " bytes\n";
            os << "Stack remaining capacity is " << arr.remaining() << " bytes\n";
            os << "==================================================\n";
            return os;
        }
    };
}

#endif //HPA_2110452_MIN_DOM_SET_VIRTUAL_MEMORY_H
