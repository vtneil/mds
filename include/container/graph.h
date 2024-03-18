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
     * @tparam GraphContainer
     */
    template<typename Integral,
            types::size_type NumVertex,
            types::size_type NumEdgePerVertex = NumVertex,
            template<typename, types::size_type, template<typename, size_t = 0> class> class VertexArrayContainer = container::heap_array_t,
            template<typename, types::size_type, template<typename, size_t = 0> class> class EdgesArrayContainer = container::heap_dynamic_array_t,
            template<typename, types::size_type, template<typename, size_t = 0> class> class SetCoverArrayContainer = container::heap_array_t,
            template<typename, size_t = 0> class Allocator = memory::NewAllocator
    >
    struct graph_t {
        using edges_container_t = EdgesArrayContainer<Integral, NumEdgePerVertex, Allocator>;
        using graph_container_t = VertexArrayContainer<edges_container_t, NumVertex, Allocator>;
        using set_cover_t = SetCoverArrayContainer<container::bitset_t<NumEdgePerVertex, int_fast32_t>, NumVertex, Allocator>;
        using deg_array_t = SetCoverArrayContainer<int, NumVertex, Allocator>;
        using vertex_set_array_t = SetCoverArrayContainer<vertex_set, NumVertex, Allocator>;
        using unique_vertex_array_t = EdgesArrayContainer<typename Integral::type, NumVertex, Allocator>;

        graph_container_t list = {};
        vertex_set_array_t set = {};
        set_cover_t matrix = {};
        unique_vertex_array_t unique = {};
        deg_array_t degrees = {};

        typename Integral::type v;
        typename Integral::type e;

        template<typename ...Integrals>
        constexpr void push_nodes(graph_node<Integrals> ...nodes) {
            push_nodes({nodes...});
        }

        constexpr void push_nodes(std::initializer_list<graph_node<Integral>> nodes) {
            for (const auto &node: nodes)
                push_nodes(node);
        }

        constexpr void push_nodes(types::const_reference<graph_node<Integral>> node) {
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

        void unionize(graph_t &dst) {
            bool stop_flag = false;

            // Unionize by setting this graph (will be unusable)
            while (!stop_flag) {

                stop_flag = true;

                // C(v, 2) combinatorial pair
                for (typename Integral::type v2 = 0; v2 < v; ++v2) {
                    for (typename Integral::type v1 = 0; v1 < v2; ++v1) {

                        if (static_cast<int_fast8_t>(set[v1]) || static_cast<int_fast8_t>(set[v2]))
                            continue;

                        bitset_compare cmp = matrix[v1].compare(matrix[v2]);

                        switch (cmp) {
                            case bitset_compare::NO_COVER:
                                break;
                            case bitset_compare::COVERS:
                                set[v2] = vertex_set::BLUE;
                                stop_flag = false;
                                break;
                            case bitset_compare::COVERED_BY:
                                set[v1] = vertex_set::BLUE;
                                stop_flag = false;
                                break;
                            default:
                                UNREACHABLE();
                        }

                    }
                }
            }

            SetCoverArrayContainer<typename Integral::type, NumVertex, Allocator> reverse_v_map = {};

            dst.v = 0;

            for (typename Integral::type i = 0, pos = 0; i < v; ++i) {
                if (set[i] == vertex_set::RED) {
                    reverse_v_map[i] = pos++;
                    unique.push_back(i);
                    ++dst.v;
                }
            }

            for (typename Integral::type i = 0; i < unique.size(); ++i) {
                for (typename Integral::type j = 0; j < v; ++j) {
                    if (i != reverse_v_map[j] && set[j] == vertex_set::RED && matrix[unique[i]].get(j)) {
                        dst.push_nodes(container::graph_node<Integral>{i, {reverse_v_map[j]}});
                        dst.push_nodes(container::graph_node<Integral>{reverse_v_map[j], {i}});
                    }
                }
            }
        }
    };
}

namespace graph_utils {
    float density(long v, long e) {
        return 2.f * static_cast<float>(e) / (static_cast<float>(v) * static_cast<float>(v - 1));
    }

    float dispersion(long v, long e) {
        float mean = 2.f * static_cast<float>(e) / static_cast<float>(v);
    }
}

#endif //HPA_2110452_MIN_DOM_SET_GRAPH_H
