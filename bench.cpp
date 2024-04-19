#include "lib_hpa.h"
#include "lib_mds.h"

graph_t graph;
edges_t sol;
std::mutex mtx;
std::condition_variable cv;

int main(const int argc, types::const_pointer_to_const<types::const_pointer_to_const<char> > argv) {
    if (argc < 2)
        return -1;

    // IO
    io::unsync_stdio();

    io::println(argv[1]);

    // Graph Initialization
    benchmark::run<1>([&]() -> void {
        read_graph_from_file(argv[1]);
    }, "I/O & Graph Initialization");

    edges_t solutions[] = {
        benchmark::run<1>([&]() {
            operations_research::solve_mds<false>(
                "BOP Solver",
                operations_research::MPSolver::BOP_INTEGER_PROGRAMMING
            );
            return edges_t(sol);
        }, "BOP Backend"),

        benchmark::run<1>([&]() {
            operations_research::solve_mds<false>(
                "CBC Solver",
                operations_research::MPSolver::CBC_MIXED_INTEGER_PROGRAMMING
            );
            return edges_t(sol);
        }, "CBC Backend"),

        benchmark::run<1>([&]() {
            operations_research::solve_mds<false>(
                "SCIP Solver",
                operations_research::MPSolver::SCIP_MIXED_INTEGER_PROGRAMMING
            );
            return edges_t(sol);
        }, "SCIP Backend"),

        benchmark::run<1>([&]() {
            operations_research::solve_mds<false>(
                "SAT Solver",
                operations_research::MPSolver::SAT_INTEGER_PROGRAMMING
            );
            return edges_t(sol);
        }, "SAT Backend"),

        benchmark::run<1>([&]() {
            operations_research::solve_mds_with_cp<false>(
                "CP-SAT Solver"
            );
            return edges_t(sol);
        }, "CP-SAT Model"),
    };

    // for (const auto &solution: solutions) {
    //     size_t count = 0;
    //     for (const vertex_t i: solution) count += i;
    //     io::println("Smallest subset is ", count);
    //
    //     for (const vertex_t i: solution) io::print(i);
    //     io::println();
    // }

    return 0;
}
