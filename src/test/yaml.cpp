#include <iostream>
#include <string>
#include <tuple>
#include <unordered_set>

#include "../yaml.hpp"
#include "tests.hpp"

int testYaml() {
    int failedTests = 0;

    std::cout << std::endl << "YAML" << std::endl;

    failedTests += genericTest("Correctly dumps vectors", []() {
        const std::vector<int> data = { 1, 2 };
        const auto dump = dumpIterable(data, [](const int &element) {
            return "first: " + std::to_string(element) +
                   "\nsecond: " + std::to_string(element) + "\n";
        });
        return dump == "- first: 1\n  second: 1\n- first: 2\n  second: 2\n";
    });

    failedTests += genericTest("Correctly dumps sets", []() {
        const std::unordered_set<int> data = { 1, 2 };
        const auto dump = dumpIterable(data, [](const int &element) {
            return "first: " + std::to_string(element) +
                   "\nsecond: " + std::to_string(element) + "\n";
        });
        return dump == "- first: 1\n  second: 1\n- first: 2\n  second: 2\n"
            || dump == "- first: 2\n  second: 2\n- first: 1\n  second: 1\n";
    });

    failedTests += genericTest("Correctly dumps tuples", []() {
        const auto data = std::make_tuple(1, 2);
        auto dump = dumpTuple(data, { "label1", "label2" });
        return dump == "label1: 1\nlabel2: 2\n";
    });

    return failedTests;
}
