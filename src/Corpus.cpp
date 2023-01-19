#include "Corpus.hpp"
#include <cmath>
#include <numeric>
#include <unordered_map>

Corpus::Corpus(std::vector<CorpusPeriod> periods)
: periods(periods) {}

Corpus::Corpus(std::vector<std::vector<std::vector<std::string>>> structuredCorpus)
: periods({}) {
    std::unordered_map<std::string, word_t> lookup = {};
    
    for (const auto period : structuredCorpus) {
        std::vector<Document> documents = {};
        for (const auto document : period) {
            std::vector<word_t> words = {};
            for (const auto word : document) {
                if (!lookup.contains(word)) {
                    lookup[word] = lookup.size();
                }
                words.push_back(lookup[word]);
            }
            documents.push_back(Document(words));
        }
        this->periods.push_back(CorpusPeriod(documents));
    }
}

dec_t Corpus::energy(word_t word, int s, dec_t c) {
    dec_t energy = 0;
    for (int i = 1; i <= s; i++) {
        energy += (dec_t)1 / i * (
            std::pow(this->periods[s].nutrition(word, c), 2) -
            std::pow(this->periods[s - i].nutrition(word, c), 2)
        );
    }
    return energy;
}

dec_t Corpus::enr(word_t word, int s, dec_t c) {
    return this->energy(word, s, c) / this->periods[s].nutrition(word, c);
}
