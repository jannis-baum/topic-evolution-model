#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

#include "Corpus.hpp"
#include "Document.hpp"

// read corpus from stdin
// - corpus periods are separated by emtpy lines
// - documents in periods are lines
// - words in documents are separated by spaces
int main(int argc, char* argv[]) {

    // vector (periods) of vectors (documents) of vectors (words) of strings
    std::vector<std::vector<std::vector<std::string>>> structuredCorpus = {{}};

    // read lines
    for (std::string line; std::getline(std::cin, line);) {
        // empty line -> next period starts
        if (line.empty()) {
            structuredCorpus.push_back({});
            continue;
        }

        // currently adding documents to last period
        auto *currentPeriod = &structuredCorpus.back();
        std::stringstream lineStream(line);
        
        // vector of words (data for this line's document) split by spaces
        std::vector<std::string> words = {};
        std::string word;
        while (getline(lineStream, word, ' ')) words.push_back(word);

        // add to current period
        currentPeriod->push_back(words);
    }

    Corpus corpus(structuredCorpus);
    std::cout << corpus;

    return 0;
}
