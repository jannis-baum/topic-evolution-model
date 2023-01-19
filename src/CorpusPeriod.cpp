#include <cmath>
#include <exception>
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

dec_t CorpusPeriod::termCorrelation(const word_t k, const word_t z) const {
    int n_kz = this->nDocumentsContaining({ k, z });
    int n_k = this->nDocumentsContaining({ k });
    int n_z = this->nDocumentsContaining({ z });
    int d_t = this->documents.size();

    // check dividents != 0 & log parameter > 0
    if (n_k - n_kz == 0) return 0;
    if (d_t - n_z - n_k + n_kz == 0) return 0;
    if (n_z - n_kz == 0) return 0;
    dec_t logParam = 
        ((dec_t)n_kz / (n_k - n_kz)) /
        ((dec_t)(n_z - n_kz) / (d_t - n_z - n_k + n_kz));
    if (logParam <= 0) return 0;
    if (n_k == 0) return 0;
    if (d_t - n_k == 0) return 0;

    // forumla from definitions.md / paper
    return std::log(
        (n_kz / (n_k - n_kz)) /
        ((n_z - n_kz) / (d_t - n_z - n_k + n_kz))
    ) * std::abs(
        (dec_t)n_kz / n_k -
        (dec_t)(n_z - n_kz) / (d_t - n_k)
    );
}
