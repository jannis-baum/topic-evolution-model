#include <cstdlib>
#include <exception>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "Corpus.hpp"
#include "Document.hpp"
#include "metrics.hpp"
#include "types.hpp"
#include "argparse.hpp"
#include "yaml.hpp"

#ifdef TESTING_ENV
#include "test/tests.hpp"
#endif

void printHelp() {
    std::cout
        << "Topic Evolution Model (TEM)" << std::endl
        << std::endl
        << "Required model parameters (see paper):" << std::endl
        << "    --delta" << std::endl
        << "    --c" << std::endl
        << "    --alpha" << std::endl
        << "    --beta" << std::endl
        << "    --gamma" << std::endl
        << "    --merge_threshold" << std::endl
        << "    --evolution_threshold" << std::endl
        << "    --theta" << std::endl
        << std::endl
        << "Optional arguments:" << std::endl
        << "    --help, -h    print this message and exit." << std::endl
        << "    --metrics     output TE metrics instead of graph" << std::endl
        << std::endl;
}

// read corpus from stdin
// - corpus periods are separated by emtpy lines
// - documents in periods are lines
// - words in documents are separated by spaces
int main(int argc, char* argv[]) {
#ifdef TESTING_ENV
    return testAll();
#endif

    // argument parsing
    char **arg_beg = argv;
    char **arg_end = argv + argc;

    if (hasArg(arg_beg, arg_end, "--help") || hasArg(arg_beg, arg_end, "-h")) {
        printHelp();
        return 0;
    }

    // TEM params
    dec_t delta, c, alpha, beta, gamma;
    dec_t merge_threshold, evolution_threshold;
    int theta;
    try {
        // convert strings to dec_t & int
        std::function<dec_t(char*)> stod = [](char *val) { return std::stof(val); };
        std::function<int(char*)> stoi = [](char *val) { return std::stoi(val); };

        // parse args
        delta = getArgOrFail<dec_t>(arg_beg, arg_end, "--delta", stod);
        c = getArgOrFail<dec_t>(arg_beg, arg_end, "--c", stod);
        alpha = getArgOrFail<dec_t>(arg_beg, arg_end, "--alpha", stod);
        beta = getArgOrFail<dec_t>(arg_beg, arg_end, "--beta", stod);
        gamma = getArgOrFail<dec_t>(arg_beg, arg_end, "--gamma", stod);
        merge_threshold = getArgOrFail<dec_t>(arg_beg, arg_end, "--merge_threshold", stod);
        evolution_threshold = getArgOrFail<dec_t>(arg_beg, arg_end, "--evolution_threshold", stod);

        theta = getArgOrFail<int>(arg_beg, arg_end, "--theta", stoi);
    } catch (const std::invalid_argument &ex) {
        std::cout << "Fatal: " << ex.what() << std::endl;
        printHelp();
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

    if (hasArg(arg_beg, arg_end, "--metrics")) {
        std::cout << "[";
        const auto metrics = getMetrics(evolution);
        for (int i = 0; i < metrics.size(); i++) {
            if (i) std::cout << ", ";
            std::cout << metrics[i];
        }
        std::cout << "]" << std::endl;
    } else {
        std::cout << dumpTopicEvolution(evolution, corpus.wtostr);
    }

    return 0;
}
