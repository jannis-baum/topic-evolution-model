#include <iostream>

#include "tests.hpp"
#include "../Corpus.hpp"

int testGraphConstruction() {
    int failedTests = 0;

    std::cout << std::endl << "GRAPH CONSTRUCTION" << std::endl;
    std::cout << "Node merging" << std::endl;

    failedTests += genericTest("Subword should point to superword", [](){
        Corpus corpus = Corpus({{ { "a", "b" }, { "b" }, {}, {}, {} }}, 1);
        return corpus.periods[0].wtonode.at(0).word == corpus.periods[0].wtonode.at(1).word;
    });
    failedTests += genericTest("Chain of subwords should point to common superword", [](){
        Corpus corpus = Corpus({{ {"c", "a", "b" }, { "a", "b" }, { "b" }, {}, {}, {}, {} }}, 1);
        return corpus.periods[0].wtonode.at(0).word == 2
            && corpus.periods[0].wtonode.at(1).word == 2
            && corpus.periods[0].wtonode.at(2).word == 2;
    });

    std::cout << "Edge building" << std::endl;

    failedTests += genericTest("Edges are built without error", [](){
        Corpus corpus = Corpus({{ {"c", "a", "b" }, { "a", "b" }, { "c", "a" }, { "b", "c" }, {"c", "b"}, {"c", "b"}, {}, {}, {}, {}, {}, {}, {} }}, 1);
        return true;
    });

    return failedTests;
}
