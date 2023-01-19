#ifndef CORPUS_HPP
#define CORPUS_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "CorpusPeriod.hpp"

class Corpus {
    std::vector<CorpusPeriod> periods;
    std::unordered_map<word_t, std::string> wtostr;

    public:
        Corpus(const std::vector<CorpusPeriod> periods);
        Corpus(const std::vector<std::vector<std::vector<std::string>>> structuredCorpus);

        dec_t energy(const word_t word, const int s, dec_t c) const;
        dec_t enr(const word_t word, const int s, dec_t c) const;

        friend std::ostream& operator<<(std::ostream& os, Corpus const &corpus) {
            for (const auto period : corpus.periods) os << period << std::endl;
            return os;
        };
};

#endif
