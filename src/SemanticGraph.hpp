#ifndef SEMANTIC_GRAPH_HPP
#define SEMANTIC_GRAPH_HPP

#include <functional>
#include <utility>
#include <vector>
#include <unordered_set>

#include "types.hpp"

class SemanticNode {
    public:
        word_t word;
        std::vector<std::pair<dec_t, SemanticNode*>> neighbors;
        // words that got merged into this node
        std::unordered_set<word_t> subwords;

        SemanticNode(word_t word, std::vector<std::pair<dec_t, SemanticNode*>> neighbors)
        : word(word), neighbors(neighbors), subwords({}) {};

        // BFS up to max depth theta
        // calls `f` with each discovered node and stops if `f` returns false
        void bfs(std::function<bool(const SemanticNode *)> f, int theta) const;

        std::unordered_set<word_t> allWords() const;
};

#endif
