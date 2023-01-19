#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <initializer_list>
#include <ostream>
#include <unordered_map>
#include <vector>

#include "types.hpp"

class Document {
    // reference to Corpus' mapping to resolve word_t (aka int) to string, e.g.
    // for printing
    const std::unordered_map<word_t, std::string> &wtostr;

    const std::vector<word_t> words;
    // term frequency map
    std::unordered_map<word_t, int> words_tf;
    // highest term frequency in document
    int tf_max;

    public:
        Document(const std::vector<word_t> words, const std::unordered_map<word_t, std::string> &wtostr);
        // see definitions.md or paper
        dec_t nutrition(const word_t word, const dec_t c) const;
        // check if all words are in document
        bool hasAllWords(const std::initializer_list<word_t> words) const;

        // streaming (e.g. printing) operator <<, prints words as strings
        friend std::ostream& operator<<(std::ostream& os, Document const &document) {
            for (const auto word : document.words) os << document.wtostr.at(word) << " ";
            return os;
        };
};

#endif
