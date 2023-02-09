#include <iostream>

#include "tests.hpp"
#include "../Corpus.hpp"

int testCorpus() {
    int failedTests = 0;

    std::cout << std::endl << "TERM METRICS" << std::endl;
    std::cout << "Energy" << std::endl;

    failedTests += genericTest("Energy is zero for unchanged periods", [](){
        Corpus c = Corpus({
            { { "a" }, {}, {} },
            { { "a" }, {}, {} },
        }, 1);
        return isEqual(c.energy(0, 1, 1), 0);
    });

    failedTests += genericTest("Energy is calculated correctly", [](){
        Corpus c = Corpus({
            { { "a", "b", "b" }, {}, {} }, // a has nutrition 1/6
            { { "a", "a" }, {}, {} }, // a has nutrition 1/3
        }, 1);
        return isEqual(c.energy(0, 1, 1), 1.0/9 - 1.0/36);
    });

    std::cout << "ENR" << std::endl;

    failedTests += genericTest("ENR is calculated correctly", [](){
        Corpus c = Corpus({
            { { "a", "b", "b" }, {}, {} }, // a has nutrition 1/6
            { { "a", "a" }, {}, {} }, // a has nutrition 1/3
        }, 1);
        return isEqual(c.enr(0, 1, 1), (1.0/9 - 1.0/36) / (1.0/3));
    });

    return failedTests;
}
