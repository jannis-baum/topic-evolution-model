#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

#include "Corpus.hpp"
#include "Document.hpp"

int main(int argc, char* argv[]) {
    // reads corpus from stdin
    // - corpus periods are separated by emtpy lines
    // - documents in periods are lines
    // - words in documents are separated by spaces

    std::vector<std::vector<std::vector<std::string>>> structuredCorpus = {{}};

    for (std::string line; std::getline(std::cin, line);) {
        if (line.empty()) {
            structuredCorpus.push_back({});
            continue;
        }

        auto *currentPeriod = &structuredCorpus.back();
        std::stringstream lineStream(line);
        
        std::vector<std::string> words = {};
        std::string word;
        while (getline(lineStream, word, ' ')) words.push_back(word);

        currentPeriod->push_back(words);
    }

    Corpus corpus(structuredCorpus);
    std::cout << corpus;

    return 0;
}
