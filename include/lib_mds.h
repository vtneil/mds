#ifndef LIB_MDS_H
#define LIB_MDS_H

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include "ortools/linear_solver/linear_solver.h"
#include "ortools/sat/cp_model.h"
#include "ortools/sat/cp_model_solver.h"

template<typename Tp>
using vector_t = std::vector<Tp>;

using vertex_t = int;
using edges_t = vector_t<vertex_t>;
using graph_t = vector_t<edges_t>;

extern graph_t graph;
extern edges_t sol;
extern std::mutex mtx;
extern std::condition_variable cv;

inline void read_graph_from_file(types::const_pointer_to_const<char> filename) {
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

namespace operations_research {
    template<bool with_lock = true>
    void solve_mds(
        const char *solver_name,
        const MPSolver::OptimizationProblemType backend = MPSolver::CBC_MIXED_INTEGER_PROGRAMMING
    ) noexcept {
        MPSolver solver(solver_name, backend);

        // Guarantees exit after 4 minutes 50 seconds.
        solver.set_time_limit(1000ull * (4ull * 60ull + 50ull));

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
            if constexpr (with_lock) {
                std::lock_guard<std::mutex> lock(mtx);
                cv.notify_one();
            }
            sol.resize(n);
            for (vertex_t i = 0; i < n; ++i)
                sol[i] = static_cast<int>(x[i]->solution_value());
        }
    }

    template<bool with_lock = true>
    void solve_mds_with_cp(
        const char *solver_name
    ) noexcept {
        sat::CpModelBuilder cp_model;
        cp_model.SetName(solver_name);

        const auto n = static_cast<vertex_t>(graph.size());
        vector_t<sat::BoolVar> x(n);

        // Variables
        for (vertex_t i = 0; i < n; ++i)
            x[i] = cp_model.NewBoolVar();

        // Constraints
        for (vertex_t i = 0; i < n; ++i) {
            vector_t<sat::BoolVar> vars;
            vars.reserve(n);
            vars.push_back(x[i]);

            for (const vertex_t j: graph[i])
                vars.push_back(x[j]);

            cp_model.AddBoolOr(vars);
        }

        // Objective
        cp_model.Minimize(sat::LinearExpr::Sum(x));

        // Solve
        if (const sat::CpSolverResponse response = sat::Solve(cp_model.Build());
            response.status() == sat::CpSolverStatus::OPTIMAL) {
            if constexpr (with_lock) {
                std::lock_guard<std::mutex> lock(mtx);
                cv.notify_one();
            }
            sol.resize(n);
            for (vertex_t i = 0; i < n; ++i)
                sol[i] = sat::SolutionBooleanValue(response, x[i]);
        }
    }
}


#endif //LIB_MDS_H
