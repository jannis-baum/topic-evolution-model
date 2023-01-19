#ifndef CORPUS_PERIOD_HPP
#define CORPUS_PERIOD_HPP

#include <vector>

#include "Document.hpp"
#include "types.hpp"

class CorpusPeriod {
    std::vector<Document> documents;

    public:
        CorpusPeriod(std::vector<Document> documents);
        dec_t nutrition(word_t word, dec_t c);
};

#endif
