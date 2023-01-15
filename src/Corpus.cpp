#include "Corpus.hpp"
#include <cmath>
#include <numeric>

Corpus::Corpus(std::vector<CorpusPeriod> periods)
: periods(periods) {}

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
