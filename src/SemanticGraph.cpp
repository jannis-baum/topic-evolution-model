#include <queue>
#include <iostream>
#include <unordered_set>

#include "SemanticGraph.hpp"

void SemanticNode::bfs(std::function<bool(const SemanticNode *)> f, int theta) const {
    if (!theta) return;

    std::queue<std::pair<const SemanticNode *, int>> discovered;
    discovered.push({ this, 0 });
    std::pair<const SemanticNode *, int> current;
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
        for (const auto & [correlation, child]: current.first->neighbors) {
            discovered.push({ child, current.second + 1 });
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
