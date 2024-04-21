#include "lib_hpa.h"
#include "lib_mds.h"

graph_t graph;
edges_t sol;
std::atomic_bool solved(false);

int main(const int argc, char **argv) {
    if (argc < 2)
        return -1;

    // IO
    io::unsync_stdio();

    io::println(argv[1]);

    // Graph Initialization
    benchmark::run<1>([&]() -> void {
        read_graph_from_file(argv[1]);
    }, "I/O & Graph Initialization");

    using sol_t = std::pair<edges_t, std::string>;

    sol_t solutions[] = {
        benchmark::run<1>([&]() -> sol_t {
            operations_research::solve_mds<false>(
                "BOP Solver",
                operations_research::MPSolver::BOP_INTEGER_PROGRAMMING
            );
            return {edges_t(sol), "BOP Solver"};
        }, "BOP Backend"),

        benchmark::run<1>([&]() -> sol_t {
            operations_research::solve_mds<false>(
                "CBC Solver",
                operations_research::MPSolver::CBC_MIXED_INTEGER_PROGRAMMING
            );
            return {edges_t(sol), "CBC Solver"};
        }, "CBC Backend"),

        benchmark::run<1>([&]() -> sol_t {
            operations_research::solve_mds<false>(
                "SCIP Solver",
                operations_research::MPSolver::SCIP_MIXED_INTEGER_PROGRAMMING
            );
            return {edges_t(sol), "SCIP Solver"};
        }, "SCIP Backend"),

        benchmark::run<1>([&]() -> sol_t {
            operations_research::solve_mds<false>(
                "SAT Solver",
                operations_research::MPSolver::SAT_INTEGER_PROGRAMMING
            );
            return {edges_t(sol), "SAT Solver"};
        }, "SAT Backend"),

        benchmark::run<1>([&]() -> sol_t {
            operations_research::solve_mds_with_cp<false>(
                "CP-SAT Solver"
            );
            return {edges_t(sol), "CP-SAT Solver"};
        }, "CP-SAT Model"),
    };

    for (const auto &[solution, name]: solutions) {
        io::println("Using ", name);
        size_t count = 0;
        for (const vertex_t i: solution) count += i;
        io::println("Solution: ", count);

        io::print("Solution: ");
        for (const vertex_t i: solution) io::print(i);
        io::println();
    }

    return 0;
}
