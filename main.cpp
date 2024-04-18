#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include "ortools/linear_solver/linear_solver.h"
#include "lib_hpa.h"

template<typename Tp>
using vector_t = std::vector<Tp>;
// using vector_t = std::pmr::vector<Tp>;

using vertex_t = int;
using edges_t = vector_t<vertex_t>;
using graph_t = vector_t<edges_t>;

graph_t graph;
edges_t sol;

void read_graph_from_file(types::const_pointer_to_const<char> filename) {
    std::ifstream stream(filename);

    if (!stream.is_open()) {
        return;
    }

    size_t v, e;

    stream >> v;
    stream >> e;

    graph.resize(v);
    for (types::reference<edges_t> edge: graph)
        edge.reserve(v);

    vertex_t v1, v2;

    while (stream >> v1 >> v2) {
        graph[v1].push_back(v2);
        graph[v2].push_back(v1);
    }

    stream.close();
}

std::mutex mtx;
std::condition_variable cv;

namespace operations_research {
    void solve_mds(
        const char *solver_name,
        const MPSolver::OptimizationProblemType backend = MPSolver::CBC_MIXED_INTEGER_PROGRAMMING
    ) noexcept {
        MPSolver solver(solver_name, backend);

        // Guarantee exit after 10 seconds.
        solver.set_time_limit(10000);

        const auto n = static_cast<vertex_t>(graph.size());
        vector_t<const MPVariable *> x(n);

        // Variables
        for (vertex_t i = 0; i < n; ++i)
            x[i] = solver.MakeBoolVar(std::to_string(i));

        // Constraints
        for (vertex_t i = 0; i < n; ++i) {
            types::reference<MPConstraint> constraint = *solver.MakeRowConstraint(1, MPSolver::infinity());
            constraint.SetCoefficient(x[i], 1);

            for (const vertex_t j: graph[i])
                constraint.SetCoefficient(x[j], 1);
        }

        // Objective
        types::reference<MPObjective> objective = *solver.MutableObjective();
        objective.SetOptimizationDirection(false);
        for (vertex_t i = 0; i < n; ++i)
            objective.SetCoefficient(x[i], 1);

        // Solve
        if (solver.Solve() == MPSolver::OPTIMAL) {
            std::lock_guard<std::mutex> lock(mtx);
            cv.notify_one();
            sol.resize(n);
            for (vertex_t i = 0; i < n; ++i)
                sol[i] = static_cast<int>(x[i]->solution_value());
        }
    }
}

int main(const int argc, char **argv) {
    if (argc < 3)
        return -1;

    // Graph Initialization
    read_graph_from_file(argv[1]);

    // Solver workers
    std::thread solvers[] = {
        std::thread(
            operations_research::solve_mds, "BOP Solver",
            operations_research::MPSolver::BOP_INTEGER_PROGRAMMING
        ),
        std::thread(
            operations_research::solve_mds, "CBC Solver",
            operations_research::MPSolver::CBC_MIXED_INTEGER_PROGRAMMING
        ),
        std::thread(
            operations_research::solve_mds, "SAT Solver",
            operations_research::MPSolver::SAT_INTEGER_PROGRAMMING
        ),
        std::thread(
            operations_research::solve_mds, "SCIP Solver",
            operations_research::MPSolver::SCIP_MIXED_INTEGER_PROGRAMMING
        )
    };

    // Wait for finish and detach threads
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock);
    }

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

    return 0;
}
