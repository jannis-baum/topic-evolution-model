#include <iostream>
#include <string>

#include "../yaml.hpp"
#include "tests.hpp"

int testYaml() {
    int failedTests = 0;

    std::cout << std::endl << "YAML" << std::endl;

    failedTests += genericTest("Correctly dumps arrays of dictionaries", []() {
        std::vector<int> data = { 1, 2 };
        auto dump = dumpVector<int>(data, [](const int &element) {
            return "first: " + std::to_string(element) +
                   "\nsecond: " + std::to_string(element) + "\n";
        });
        return dump == "- first: 1\n  second: 1\n- first: 2\n  second: 2\n";
    });

    return failedTests;
}
