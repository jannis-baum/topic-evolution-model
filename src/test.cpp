#include <iostream>
#include "assert.h"

#include "test.hpp"
#include "Corpus.hpp"

int testAll() {
    int failedTests = 0;

    std::cout << "CORPUS PERIOD" << std::endl;

    failedTests += genericTest("Subword should point to superword", [](){
        Corpus corpus = Corpus({{ { "a", "b" }, { "b" }, {}, {}, {} }});
        return corpus.periods[0].wtonode.at(0).word == corpus.periods[0].wtonode.at(1).word;
    });
    failedTests += genericTest("Chain of subwords should point to common superword", [](){
        Corpus corpus = Corpus({{ {"c", "a", "b" }, { "a", "b" }, { "b" }, {}, {}, {}, {} }});
        return corpus.periods[0].wtonode.at(0).word == 2
            && corpus.periods[0].wtonode.at(1).word == 2
            && corpus.periods[0].wtonode.at(2).word == 2;
    });

    std::cout << "DONE! failed: " << failedTests << std::endl;
    return failedTests;
}

int genericTest(std::string expectation, std::function<bool()> isSuccess) {
    std::cout << "- " << expectation;
    bool success = isSuccess();
    std::cout << " " << (success ? "✅" : "❌") << std::endl;
    return !success;
}
