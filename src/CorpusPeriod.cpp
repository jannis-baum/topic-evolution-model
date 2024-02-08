#include <algorithm>
#include <cmath>
#include <exception>
#include <numeric>
#include <unordered_set>
#include <utility>

#include "CorpusPeriod.hpp"
#include "Document.hpp"
#include "SemanticGraph.hpp"
#include "helpers.hpp"

// constructor from list of list of words (constructs documents)
CorpusPeriod::CorpusPeriod(
    const std::vector<std::vector<word_t>> structured_documents,
    const std::unordered_map<word_t, std::string> &wtostr)
: documents({}), wtostr(wtostr), wtonode({}) {
    for (const auto & words: structured_documents) {
        this->documents.push_back(Document(words, this->wtostr));
    }
};

// constructor from list of list of documents
CorpusPeriod::CorpusPeriod(
    const std::vector<Document> documents,
    const std::unordered_map<word_t, std::string> &wtostr)
: documents(documents), wtostr(wtostr), wtonode({}) { };

// construct graph
void CorpusPeriod::constructGraph(const dec_t delta, dec_t **distances) {
    this->constructNodes();
    this->addEdges(delta, distances);
}

void CorpusPeriod::constructNodes() {
    for (const auto & [word, _] : this->wtostr) {
        int occurrences = this->nDocumentsContaining({ word });
        if (occurrences && occurrences <= this->documents.size() / 2.0) {
            this->wtonode.emplace(word, SemanticNode(word, {}));
        }
    }
}

void CorpusPeriod::addEdges(const dec_t delta, dec_t **distances) {
    // find main words for nodes
    std::unordered_set<word_t> words = {};
    for (const auto & [word, node] : this->wtonode) {
        words.insert(node.word);
    }
    // find list of all distance values
    std::vector<dec_t> correlation_values;
    for (const auto word1 : words) {
        for (const auto word2 : words) {
            if (word1 == word2) continue;
            correlation_values.push_back(distances[word1][word2]);
        }
    }
    if (correlation_values.empty()) return;
    // threshold to add edges
    dec_t threshold = mstdThreshold(correlation_values, delta);

    // add neighbors for nodes by threshold
    for (auto word1 : words) {
        for (auto word2 : words) {
            if (word1 == word2) continue;

            auto distance = distances[word1][word2];
            if (distance <= threshold) continue;

            this->wtonode.at(word1).neighbors.push_back(
                { distance, &(this->wtonode.at(word2)) }
            );
        }
    }
}

dec_t CorpusPeriod::nutrition(const word_t word, const dec_t c) const {
    dec_t total = 0;
    for (const auto &document : this->documents) {
        total += document.nutrition(word, c);
    }
    return total / this->documents.size();
}

std::vector<word_t> CorpusPeriod::findNonFloodWords(const dec_t c, const dec_t alpha) const {
    std::vector<dec_t> nutritions = {};
    for (const auto & [word, node] : this->wtonode) {
        nutritions.push_back(this->nutrition(word, c));
    }
    dec_t threshold = mstdThreshold(nutritions, alpha);

    std::vector<word_t> non_flood_words = {};
    auto it = this->wtonode.begin();
    for (int i = 0; i < nutritions.size(); i++) {
        if (nutritions[i] <= threshold) {
            non_flood_words.push_back(it->first);
        }
        it++;
    }
    return non_flood_words;
}

int CorpusPeriod::nDocumentsContaining(const std::initializer_list<word_t> words) const {
    int count = 0;
    for (const auto &document : this->documents) {
        count += document.hasAllWords(words);
    }
    return count;
}
