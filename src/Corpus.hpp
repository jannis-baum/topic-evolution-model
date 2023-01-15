#ifndef CORPUS_HPP
#define CORPUS_HPP

#include <string>
#include <vector>

#include "CorpusPeriod.hpp"

class Corpus {
    std::vector<CorpusPeriod> periods;

    public:
        Corpus(std::vector<CorpusPeriod> periods);
        Corpus(std::vector<std::vector<std::vector<std::string>>> structuredCorpus);

        dec_t energy(word_t word, int s, dec_t c);
        dec_t enr(word_t word, int s, dec_t c);
};

#endif
