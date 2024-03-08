#ifndef HPA_2110452_MIN_DOM_SET_BENCHMARK_H
#define HPA_2110452_MIN_DOM_SET_BENCHMARK_H

#include <chrono>

namespace benchmark {
    template<typename Proc>
    auto measure(Proc procedure) {
        auto start = std::chrono::high_resolution_clock::now();
        (void) procedure();
        auto stop = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        return duration.count();
    }

    // todo: implement measure for various inputs
    template<typename Proc, typename ...Args>
    void benchmark(Proc procedure, types::const_reference<Args> ...args) {

    }
}

#endif //HPA_2110452_MIN_DOM_SET_BENCHMARK_H
