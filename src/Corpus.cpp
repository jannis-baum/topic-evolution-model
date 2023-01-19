#include "Corpus.hpp"
#include <cmath>
#include <numeric>
#include <unordered_map>

Corpus::Corpus(const std::vector<CorpusPeriod> periods)
: periods(periods) {}

Corpus::Corpus(const std::vector<std::vector<std::vector<std::string>>> structuredCorpus)
: periods({})
, wtostr({}) {
    // string to word_t (aka int) mapping
    std::unordered_map<std::string, word_t> strtow = {};
    
    // construct periods
    for (const auto period : structuredCorpus) {
        std::vector<Document> documents = {};
        // construct documents
        for (const auto document : period) {
            std::vector<word_t> words = {};
            // index words & save mapping in both directions
            for (const auto word : document) {
                if (!strtow.contains(word)) {
                    int newIndex = strtow.size();
                    strtow[word] = newIndex;
                    wtostr[newIndex] = word;
                }
                words.push_back(strtow[word]);
            }
            documents.push_back(Document(words, this->wtostr));
        }
        this->periods.push_back(CorpusPeriod(documents));
    }
}

dec_t Corpus::energy(const word_t word, const int s, const dec_t c) const {
    dec_t energy = 0;
    for (int i = 1; i <= s; i++) {
        // see definitions.md or paper
        energy += (dec_t)1 / i * (
            std::pow(this->periods[s].nutrition(word, c), 2) -
            std::pow(this->periods[s - i].nutrition(word, c), 2)
        );
    }
    return energy;
}

dec_t Corpus::enr(const word_t word, const int s, dec_t c) const {
    // see definitions.md or paper
    return this->energy(word, s, c) / this->periods[s].nutrition(word, c);
}

bool Corpus::isEmerging(
    const word_t word,
    const int s,
    const dec_t c,
    const dec_t alpha,
    const dec_t beta,
    const dec_t gamma
) const {
    // see definitions.md or paper
    return this->periods[s].nutrition(word, c) < alpha
        && this->energy(word, s, c) > beta
        && this->enr(word, s, c) > gamma;
}
