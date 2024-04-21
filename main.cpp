#include "lib_hpa.h"
#include "lib_mds.h"

// #define USE_DEBUG

#ifdef USE_DEBUG
#define DEBUG(...)      io::print(__VA_ARGS__)
#define DEBUGLN(...)    io::print(__VA_ARGS__)
#else
#define DEBUG(...)
#define DEBUGLN(...)
#endif

graph_t graph;
edges_t sol;
std::atomic_bool solved(false);


int main(const int argc, char **argv) {
    if (argc < 3)
        return -1;

    // Graph Initialization
    read_graph_from_file(argv[1]);

    // Solver workers (sorted by its potential to finish first)
    std::thread solvers[] = {
        std::thread(
            operations_research::solve_mds<true>, "BOP Solver",
            operations_research::MPSolver::BOP_INTEGER_PROGRAMMING
        ),
        std::thread(
            operations_research::solve_mds<true>, "CBC Solver",
            operations_research::MPSolver::CBC_MIXED_INTEGER_PROGRAMMING
        ),
        std::thread(
            operations_research::solve_mds_with_cp<true>, "CP-SAT Solver"
        ),
        // std::thread(
        //     operations_research::solve_mds<true>, "SCIP Solver",
        //     operations_research::MPSolver::SCIP_MIXED_INTEGER_PROGRAMMING
        // ),
        // std::thread(
        //     operations_research::solve_mds<true>, "SAT Solver",
        //     operations_research::MPSolver::SAT_INTEGER_PROGRAMMING
        // ),
    };

    // Wait for finish and detach threads
    while (!solved.load(std::memory_order_acquire))
        std::this_thread::yield();

    // Detach all threads immediately
    // (guaranteed to stop at some point in time)
    for (auto &solver: solvers)
        solver.detach();

    // Write the result to output file
    std::ofstream file(argv[2]);
    if (!file.is_open())
        return 1;

    for (const vertex_t i: sol)
        file << i;

    file.close();

    std::exit(0);
}
