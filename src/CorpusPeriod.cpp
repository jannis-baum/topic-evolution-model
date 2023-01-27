#include <algorithm>
#include <cmath>
#include <exception>
#include <numeric>

#include "CorpusPeriod.hpp"
#include "SemanticGraph.hpp"
#include "helpers.hpp"

CorpusPeriod::CorpusPeriod(const std::vector<Document> documents, const std::unordered_map<word_t, std::string> &wtostr)
: documents(documents), wtostr(wtostr), wtonode({}) {
    // word -> (nDocumentsContaining(word),
    //          [(all other words, and their co-occurrences with word)])
    std::unordered_map<word_t, std::pair<int, std::vector<std::pair<word_t, int>>>> occurrences = {};
    // occurrence for single words
    std::vector<std::pair<word_t, int>> singleOccurrences = {};
    for (const auto & [k, _kstr] : this->wtostr) {
        std::vector<std::pair<word_t, int>> nkzOccurrences = {};
        for (const auto & [z, _zstr] : this->wtostr) {
            nkzOccurrences.push_back({ z, this->nDocumentsContaining({ k, z }) });
        }
        int nkOccurrences = this->nDocumentsContaining({ k });
        occurrences[k] = { nkOccurrences, nkzOccurrences };
        singleOccurrences.push_back({ k, nkOccurrences });
    }

    // create nodes for words that have 0 < occurrences < Dt / 2
    for (const auto & [word, occurrence] : singleOccurrences) {
        if (occurrence && occurrence < this->documents.size() / 2) {
            this->wtonode.emplace(word, SemanticNode(word, {}));
        }
    }

    // sort by number of occurrences for merging
    std::sort(singleOccurrences.begin(), singleOccurrences.end(), [](const auto & a, const auto & b) {
        return a.second < b.second;
    });

    // merge subwords that only occur with another superword are merged by
    // being represented by the superword's node
    // (sorting is necessary to correctly resolve relationships with more than
    // 2 words)
    for (const auto & [word, occurrence] : singleOccurrences) {
        if (!this->wtonode.contains(word)) continue;

        auto coOccurrences = occurrences[word].second;
        for (const auto & [coWord, coOccurrence] : coOccurrences) {
            if (occurrences[coWord].first == coOccurrence) {
                // reassign node
                // operator[] not allowed because SemanticNode has no ()
                // constructor
                auto it = this->wtonode.find(coWord);
                if (it != this->wtonode.end()) {
                    it->second = this->wtonode.at(word);
                }
            }
        }
    }
};

dec_t CorpusPeriod::nutrition(const word_t word, const dec_t c) const {
    dec_t total = 0;
    for (const auto &document : this->documents) {
        total += document.nutrition(word, c);
    }
    return total / this->documents.size();
}

int CorpusPeriod::nDocumentsContaining(const std::initializer_list<word_t> words) const {
    int count = 0;
    for (const auto &document : this->documents) {
        count += document.hasAllWords(words);
    }
    return count;
}

dec_t CorpusPeriod::termCorrelation(const word_t k, const word_t z) const {
    int n_kz = this->nDocumentsContaining({ k, z });
    int n_k = this->nDocumentsContaining({ k });
    int n_z = this->nDocumentsContaining({ z });
    int d_t = this->documents.size();

    // check dividents != 0
    if (n_k - n_kz == 0) return 0;
    if (d_t - n_z - n_k + n_kz == 0) return 0;
    if (n_z - n_kz == 0) return 0;
    if (n_k == 0) return 0;
    if (d_t - n_k == 0) return 0;

    // forumla from definitions.md / paper
    return std::log(
        (n_kz / (n_k - n_kz)) /
        ((n_z - n_kz) / (d_t - n_z - n_k + n_kz))
    ) * std::abs(
        (dec_t)n_kz / n_k -
        (dec_t)(n_z - n_kz) / (d_t - n_k)
    );
}

std::vector<SemanticNode> CorpusPeriod::semanticGraph(const dec_t delta) const {
    // construct node for each word
    std::vector<SemanticNode> graph;
    for (const auto & [word, str] : this->wtostr) {
        graph.push_back(SemanticNode(word, {}));
    }
    // find term corralations between all word pairs as matrix (2d-vector),
    // as list of values,
    // and find mean value
    std::vector<std::vector<dec_t>> correlationMatrix;
    std::vector<dec_t> correlationValues;
    for (const auto & node1 : graph) {
        correlationMatrix.push_back({});
        for (const auto & node2 : graph) {
            const dec_t correlation = this->termCorrelation(node1.word, node2.word);
            correlationMatrix.back().push_back(correlation);
            correlationValues.push_back(correlation);
        }
    }

    // add neighbors for nodes by threshold
    dec_t threshold = mstdThreshold(correlationValues, delta);
    for (int i = 0; i < graph.size(); i++) {
        for (int j = 0; j < graph.size(); j++) {
            const dec_t correlation = correlationMatrix[i][j];
            if (correlation > threshold) {
                graph[i].neighbors.push_back({ correlation, &graph[j] });
            }
        }
    }
    return graph;
}
