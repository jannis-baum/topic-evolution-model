#include <algorithm>
#include <unordered_map>
#include <vector>

#include "Document.hpp"
#include "types.hpp"

Document::Document(std::vector<word_t> words, std::unordered_map<word_t, std::string> &wtostr)
: wtostr(wtostr)
, words(words)
, words_tf({})
, tf_max(0) {
    std::for_each(this->words.begin(), this->words.end(), [this](word_t word) {
        if (this->words_tf.contains(word)) {
            this->words_tf[word] += 1;
        } else {
            this->words_tf[word] = 1;
        }
    });

    for (const auto & [word, tf] : this->words_tf) {
        if (tf > tf_max) this->tf_max = tf;
    }
}

dec_t Document::nutrition(word_t word, dec_t c) {
    if (!this->words_tf.contains(word)) {
        return 1 - c;
    }
    return (1 - c) + c * (dec_t)this->words_tf[word] / (dec_t)this->tf_max;
}
