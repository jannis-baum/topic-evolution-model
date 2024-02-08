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
    this->addEdges(delta);
}

void CorpusPeriod::constructNodes() {
    // word -> (nDocumentsContaining(word),
    //          [(all other words, and their co-occurrences with word)])
    std::unordered_map<word_t, std::pair<int, std::vector<std::pair<word_t, int>>>> occurrences = {};
    // occurrence for single words
    std::vector<std::pair<word_t, int>> single_occurrences = {};
    for (const auto & [k, _kstr] : this->wtostr) {
        std::vector<std::pair<word_t, int>> nkz_occurrences = {};
        for (const auto & [z, _zstr] : this->wtostr) {
            nkz_occurrences.push_back({ z, this->nDocumentsContaining({ k, z }) });
        }
        int nk_occurrences = this->nDocumentsContaining({ k });
        occurrences[k] = { nk_occurrences, nkz_occurrences };
        single_occurrences.push_back({ k, nk_occurrences });
    }

    // create nodes for words that have 0 < occurrences < Dt / 2
    for (const auto & [word, occurrence] : single_occurrences) {
        if (occurrence && occurrence <= this->documents.size() / 2.0) {
            this->wtonode_all.emplace(word, SemanticNode(word, {}));
            this->wtonode.emplace(word, &this->wtonode_all.at(word));
        }
    }

    // sort by number of occurrences for merging
    std::sort(single_occurrences.begin(), single_occurrences.end(), [](const auto & a, const auto & b) {
        return a.second < b.second;
    });

    // merge subwords that only occur with another superword are merged by
    // being represented by the superword's node
    // (sorting is necessary to correctly resolve relationships with more than
    // 2 words)
    for (const auto & [word, occurrence] : single_occurrences) {
        if (!this->wtonode.contains(word)) continue;

        auto co_occurrences = occurrences[word].second;
        for (const auto & [co_word, co_occurrence] : co_occurrences) {
            if (occurrences[co_word].first == co_occurrence) {
                // reassign node
                // operator[] not allowed because SemanticNode has no ()
                // constructor
                auto it = this->wtonode.find(co_word);
                if (it != this->wtonode.end()) {
                    SemanticNode *supernode = this->wtonode.at(word);
                    it->second = supernode;
                    supernode->subwords.insert(it->first);
                }
            }
        }
    }
}

void CorpusPeriod::addEdges(const dec_t delta) {
    // find main words for nodes
    std::unordered_set<word_t> words = {};
    for (const auto & [word, node] : this->wtonode) {
        words.insert(node->word);
    }
    // find term corralations between all word pairs as matrix (undefined if no
    // correlation), and as list of all correlations
    std::vector<std::vector<std::optional<dec_t>>> correlation_matrix;
    std::vector<dec_t> correlation_values;
    for (const auto word1 : words) {
        correlation_matrix.push_back({});
        for (const auto word2 : words) {
            if (word1 == word2) continue;
            const std::optional<dec_t> correlation = this->termCorrelation(word1, word2);
            correlation_matrix.back().push_back(correlation);
            if (correlation.has_value()) {
                correlation_values.push_back(correlation.value());
            }
        }
    }
    if (correlation_values.empty()) return;
    // threshold to add edges
    dec_t threshold = mstdThreshold(correlation_values, delta);

    // add neighbors for nodes by threshold
    auto it_i = words.begin();
    for (int i = 0; i < correlation_matrix.size(); i++) {
        auto it_j = words.begin();
        for (int j = 0; j < correlation_matrix[i].size(); j++) {
            const auto correlation = correlation_matrix[i][j];
            if (correlation.has_value() && correlation.value() > threshold) {
                this->wtonode.at(*it_i)->neighbors.push_back(
                    { correlation.value(), this->wtonode.at(*it_j) }
                );
            }
            it_j++;
        }
        it_i++;
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

std::optional<dec_t> CorpusPeriod::termCorrelation(const word_t k, const word_t z) const {
    int n_kz = this->nDocumentsContaining({ k, z });
    // return nullopt if no co-occurrence
    if (!n_kz) return std::nullopt;

    int n_k = this->nDocumentsContaining({ k });
    int n_z = this->nDocumentsContaining({ z });
    int d_t = this->documents.size();

    // forumla from definitions.md / paper
    return std::log(
        ((dec_t)n_kz / (n_k - n_kz)) /
        ((dec_t)(n_z - n_kz) / (d_t - n_z - n_k + n_kz))
    ) * std::abs(
        (dec_t)n_kz / n_k -
        (dec_t)(n_z - n_kz) / (d_t - n_k)
    );
}
