#include <iostream>
#include <unordered_map>

#include "tests.hpp"
#include "../Corpus.hpp"

class MockCorpus: public Corpus {
    const std::unordered_map<word_t, SemanticNode> &wtonodeByPeriod(const int s) const override {
        return this->mock_wtonode;
    }

    std::vector<word_t> findEmergingWords(const int s) const override {
        return this->mock_emergingWords;
    };

    public:
        std::unordered_map<word_t, SemanticNode> mock_wtonode;
        std::vector<word_t> mock_emergingWords;

        MockCorpus(
            const int testingCase,
            const dec_t c = 1,
            const dec_t alpha = 0,
            const dec_t beta = 0,
            const dec_t gamma = 0,
            const int theta = 1,
            const dec_t mergeThreshold = 1
        ) : Corpus(c, alpha, beta, gamma, theta, mergeThreshold) {
            switch (testingCase) {
                case 5:
                    this->mock_wtonode.emplace(0, SemanticNode(0, {}));
                    this->mock_wtonode.emplace(1, SemanticNode(1, {}));
                    this->mock_wtonode.emplace(2, SemanticNode(2, {}));
                    this->mock_wtonode.emplace(3, SemanticNode(3, {}));
                    this->mock_wtonode.emplace(4, SemanticNode(4, {}));
                    //this->mock_wtonode.emplace(5, SemanticNode(5, {}));
                    this->topicbyperiod.push_back({
                        Topic({ &(this->mock_wtonode.at(0)), &(this->mock_wtonode.at(1)), &(this->mock_wtonode.at(2))})
                        });
                    this->topicbyperiod.push_back({
                        Topic({ &(this->mock_wtonode.at(0)), &(this->mock_wtonode.at(1)), &(this->mock_wtonode.at(2))}),
                        Topic({ &(this->mock_wtonode.at(2)), &(this->mock_wtonode.at(3)), &(this->mock_wtonode.at(4))})
                    });
                    break;
                case 4:
                    this->mock_wtonode.emplace(0, SemanticNode(0, {}));
                    this->mock_wtonode.emplace(1, SemanticNode(1, {}));
                    this->mock_wtonode.emplace(2, SemanticNode(2, {}));
                    this->mock_wtonode.emplace(3, SemanticNode(3, {}));
                    this->mock_wtonode.emplace(4, SemanticNode(4, {}));
                    this->mock_wtonode.emplace(5, SemanticNode(5, {}));

                    this->topicbyperiod.push_back({
                        Topic({ &(this->mock_wtonode.at(0)), &(this->mock_wtonode.at(1)) }),
                        Topic({ &(this->mock_wtonode.at(0)), &(this->mock_wtonode.at(1)), &(this->mock_wtonode.at(3)), &(this->mock_wtonode.at(5)) }),
                        Topic({ &(this->mock_wtonode.at(3)), &(this->mock_wtonode.at(4)), &(this->mock_wtonode.at(2)) })
                    });
                    break;
                case 3:
                    // 0 -> 1 -> 2 -> 3 -> 0
                    this->mock_wtonode.emplace(0, SemanticNode(0, {}));
                    this->mock_wtonode.emplace(1, SemanticNode(1, {}));
                    this->mock_wtonode.emplace(2, SemanticNode(2, {}));
                    this->mock_wtonode.emplace(3, SemanticNode(3, {}));
                    this->mock_wtonode.at(0).neighbors.push_back({ 1, &(this->mock_wtonode.at(1)) });
                    this->mock_wtonode.at(1).neighbors.push_back({ 1, &(this->mock_wtonode.at(2)) });
                    this->mock_wtonode.at(2).neighbors.push_back({ 1, &(this->mock_wtonode.at(3)) });
                    this->mock_wtonode.at(3).neighbors.push_back({ 1, &(this->mock_wtonode.at(0)) });
                    this->mock_emergingWords = { 0 };
                    break;
                case 2:
                    // 0 <-> 1 <-> 2
                    this->mock_wtonode.emplace(0, SemanticNode(0, {}));
                    this->mock_wtonode.emplace(1, SemanticNode(1, {}));
                    this->mock_wtonode.emplace(2, SemanticNode(2, {}));
                    this->mock_wtonode.at(0).neighbors.push_back({ 1, &(this->mock_wtonode.at(1)) });
                    this->mock_wtonode.at(1).neighbors.push_back({ 1, &(this->mock_wtonode.at(0)) });
                    this->mock_wtonode.at(1).neighbors.push_back({ 1, &(this->mock_wtonode.at(2)) });
                    this->mock_wtonode.at(2).neighbors.push_back({ 1, &(this->mock_wtonode.at(1)) });
                    this->mock_emergingWords = { 0 };
                    break;
                case 1:
                    // 1 -> 0
                    this->mock_wtonode.emplace(0, SemanticNode(0, {}));
                    this->mock_wtonode.emplace(1, SemanticNode(1, { { 1, &(this->mock_wtonode.at(0)) } }));
                    this->mock_emergingWords = { 1 };
                    break;
                default:
                    // 0 <-> 1
                    this->mock_wtonode.emplace(0, SemanticNode(0, {}));
                    this->mock_wtonode.emplace(1, SemanticNode(1, { { 1, &(this->mock_wtonode.at(0)) } }));
                    this->mock_wtonode.at(0).neighbors.push_back({ 1, &(this->mock_wtonode.at(1)) });
                    this->mock_emergingWords = { 0 };
                    break;
            }
        }
};

int testCorpus() {
    int failedTests = 0;

    std::cout << std::endl << "TERM METRICS" << std::endl;
    std::cout << "Energy" << std::endl;

    failedTests += genericTest("Energy is zero for unchanged periods", [](){
        Corpus c = Corpus({
            { { "a" }, {}, {} },
            { { "a" }, {}, {} },
        }, 1, 1);
        return isEqual(c.energy(0, 1), 0);
    });

    failedTests += genericTest("Energy is calculated correctly", [](){
        Corpus c = Corpus({
            { { "a", "b", "b" }, {}, {} }, // a has nutrition 1/6
            { { "a", "a" }, {}, {} }, // a has nutrition 1/3
        }, 1, 1);
        return isEqual(c.energy(0, 1), 1.0/9 - 1.0/36);
    });

    std::cout << "ENR" << std::endl;

    failedTests += genericTest("ENR is calculated correctly", [](){
        Corpus c = Corpus({
            { { "a", "b", "b" }, {}, {} }, // a has nutrition 1/6
            { { "a", "a" }, {}, {} }, // a has nutrition 1/3
        }, 1, 1);
        return isEqual(c.enr(0, 1), (1.0/9 - 1.0/36) / (1.0/3));
    });

    std::cout << std::endl << "EMERGING WORDS & TOPICS" << std::endl;
    std::cout << "Words" << std::endl;

    failedTests += genericTest("Emerging terms are found correctly", [](){
        Corpus c = Corpus({
            { { "a", "b", "b", "c", "c", "c" }, {}, {} }, // c is flood word for alpha = 1
            { { "a", "a", "b", "b", "c", "c", "c" }, {}, {} }, // a gets higher energy
        }, 1, 1, 1, 0, 0);
        auto e = c.findEmergingWords(1);
        return e.size() == 1 && e[0] == 0;
    });

    std::cout << "Topics" << std::endl;

    failedTests += genericTest("Simple emerging topic with 2 nodes", [](){
        MockCorpus m = MockCorpus(0, 0, 0, 0, 0, 1, 0);
        // first 5 params are for finding emerging words (irrelevant), theta is
        // BFS depth & last would only be relevant if we had more than one
        // emerging word
        auto topics = m.findEmergingTopics(0);
        return topics.size() == 1
            && topics[0].size() == 2;
    });

    failedTests += genericTest("Missing back-connection", [](){
        MockCorpus m = MockCorpus(1, 0, 0, 0, 0, 1, 0);
        auto topics = m.findEmergingTopics(0);
        return topics.size() == 1
            && topics[0].size() == 1;
    });

    failedTests += genericTest("Connection with multi-node path but insufficient lambda", [](){
        MockCorpus m = MockCorpus(2, 0, 0, 0, 0, 1, 0);
        auto topics = m.findEmergingTopics(0);
        return topics.size() == 1
            && topics[0].size() == 2;
    });

    failedTests += genericTest("Connection with multi-node path but sufficient lambda", [](){
        MockCorpus m = MockCorpus(2, 0, 0, 0, 0, 2, 0);
        auto topics = m.findEmergingTopics(0);
        return topics.size() == 1
            && topics[0].size() == 3;
    });

    failedTests += genericTest("Insufficient lambda on backwards search", [](){
        MockCorpus m = MockCorpus(3, 0, 0, 0, 0, 2, 0);
        auto topics = m.findEmergingTopics(0);
        return topics.size() == 1
            && topics[0].size() == 2
            && topics[0].contains(&(m.mock_wtonode.at(0)))
            && topics[0].contains(&(m.mock_wtonode.at(2)));
    });

    failedTests += genericTest("Predecessors are found correctly", [](){
        MockCorpus m = MockCorpus(4);
        Topic topic = {&(m.mock_wtonode.at(3)), &(m.mock_wtonode.at(4)), &(m.mock_wtonode.at(5))};
        auto predecessor = m.findPredecessorTopic(topic, 1.0, 0);
        return predecessor.size() == 4;
    });

    failedTests += genericTest("Predecessor are chosen with threshold", [](){
        MockCorpus m = MockCorpus(4);
        Topic topic = {&(m.mock_wtonode.at(3)), &(m.mock_wtonode.at(4)), &(m.mock_wtonode.at(5))};
        auto predecessor = m.findPredecessorTopic(topic, 0, 0);
        return predecessor.size() == 0;
    });

    failedTests += genericTest("Topic Ids are assigned correctly", [](){
        MockCorpus m = MockCorpus(5);
        auto topicIds = m.getTopicIds(0.01);
        return (topicIds[0].second == 0 && topicsEqual(topicIds[0].first, m.topicbyperiod[0][0]))
            && (topicIds[1].second == 0 && topicsEqual(topicIds[1].first, m.topicbyperiod[1][0]))
            && (topicIds[2].second == 1 && topicsEqual(topicIds[2].first, m.topicbyperiod[1][1]));
    });

    return failedTests;
}
