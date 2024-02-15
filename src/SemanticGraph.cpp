#include <queue>
#include <iostream>
#include <unordered_set>

#include "SemanticGraph.hpp"

void SemanticNode::bfs(std::function<bool(const SemanticNode *)> f, dec_t theta) const {
    if (!theta) return;

    // discovered nodes
    std::unordered_set<const SemanticNode *> discovered = { this };
    // nodes to check & the total weight of the path there
    std::queue<std::pair<const SemanticNode *, dec_t>> todo;
    todo.push({ this, 0 });
    std::pair<const SemanticNode *, dec_t> &current = todo.front();
    // assign front to `current`, pop front and return true if there are more
    // elements in queue
    std::function<bool()> popFront = [&todo, &current]() mutable {
        if (todo.empty()) return false;
        current = todo.front();
        todo.pop();
        return true;
    };

    while (popFront() && current.second <= theta) {
        // don't call `f` on the node itself
        if (current.first != this) {
            if (!f(current.first)) return;
        }
        for (const auto & [weight, child]: current.first->neighbors) {
            if (discovered.find(child) != discovered.end()) continue;
            discovered.insert(child);
            todo.push({ child, current.second + weight });
        }
    }
}

std::unordered_set<word_t> SemanticNode::allWords() const {
    std::unordered_set<word_t> words = { this->word };
    for (const auto &word: this->subwords) {
        words.insert(word);
    }
    return words;
}
