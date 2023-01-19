#include <numeric>

#include "CorpusPeriod.hpp"

CorpusPeriod::CorpusPeriod(const std::vector<Document> documents)
: documents(documents) {}

dec_t CorpusPeriod::nutrition(const word_t word, const dec_t c) const {
    dec_t total = 0;
    for (const auto &document : this->documents) {
        total += document.nutrition(word, c);
    }
    return total;
}

int CorpusPeriod::nDocumentsContaining(const std::initializer_list<word_t> words) const {
    int count = 0;
    for (const auto &document : this->documents) {
        count += document.hasAllWords(words);
    }
    return count;
}
