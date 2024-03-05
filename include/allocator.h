#ifndef HPA_2110452_MIN_DOM_SET_MEMORY_H
#define HPA_2110452_MIN_DOM_SET_MEMORY_H

template<typename Tp>
class NewAllocator {
private:
    using value_type = Tp;
    using pointer_type = types::pointer<Tp>;
    using reference = types::reference<value_type>;
    using const_reference = types::const_reference<value_type>;

public:
    FORCE_INLINE static pointer_type allocate(const size_t size = 1) {
        return new value_type[size];
    }

    FORCE_INLINE static void deallocate(pointer_type object) {
        delete[] object;
    }
};

template<typename Tp>
class MallocAllocator {
private:
    using value_type = Tp;
    using pointer_type = types::pointer<Tp>;
    using reference = types::reference<value_type>;
    using const_reference = types::const_reference<value_type>;

public:
    FORCE_INLINE static pointer_type allocate(const size_t size = 1) {
        return static_cast<pointer_type>(malloc(size * sizeof(Tp)));
    }

    FORCE_INLINE static void deallocate(pointer_type object) {
        free(object);
    }
};

#endif //HPA_2110452_MIN_DOM_SET_MEMORY_H
