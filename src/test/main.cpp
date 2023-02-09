#include <iostream>

#include "tests.hpp"

int testAll() {
    int failedTests = 0;

    failedTests += testPeriod();
    failedTests += testTopics();

    std::cout << "DONE! failed: " << failedTests << std::endl;
    return failedTests;
}

int genericTest(std::string expectation, std::function<bool()> isSuccess) {
    std::cout << "- " << expectation;
    bool success = isSuccess();
    std::cout << " " << (success ? "✅" : "❌") << std::endl;
    return !success;
}
