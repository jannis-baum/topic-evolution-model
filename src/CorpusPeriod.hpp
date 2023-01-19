#ifndef CORPUS_PERIOD_HPP
#define CORPUS_PERIOD_HPP

#include <iostream>
#include <vector>

#include "Document.hpp"
#include "types.hpp"

class CorpusPeriod {
    std::vector<Document> documents;

    public:
        CorpusPeriod(std::vector<Document> documents);
        dec_t nutrition(word_t word, dec_t c);

        friend std::ostream& operator<<(std::ostream& os, CorpusPeriod const &period) {
            for (const auto document : period.documents) os << document << std::endl;
            return os;
        };
};

#endif
