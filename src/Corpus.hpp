#ifndef CORPUS_HPP
#define CORPUS_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "CorpusPeriod.hpp"

class Corpus {
    std::vector<CorpusPeriod> periods;
    // word_t (aka int) to string mapping
    std::unordered_map<word_t, std::string> wtostr;

    public:
        Corpus(const std::vector<CorpusPeriod> periods);
        // construct Corpus from vector (periods) of vectors (documents) of
        // strings (words)
        Corpus(const std::vector<std::vector<std::vector<std::string>>> structuredCorpus);

        // see definitions.md or paper
        dec_t energy(const word_t word, const int s, const dec_t c) const;
        // ENR (energy-nutrition-ratio), see definitions.md or paper
        dec_t enr(const word_t word, const int s, const dec_t c) const;

        // see definitions.md or paper
        bool isEmerging(
            const word_t word,
            const int s,
            const dec_t c,
            const dec_t alpha,
            const dec_t beta,
            const dec_t gamma
        ) const;

        // streaming (e.g. printing) operator <<
        friend std::ostream& operator<<(std::ostream& os, Corpus const &corpus) {
            for (const auto period : corpus.periods) os << period << std::endl;
            return os;
        };
};

#endif
