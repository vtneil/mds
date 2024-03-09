#ifndef HPA_2110452_MIN_DOM_SET_GRAPH_H
#define HPA_2110452_MIN_DOM_SET_GRAPH_H

#include "container/generic.h"
#include "container/array.h"

namespace container {
    template<typename Integral>
    struct graph_node {
        Integral vertex;
        std::initializer_list<Integral> edges;
    };

    /**
     * Graph represented in simple adjacency lists
     * using pre-allocated container.
     *
     * @tparam Integral Types to represent vertices (default: int)
     * @tparam NumVertex Maximum number of vertices in a graph
     * @tparam NumEdgePerVertex Maximum number of edges per vertex (default: NumVertex)
     * @tparam EdgesArrayContainer Dynamic array container for edges
     * @tparam GraphContainer
     */
    template<typename Integral,
            types::size_type NumVertex,
            types::size_type NumEdgePerVertex = NumVertex,
            template<typename, types::size_type, template<typename, size_t = 0> class> class VertexArrayContainer = container::heap_array_t,
            template<typename, types::size_type, template<typename, size_t = 0> class> class EdgesArrayContainer = container::heap_dynamic_array_t,
            template<typename, size_t = 0> class Allocator = memory::NewAllocator
    >
    struct graph_adjacency_list {
        using edges_container_t = EdgesArrayContainer<Integral, NumEdgePerVertex, Allocator>;
        using graph_container_t = VertexArrayContainer<edges_container_t, NumVertex, Allocator>;

        graph_container_t graph_repr = {};

        template<typename ...Integrals>
        constexpr void push_nodes(graph_node<Integrals> ...nodes) {
            push_nodes({nodes...});
        }

        constexpr void push_nodes(std::initializer_list<graph_node<Integral>> nodes) {
            for (const auto &node: nodes)
                push_nodes(node);
        }

        constexpr void push_nodes(types::const_reference<graph_node<Integral>> node) {
            graph_repr[node.vertex.value].push_back(node.edges);
        }

        // Element access
        [[nodiscard]] FORCE_INLINE CPP17_CONSTEXPR types::reference<edges_container_t>
        operator[](types::size_type index) noexcept {
            return graph_repr[index];
        }

        [[nodiscard]] FORCE_INLINE constexpr types::const_reference<edges_container_t>
        operator[](types::size_type index) const noexcept {
            return graph_repr[index];
        }

        // Iterator
        [[nodiscard]] FORCE_INLINE CPP17_CONSTEXPR types::pointer<edges_container_t> begin() noexcept {
            return graph_repr.begin();
        }

        [[nodiscard]] FORCE_INLINE constexpr types::pointer_to_const<edges_container_t> begin() const noexcept {
            return graph_repr.begin();
        }

        [[nodiscard]] FORCE_INLINE CPP17_CONSTEXPR types::pointer<edges_container_t> end() noexcept {
            return graph_repr.end();
        }

        [[nodiscard]] FORCE_INLINE constexpr types::pointer_to_const<edges_container_t> end() const noexcept {
            return graph_repr.end();
        }
    };

    class adjacency_matrix {

    };
}

#endif //HPA_2110452_MIN_DOM_SET_GRAPH_H
