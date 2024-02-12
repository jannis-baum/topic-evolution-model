#include <queue>
#include <iostream>
#include <unordered_set>

#include "SemanticGraph.hpp"

void SemanticNode::bfs(std::function<bool(const SemanticNode *)> f, dec_t theta) const {
    if (!theta) return;

    // discovered nodes & the total weight of the path there
    std::queue<std::pair<const SemanticNode *, dec_t>> discovered;
    discovered.push({ this, 0 });
    std::pair<const SemanticNode *, dec_t> current;
    // assign front to `current`, pop front and return true if there are more
    // elements in queue
    std::function<bool()> popFront = [&discovered, &current]() mutable {
        if (discovered.empty()) return false;
        current = discovered.front();
        discovered.pop();
        return true;
    };

    while (popFront() && current.second <= theta) {
        // don't call `f` on the node itself
        if (current.second) {
            if (!f(current.first)) return;
        }
        for (const auto & [weight, child]: current.first->neighbors) {
            discovered.push({ child, current.second + weight });
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
