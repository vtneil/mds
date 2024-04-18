#ifndef HPA_2110452_MIN_DOM_SET_BENCHMARK_H
#define HPA_2110452_MIN_DOM_SET_BENCHMARK_H

#include <chrono>
#include <utility>

namespace benchmark {
    template<size_t Trials = 1, typename Proc>
    auto measure(Proc &&procedure) {
        static_assert(Trials > static_cast<size_t>(0), "Number of trials cannot be zero!");
        const auto start = std::chrono::high_resolution_clock::now();
        using ReturnType = std::invoke_result_t<Proc>;
        if constexpr (!std::is_same_v<ReturnType, void>) {
            ReturnType ret;
            for (size_t i = 0; i < Trials - 1; ++i) {
                (void) procedure();
            }
            ret = procedure();
            const auto stop = std::chrono::high_resolution_clock::now();
            const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
            return std::make_pair(ret, static_cast<double>(duration.count()) / static_cast<double>(Trials));
        } else {
            for (size_t i = 0; i < Trials; ++i) {
                procedure();
            }
            const auto stop = std::chrono::high_resolution_clock::now();
            const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
            return static_cast<double>(duration.count()) / static_cast<double>(Trials);
        }
    }

    template<size_t Trials = 1, typename Proc>
    auto run(Proc &&procedure, const char *procedure_name = "Untitled") {
        if constexpr (
            auto result = measure<Trials>(std::forward<Proc>(procedure));
            types::is_same<decltype(result), double>::value
        ) {
            io::println("Time taken for procedure \"", procedure_name, "\": ", result, " microseconds.");
            io::flush();
        } else {
            io::println("Time taken for procedure \"", procedure_name, "\": ", std::get<1>(result), " microseconds.");
            io::flush();
            return std::get<0>(result);
        }
    }
}

#endif //HPA_2110452_MIN_DOM_SET_BENCHMARK_H
