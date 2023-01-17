#ifndef CORPUS_PERIOD_HPP
#define CORPUS_PERIOD_HPP

#include <iostream>
#include <vector>

#include "Document.hpp"
#include "types.hpp"

class CorpusPeriod {
    const std::vector<Document> documents;

    public:
        CorpusPeriod(const std::vector<Document> documents);
        dec_t nutrition(const word_t word, const dec_t c) const;

        friend std::ostream& operator<<(std::ostream& os, CorpusPeriod const &period) {
            for (const auto document : period.documents) os << document << std::endl;
            return os;
        };
};

#endif
