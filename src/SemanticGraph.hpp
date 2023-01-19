#ifndef SEMANTIC_GRAPH_HPP
#define SEMANTIC_GRAPH_HPP

#include <utility>
#include <vector>

#include "types.hpp"

class SemanticNode {
    word_t word;
    std::vector<std::pair<dec_t, SemanticNode*>> neighbors;
};

#endif
