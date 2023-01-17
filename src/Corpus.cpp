#include "Corpus.hpp"
#include <cmath>
#include <numeric>
#include <unordered_map>

Corpus::Corpus(const std::vector<CorpusPeriod> periods)
: periods(periods) {}

Corpus::Corpus(const std::vector<std::vector<std::vector<std::string>>> structuredCorpus)
: periods({})
, wtostr({}) {
    std::unordered_map<std::string, word_t> strtow = {};
    
    for (const auto period : structuredCorpus) {
        std::vector<Document> documents = {};
        for (const auto document : period) {
            std::vector<word_t> words = {};
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
        energy += (dec_t)1 / i * (
            std::pow(this->periods[s].nutrition(word, c), 2) -
            std::pow(this->periods[s - i].nutrition(word, c), 2)
        );
    }
    return energy;
}

dec_t Corpus::enr(const word_t word, const int s, dec_t c) const {
    return this->energy(word, s, c) / this->periods[s].nutrition(word, c);
}
