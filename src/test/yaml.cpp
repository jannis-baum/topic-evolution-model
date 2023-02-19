#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

#include "../yaml.hpp"
#include "../SemanticGraph.hpp"
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
        // check both possible orders for unordered_set
        return dump == "- first: 1\n  second: 1\n- first: 2\n  second: 2\n"
            || dump == "- first: 2\n  second: 2\n- first: 1\n  second: 1\n";
    });

    failedTests += genericTest("Correctly dumps tuples", []() {
        const auto data = std::make_tuple(1, 2);
        auto dump = dumpTuple(data, { "label1", "label2" });
        return dump == "label1: 1\nlabel2: 2\n";
    });

    failedTests += genericTest("Correctly dumps topic evolution", []() {
        const std::unordered_map<word_t, std::string> wtostr = {
            { 0, "a" }, { 1, "b" }
        };
        const SemanticNode node0(0, {});
        const SemanticNode node1(1, {});
        const std::vector<std::vector<TopicData>> evolution = {
            { std::make_tuple(Topic({ &node0 }), 0, 1), std::make_tuple(Topic({ &node1 }), 1, 1) },
            { std::make_tuple(Topic({ &node0, &node1 }), 0, 1) }
        };

        auto dump = dumpTopicEvolution(evolution, wtostr);
        // check both possible orders for unordered_set
        return dump == "- - topic:\n    - a\n    id: 0\n    health: 1.000000\n  - topic:\n    - b\n    id: 1\n    health: 1.000000\n- - topic:\n    - b\n    - a\n    id: 0\n    health: 1.000000\n"
            || dump == "- - topic:\n    - a\n    id: 0\n    health: 1.000000\n  - topic:\n    - b\n    id: 1\n    health: 1.000000\n- - topic:\n    - a\n    - b\n    id: 0\n    health: 1.000000\n";
    });

    return failedTests;
}
