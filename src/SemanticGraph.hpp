#ifndef SEMANTIC_GRAPH_HPP
#define SEMANTIC_GRAPH_HPP

#include <utility>
#include <vector>

class SemanticNode {
    int word;
    std::vector<std::pair<float, SemanticNode*>> neighbors;
};

#endif
