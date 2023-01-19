#include <algorithm>
#include <unordered_map>
#include <vector>

#include "Document.hpp"
#include "types.hpp"

Document::Document(const std::vector<word_t> words, const std::unordered_map<word_t, std::string> &wtostr)
: wtostr(wtostr)
, words(words)
, words_tf({})
, tf_max(0) {
    // build term-frequency map for words
    std::for_each(this->words.begin(), this->words.end(), [this](word_t word) {
        if (this->words_tf.contains(word)) {
            this->words_tf[word] += 1;
        } else {
            this->words_tf[word] = 1;
        }
    });

    // find highest term-frequency
    for (const auto & [word, tf] : this->words_tf) {
        if (tf > tf_max) this->tf_max = tf;
    }
}

dec_t Document::nutrition(const word_t word, const dec_t c) const {
    // see definitions.md or paper
    if (!this->words_tf.contains(word)) {
        return 1 - c;
    }
    return (1 - c) + c * (dec_t)this->words_tf.at(word) / (dec_t)this->tf_max;
}

bool Document::hasAllWords(const std::initializer_list<word_t> words) const {
    for (auto word : words) {
        if (!this->words_tf.contains(word)) return false;
    }
    return true;
}
