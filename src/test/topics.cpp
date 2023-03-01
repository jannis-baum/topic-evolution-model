#include <cmath>
#include <iostream>

#include "tests.hpp"
#include "../topics.hpp"

int testTopics() {
    int failedTests = 0;

    std::cout << std::endl << "TOPICS" << std::endl;
    std::cout << "Distances" << std::endl;

    failedTests += genericTest("Topic distance is inf", [](){
        SemanticNode node1(1, {});
        SemanticNode node2(2, {});
        Topic t1 = { &node1 };
        Topic t2 = { &node2 };
        return std::isinf(topicDistance(t1, t2));
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
        return topics.size() == 2
            && topics.begin()->size() == 2
            && topics.begin()->contains(&node2);
    });

    failedTests += genericTest("Simple merge by threshold I", [](){
        SemanticNode node1(1, {});
        SemanticNode node2(2, {});
        SemanticNode node3(3, {});
        SemanticNode node4(4, {});
        std::vector<Topic> topics = {
            Topic({ &node4 }),
            Topic({ &node1, &node2 }),
            Topic({ &node2, &node3 })
        };
        mergeTopicsByThreshold(topics, 1);
        return topics.size() == 2
            && topics.begin()->size() == 1
            && (topics.begin() + 1)->size() == 3;
    });

    failedTests += genericTest("Simple merge by threshold II", [](){
        SemanticNode node1(1, {});
        SemanticNode node2(2, {});
        SemanticNode node3(3, {});
        SemanticNode node4(4, {});
        std::vector<Topic> topics = {
            Topic({ &node1, &node2 }),
            Topic({ &node2, &node3, &node4 }),
            Topic({ &node3, &node4 }),
        };
        mergeTopicsByThreshold(topics, 0);
        return topics.size() == 2
            && topics.begin()->size() == 2
            && (topics.begin() + 1)->size() == 3;
    });

    failedTests += genericTest("Recursive merge by threshold", [](){
        SemanticNode node1(1, {});
        SemanticNode node2(2, {});
        SemanticNode node3(3, {});
        SemanticNode node4(4, {});
        std::vector<Topic> topics = {
            Topic({ &node1, &node2 }),
            Topic({ &node2, &node3, &node4 }),
            Topic({ &node3, &node4 }),
        };
        mergeTopicsByThreshold(topics, 1);
        return topics.size() == 1
            && topics.begin()->size() == 4;
    });

    failedTests += genericTest("Merge many topics", [](){
        SemanticNode node1(1, {});
        std::vector<Topic> topics = {
            Topic({ &node1 }),
            Topic({ &node1 }),
            Topic({ &node1 }),
            Topic({ &node1 }),
            Topic({ &node1 }),
            Topic({ &node1 }),
        };
        mergeTopicsByThreshold(topics, 1);
        return topics.size() == 1
            && topics.begin()->size() == 1;
    });

    return failedTests;
}
