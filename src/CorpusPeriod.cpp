#include <numeric>

#include "CorpusPeriod.hpp"

CorpusPeriod::CorpusPeriod(std::vector<Document> documents)
: documents(documents) {}

dec_t CorpusPeriod::nutrition(word_t word, dec_t c) {
    return std::reduce(
        this->documents.begin(),
        this->documents.end(),
        0,
        [word, c](dec_t total, Document document) {
            return total + document.nutrition(word, c);
        }
    );
}
