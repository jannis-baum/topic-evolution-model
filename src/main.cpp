#include <cstdlib>
#include <exception>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "Corpus.hpp"
#include "Document.hpp"
#include "types.hpp"
#include "argparse.hpp"
#include "yaml.hpp"

#ifdef TESTING_ENV
#include "test/tests.hpp"
#endif

// read corpus from stdin
// - corpus periods are separated by emtpy lines
// - documents in periods are lines
// - words in documents are separated by spaces
int main(int argc, char* argv[]) {
#ifdef TESTING_ENV
    return testAll();
#endif

    // TEM params
    dec_t delta, c, alpha, beta, gamma;
    dec_t merge_threshold, evolution_threshold;
    int theta;
    try {
        // convert strings to dec_t & int
        std::function<dec_t(char*)> stod = [](char *val) { return std::stof(val); };
        std::function<int(char*)> stoi = [](char *val) { return std::stoi(val); };

        // parse args
        delta = getArgOrFail<dec_t>(argv, argv + argc, "--delta", stod);
        c = getArgOrFail<dec_t>(argv, argv + argc, "--c", stod);
        alpha = getArgOrFail<dec_t>(argv, argv + argc, "--alpha", stod);
        beta = getArgOrFail<dec_t>(argv, argv + argc, "--beta", stod);
        gamma = getArgOrFail<dec_t>(argv, argv + argc, "--gamma", stod);
        merge_threshold = getArgOrFail<dec_t>(argv, argv + argc, "--merge_threshold", stod);
        evolution_threshold = getArgOrFail<dec_t>(argv, argv + argc, "--evolution_threshold", stod);

        theta = getArgOrFail<int>(argv, argv + argc, "--theta", stoi);
    } catch (const std::invalid_argument &ex) {
        std::cout << "Fatal: " << ex.what() << std::endl;
        return 1;
    }

    // vector (periods) of vectors (documents) of vectors (words) of strings
    std::vector<std::vector<std::vector<std::string>>> structured_corpus = {{}};

    // read lines
    for (std::string line; std::getline(std::cin, line);) {
        // empty line -> next period starts
        if (line.empty()) {
            structured_corpus.push_back({});
            continue;
        }

        // currently adding documents to last period
        auto *current_period = &structured_corpus.back();
        std::stringstream line_stream(line);
        
        // vector of words (data for this line's document) split by spaces
        std::vector<std::string> words = {};
        for (std::string word; getline(line_stream, word, ' ');) {
            words.push_back(word);
        }

        // add to current period
        current_period->push_back(words);
    }

    Corpus corpus(structured_corpus, delta, c, alpha, beta, gamma, theta, merge_threshold, evolution_threshold);
    const auto evolution = corpus.getTopicEvolution();

    std::cout << dumpTopicEvolution(evolution, corpus.wtostr);

    return 0;
}
