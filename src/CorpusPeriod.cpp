#include <numeric>

#include "CorpusPeriod.hpp"

CorpusPeriod::CorpusPeriod(std::vector<Document> documents)
: documents(documents) {}

dec_t CorpusPeriod::nutrition(word_t word, dec_t c) {
    dec_t total = 0;
    for (auto &document : this->documents) {
        total += document.nutrition(word, c);
    }
    return total;
}
