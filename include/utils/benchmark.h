#ifndef HPA_2110452_MIN_DOM_SET_BENCHMARK_H
#define HPA_2110452_MIN_DOM_SET_BENCHMARK_H

#include <chrono>

namespace benchmark {
    template<size_t Trials = 1, typename Proc>
    auto measure(Proc procedure, const char *procedure_name = "") {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < Trials; ++i)
            (void) procedure();
        auto stop = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        auto t = duration.count();
        return t / static_cast<decltype(t)>(Trials);
    }

    template<size_t Trials = 1, typename Proc>
    auto run_measure(Proc procedure, const char *procedure_name = "") {
        auto dt = measure<Trials>(procedure);
        io::println("Time taken for procedure \"", procedure_name, "\": ", dt, " microseconds.");
        std::cout.flush();
    }

    // todo: implement measure for various inputs
    template<typename Proc, typename ...Args>
    void benchmark(Proc procedure, types::const_reference<Args> ...args) {

    }
}

#endif //HPA_2110452_MIN_DOM_SET_BENCHMARK_H
