#ifndef MACROS_BOOTSTRAP_H
#define MACROS_BOOTSTRAP_H

// C++ Aligned
#define ALIGNED(ALIGNMENT)  __attribute__((aligned(ALIGNMENT)))

// C++ Inline
#define INLINE              __inline

// C++ Force inline
#define FORCE_INLINE        INLINE __attribute__((always_inline))

// C++ Force no inline (precedence over any inlines)
#define NO_INLINE           __attribute__((noinline))

// Call void function and return 0
#define VZ(fn)              [&]() -> int { fn; return 0; }

// Compiler hint to use jump instead of conditional move
#define EXPECT(a, b)    __builtin_expect(a, b)

// Compiler hint to use jump instead of conditional move
#define LIKELY(x)       EXPECT(!!(x), 1)

// Compiler hint to use jump instead of conditional move
#define UNLIKELY(x)     EXPECT(!!(x), 0)


#endif //MACROS_BOOTSTRAP_H
