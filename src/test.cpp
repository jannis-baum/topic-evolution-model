#include <iostream>
#include "assert.h"

#include "test.hpp"
#include "Corpus.hpp"

int testAll() {
    int failedTests = 0;

    failedTests += testCorpusPeriodBasic();
    failedTests += testCorpusPeriodBasic();

    std::cout << "DONE! failed: " << failedTests << std::endl;
    return failedTests;
}

int testCorpusPeriodBasic() {
    std::cout << "Subword should point to superword ";
    // empty vectors to make sure occurrence rate < 50%
    Corpus corpus = Corpus({{ { "a", "b" }, { "b" }, {}, {}, {} }});
    bool success = corpus.periods[0].wtonode.at(0).word == corpus.periods[0].wtonode.at(1).word;
    std::cout << (success ? "✅" : "❌") << std::endl;
    return !success;
}

int testCorpusPeriodChain() {
    std::cout << "Chains of subwords should point to common superword ";
    Corpus corpus = Corpus({{ {"c", "a", "b" }, { "a", "b" }, { "b" }, {}, {}, {} }});

    bool success = corpus.periods[0].wtonode.at(0).word == 2 
        && corpus.periods[0].wtonode.at(1).word == 2
        && corpus.periods[0].wtonode.at(2).word == 2;
    std::cout << (success ? "✅" : "❌") << std::endl;
    return !success;
}
