#include <cmath>
#include <iostream>
#include "assert.h"

#include "test.hpp"
#include "Corpus.hpp"
#include "topics.hpp"

int testGraphConstruction();
int testTopics();

int testAll() {
    int failedTests = 0;
    failedTests += testGraphConstruction();
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

int testTopics() {
    int failedTests = 0;

    std::cout << std::endl << "TOPICS" << std::endl;
    std::cout << "Distances" << std::endl;

    failedTests += genericTest("Topic distance is inf", [](){
        SemanticNode node1(1, {});
        SemanticNode node2(2, {});
        Topic t1 = { &node1 };
        Topic t2 = { &node2 };
        return isinf(topicDistance(t1, t2));
    });

    failedTests += genericTest("Topic distance is 0", [](){
        SemanticNode node1(1, {});
        Topic t1 = { &node1 };
        Topic t2 = { &node1 };
        return isEqual(topicDistance(t1, t2), 0);
    });

    failedTests += genericTest("Topic distance is 1", [](){
        SemanticNode node1(1, {});
        SemanticNode node2(2, {});
        SemanticNode node3(3, {});
        Topic t1 = { &node1, &node2 };
        Topic t2 = { &node2, &node3 };
        return isEqual(topicDistance(t1, t2), 1);
    });

    std::cout << "Merging" << std::endl;

    failedTests += genericTest("Merge two topics", [](){
        SemanticNode node1(1, {});
        SemanticNode node2(2, {});
        std::vector<Topic> topics = { Topic({ &node1 }), Topic({ &node2 }) };
        mergeTopics(topics, { topics.begin(), topics.begin() + 1 });
        return topics.size() == 1
            && topics.begin()->size() == 2
            && topics.begin()->contains(&node2);
    });

    return failedTests;
}
