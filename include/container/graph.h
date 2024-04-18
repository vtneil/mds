#ifndef HPA_2110452_MIN_DOM_SET_GRAPH_H
#define HPA_2110452_MIN_DOM_SET_GRAPH_H

#include <fstream>
#include <algorithm>
#include "container/generic.h"
#include "container/array.h"
#include "container/bitset.h"

namespace container {
    enum class vertex_set : int_fast8_t {
        RED = 0,
        BLUE
    };

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
     */
    template<typename Integral,
        types::size_type NumVertex,
        types::size_type NumEdgePerVertex = NumVertex,
        template<typename, types::size_type, template<typename, size_t = 0> class> class VertexArrayContainer =
        heap_array_t,
        template<typename, types::size_type, template<typename, size_t = 0> class> class EdgesArrayContainer =
        heap_dynamic_array_t,
        template<typename, types::size_type, template<typename, size_t = 0> class> class SetCoverArrayContainer =
        heap_array_t,
        template<typename, size_t = 0> class Allocator = memory::NewAllocator
    >
    struct graph_t {
        using edges_container_t = EdgesArrayContainer<Integral, NumEdgePerVertex, Allocator>;
        using graph_container_t = VertexArrayContainer<edges_container_t, NumVertex, Allocator>;
        using set_cover_t = SetCoverArrayContainer<bitset_t<NumEdgePerVertex, int_fast32_t>, NumVertex, Allocator>;
        using deg_array_t = SetCoverArrayContainer<int, NumVertex, Allocator>;

        graph_container_t list = {};
        set_cover_t matrix = {};
        deg_array_t degrees = {};

        typename Integral::type v;
        typename Integral::type e;

        template<typename... Integrals>
        constexpr void push_nodes(graph_node<Integrals>... nodes) {
            push_nodes({nodes...});
        }

        constexpr void push_nodes(std::initializer_list<graph_node<Integral> > nodes) {
            for (const auto &node: nodes)
                push_nodes(node);
        }

        constexpr void push_nodes(types::const_reference<graph_node<Integral> > node) {
            list[node.vertex.value].push_back(node.edges);

            matrix[node.vertex.value].set(node.vertex.value, true);

            for (const auto &edge: node.edges) {
                matrix[node.vertex.value].set(edge.value, true);
                ++degrees[node.vertex.value];
            }
        }

        // Element access
        [[nodiscard]] FORCE_INLINE CPP17_CONSTEXPR types::reference<edges_container_t>
        operator[](types::size_type index) noexcept {
            return list[index];
        }

        [[nodiscard]] FORCE_INLINE constexpr types::const_reference<edges_container_t>
        operator[](types::size_type index) const noexcept {
            return list[index];
        }

        // Iterator
        [[nodiscard]] FORCE_INLINE CPP17_CONSTEXPR types::pointer<edges_container_t> begin() noexcept {
            return list.begin();
        }

        [[nodiscard]] FORCE_INLINE constexpr types::pointer_to_const<edges_container_t> begin() const noexcept {
            return list.begin();
        }

        [[nodiscard]] FORCE_INLINE CPP17_CONSTEXPR types::pointer<edges_container_t> end() noexcept {
            return list.end();
        }

        [[nodiscard]] FORCE_INLINE constexpr types::pointer_to_const<edges_container_t> end() const noexcept {
            return list.end();
        }

        bool from_file(const char *filename) {
            std::ifstream stream(filename);

            if (!stream.is_open()) {
                return false;
            }

            stream >> v;
            stream >> e;

            typename Integral::type v1, v2;

            while (stream >> v1 >> v2) {
                push_nodes(container::graph_node<Integral>{v1, {v2}});
                push_nodes(container::graph_node<Integral>{v2, {v1}});
            }

            stream.close();

            return true;
        }

        [[nodiscard]] float density() const {
            return 2.f * static_cast<float>(e) / (static_cast<float>(v) * static_cast<float>(v - 1));
        }

        [[nodiscard]] float mean_degrees() const {
            return static_cast<float>(degrees.sum()) / static_cast<float>(v);
        }

        [[nodiscard]] float variance_degrees() const {
            float acc = 0.f;
            const float mu = mean_degrees();

            for (typename Integral::type i = 0; i < v; ++i) {
                acc += (static_cast<float>(degrees[i]) - mu) * (static_cast<float>(degrees[i]) - mu);
            }

            return acc / static_cast<float>(v);
        }

        [[nodiscard]] float dispersion() const {
            return variance_degrees() / mean_degrees();
        }
    };
}

#endif //HPA_2110452_MIN_DOM_SET_GRAPH_H
