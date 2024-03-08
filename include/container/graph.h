#ifndef HPA_2110452_MIN_DOM_SET_GRAPH_H
#define HPA_2110452_MIN_DOM_SET_GRAPH_H

#include "container/generic.h"
#include "container/array.h"

namespace container {
    template<typename Integral,
            types::size_type NumVertex,
            types::size_type NumEdgePerVertex,
            typename GraphContainer = container::heap_array_t<
                    container::heap_dynamic_array_t<
                            Integral,
                            NumEdgePerVertex,
                            memory::NewAllocator
                    >,
                    NumVertex,
                    memory::NewAllocator
            >
    >
    struct adjacency_list {
        GraphContainer graph_repr = {};
    };

    class adjacency_matrix {

    };
}

#endif //HPA_2110452_MIN_DOM_SET_GRAPH_H
