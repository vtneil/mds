#ifndef HPA_2110452_MIN_DOM_SET_DEQUE_H
#define HPA_2110452_MIN_DOM_SET_DEQUE_H

#include "container/generic.h"
#include "container/array.h"

namespace container {
    template<typename Tp, types::size_type Size, template<typename, size_t = 0> class = memory::UnusedAllocator>
    struct deque_t : public array_container_t<deque_t, Tp, Size> {
        using value_type = Tp;
        using reference = types::reference<value_type>;
        using const_reference = types::const_reference<value_type>;

        array_t<Tp, Size> data;
        types::size_type n_elements = {};
        types::size_type pos_front = {};
        types::size_type pos_back = {};

        // Extended element access
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

        // Capacity
        // Override
        [[nodiscard]] FORCE_INLINE constexpr types::size_type size() const noexcept {
            return n_elements;
        }
    };
}

#endif //HPA_2110452_MIN_DOM_SET_DEQUE_H
