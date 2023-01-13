#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <unordered_map>
#include <vector>

#include "types.hpp"

class Document {
    std::vector<word_t> words;
    std::unordered_map<word_t, int> words_tf;
    int tf_max;

    public:
        Document(std::vector<word_t> words);
        dec_t nutrition(word_t word, dec_t c);
};

#endif
