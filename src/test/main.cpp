#include <iostream>

#include "tests.hpp"

int testAll() {
    int failed = 0;

    failed += testPeriod();
    failed += testCorpus();
    failed += testTopics();
    failed += testYaml();

    std::cout << std::endl
        << "--------------------------------------------------------------------------------"
        << std::endl;
    if (!failed) {
        std::cout << "✅ Done! All tests passed successfully." << std::endl;
    } else {
        std::cout << "❌ Done! " << failed << " tests failed." << std::endl;
    }
    std::cout
        << "--------------------------------------------------------------------------------"
        << std::endl << std::endl;

    return failed;
}

int genericTest(std::string expectation, std::function<bool()> is_success) {
    std::cout << "- " << expectation;
    bool success = is_success();
    std::cout << " " << (success ? "✅" : "❌") << std::endl;
    return !success;
}
