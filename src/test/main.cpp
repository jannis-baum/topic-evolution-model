#include <iostream>

#include "tests.hpp"

int testAll() {
    int failedTests = 0;

    failedTests += testPeriod();
    failedTests += testCorpus();
    failedTests += testTopics();
    failedTests += testYaml();

    std::cout << std::endl
        << "--------------------------------------------------------------------------------"
        << std::endl;
    if (!failedTests) {
        std::cout << "✅ Done! All tests passed successfully." << std::endl;
    } else {
        std::cout << "❌ Done! " << failedTests << " tests failed." << std::endl;
    }
    std::cout
        << "--------------------------------------------------------------------------------"
        << std::endl << std::endl;

    return failedTests;
}

int genericTest(std::string expectation, std::function<bool()> isSuccess) {
    std::cout << "- " << expectation;
    bool success = isSuccess();
    std::cout << " " << (success ? "✅" : "❌") << std::endl;
    return !success;
}
