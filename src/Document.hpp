#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <ostream>
#include <unordered_map>
#include <vector>

#include "types.hpp"

class Document {
    const std::unordered_map<word_t, std::string> &wtostr;

    const std::vector<word_t> words;
    std::unordered_map<word_t, int> words_tf;
    int tf_max;

    public:
        Document(const std::vector<word_t> words, const std::unordered_map<word_t, std::string> &wtostr);
        dec_t nutrition(const word_t word, const dec_t c) const;

        friend std::ostream& operator<<(std::ostream& os, Document const &document) {
            for (const auto word : document.words) os << document.wtostr.at(word) << " ";
            return os;
        };
};

#endif
