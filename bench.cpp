#include <vector>
#include "ortools/linear_solver/linear_solver.h"
#include "lib_hpa.h"

template<typename Tp>
using vector_t = std::vector<Tp>;
// using vector_t = std::pmr::vector<Tp>;

using vertex_t = int;
using edges_t = vector_t<vertex_t>;
using graph_t = vector_t<edges_t>;

graph_t createLineGraph(const int N) {
    graph_t graph_line(N);

    for (int i = 0; i < N; ++i) {
        graph_line[i].reserve(N);
    }

    for (int i = 0; i < N; ++i) {
        if (i == 0) {
            graph_line[i].push_back(i + 1);
        } else if (i == N - 1) {
            graph_line[i].push_back(i - 1);
        } else {
            graph_line[i].push_back(i - 1);
            graph_line[i].push_back(i + 1);
        }
    }

    return graph_line;
}

graph_t read_graph_from_file(types::const_pointer_to_const<char> filename) {
    std::ifstream stream(filename);

    if (!stream.is_open()) {
        return {};
    }

    graph_t graph;
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

    return graph;
}

namespace operations_research {
    edges_t solve_mds(
        const graph_t &g,
        const MPSolver::OptimizationProblemType backend = MPSolver::CBC_MIXED_INTEGER_PROGRAMMING
    ) noexcept {
        MPSolver solver("MDS Solver", backend);

        const auto n = static_cast<vertex_t>(g.size());
        vector_t<const MPVariable *> x(n);
        vector_t<int> result(n);

        // Variables
        for (vertex_t i = 0; i < n; ++i) {
            x[i] = solver.MakeBoolVar(std::to_string(i));
        }

        // Constraints
        for (vertex_t i = 0; i < n; ++i) {
            types::reference<MPConstraint> constraint = *solver.MakeRowConstraint(1, MPSolver::infinity());
            constraint.SetCoefficient(x[i], 1);

            for (const vertex_t j: g[i])
                constraint.SetCoefficient(x[j], 1);
        }

        // Objective
        types::reference<MPObjective> objective = *solver.MutableObjective();
        for (vertex_t i = 0; i < n; ++i) {
            objective.SetCoefficient(x[i], 1);
        }
        objective.SetOptimizationDirection(false);

        // Solve
        if (const MPSolver::ResultStatus status = solver.Solve(); status != MPSolver::OPTIMAL)
            return {};

        for (vertex_t i = 0; i < n; ++i) {
            result[i] = static_cast<int>(x[i]->solution_value());
        }
        return result;
    }
}

int main(const int argc, types::const_pointer_to_const<types::const_pointer_to_const<char> > argv) {
    if (argc < 2)
        return -1;

    // IO
    io::unsync_stdio();

    const graph_t graph = benchmark::run<1>([&]() -> graph_t {
        return read_graph_from_file(argv[1]);
    }, "I/O & Graph Initialization");

    edges_t solutions[] = {
        benchmark::run<1>([&]() -> edges_t {
            return operations_research::solve_mds(
                graph,
                operations_research::MPSolver::BOP_INTEGER_PROGRAMMING
            );
        }, "BOP Backend"),
        benchmark::run<1>([&]() -> edges_t {
            return operations_research::solve_mds(
                graph,
                operations_research::MPSolver::CBC_MIXED_INTEGER_PROGRAMMING
            );
        }, "CBC Backend"),
        benchmark::run<1>([&]() -> edges_t {
            return operations_research::solve_mds(
                graph,
                operations_research::MPSolver::SCIP_MIXED_INTEGER_PROGRAMMING
            );
        }, "SCIP Backend"),
        benchmark::run<1>([&]() -> edges_t {
            return operations_research::solve_mds(
                graph,
                operations_research::MPSolver::SAT_INTEGER_PROGRAMMING
            );
        }, "SAT Backend"),
    };

    for (const auto &solution: solutions) {
        size_t count = 0;
        for (const vertex_t i: solution) count += i;
        io::println("Smallest subset is ", count);

        for (const vertex_t i: solution) io::print(i);
        io::println();
    }

    return 0;
}
