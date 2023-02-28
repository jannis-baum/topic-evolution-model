#include <iostream>
#include <unordered_map>

#include "tests.hpp"
#include "../Corpus.hpp"

// helper to create wtonode ref map from value map
std::unordered_map<word_t, SemanticNode *> makeReferenceWtonode(std::unordered_map<word_t, SemanticNode> &wtonode) {
    std::unordered_map<word_t, SemanticNode *> ref_wtonode;
    for (auto &[w, node]: wtonode) {
        ref_wtonode.emplace(w, &node);
    }
    return ref_wtonode;
}

class MockCorpus2: public Corpus {

    dec_t energy(const word_t word, const int s) const override {
        return 1;
    }

    inline int nPeriods() const override {
        return this->periodCount;
    }

    public:
        std::unordered_map<word_t, SemanticNode> value_wtonode;
        std::unordered_map<word_t, SemanticNode *> mock_wtonode;
        int periodCount;

        MockCorpus2(
                const int periodCount,
                const dec_t c = 1,
                const dec_t alpha = 0,
                const dec_t beta = 0,
                const dec_t gamma = 0,
                const int theta = 1,
                const dec_t mergeThreshold = 1
            ) : Corpus(c, alpha, beta, gamma, theta, mergeThreshold) {
                this->periodCount = periodCount;
                this->value_wtonode.emplace(0, SemanticNode(0, {}));
                this->value_wtonode.emplace(1, SemanticNode(1, {}));
                this->value_wtonode.emplace(2, SemanticNode(2, {}));
                this->value_wtonode.emplace(3, SemanticNode(3, {}));
                this->value_wtonode.emplace(4, SemanticNode(4, {}));
                this->mock_wtonode = makeReferenceWtonode(this->value_wtonode);
                this->topicsByPeriod.push_back({
                    Topic({ this->mock_wtonode.at(0), this->mock_wtonode.at(1), this->mock_wtonode.at(2)})
                    });
                this->topicsByPeriod.push_back({
                    Topic({ this->mock_wtonode.at(0), this->mock_wtonode.at(1), this->mock_wtonode.at(2)}),
                    Topic({ this->mock_wtonode.at(2), this->mock_wtonode.at(3), this->mock_wtonode.at(4)})
                });
            }
};

class MockCorpus: public Corpus {
    const inline std::unordered_map<word_t, SemanticNode *> &wtonodeByPeriod(const int s) const override {
        return this->mock_wtonode;
    }

    std::optional<std::vector<word_t>> findEmergingWords(const int s) const override {
        return this->mock_emergingWords;
    };

    inline int nPeriods() const override {
        return this->periodCount;
    }

    public:
        std::unordered_map<word_t, SemanticNode> value_wtonode;
        std::unordered_map<word_t, SemanticNode *> mock_wtonode;
        std::vector<word_t> mock_emergingWords;
        int periodCount;

        MockCorpus(
            const int testingCase,
            const int periodNumber,
            const dec_t c = 1,
            const dec_t alpha = 0,
            const dec_t beta = 0,
            const dec_t gamma = 0,
            const int theta = 1,
            const dec_t mergeThreshold = 1
        ) : Corpus(c, alpha, beta, gamma, theta, mergeThreshold) {
            this->periodCount = periodNumber;
            switch (testingCase) {
                case 4:
                    this->value_wtonode.emplace(0, SemanticNode(0, {}));
                    this->value_wtonode.emplace(1, SemanticNode(1, {}));
                    this->value_wtonode.emplace(2, SemanticNode(2, {}));
                    this->value_wtonode.emplace(3, SemanticNode(3, {}));
                    this->value_wtonode.emplace(4, SemanticNode(4, {}));
                    this->value_wtonode.emplace(5, SemanticNode(5, {}));
                    this->mock_wtonode = makeReferenceWtonode(this->value_wtonode);

                    this->topicsByPeriod.push_back({
                        Topic({ this->mock_wtonode.at(0), this->mock_wtonode.at(1) }),
                        Topic({ this->mock_wtonode.at(0), this->mock_wtonode.at(1), this->mock_wtonode.at(3), this->mock_wtonode.at(5) }),
                        Topic({ this->mock_wtonode.at(3), this->mock_wtonode.at(4), this->mock_wtonode.at(2) })
                    });
                    break;
                case 3:
                    // 0 -> 1 -> 2 -> 3 -> 0
                    this->value_wtonode.emplace(0, SemanticNode(0, {}));
                    this->value_wtonode.emplace(1, SemanticNode(1, {}));
                    this->value_wtonode.emplace(2, SemanticNode(2, {}));
                    this->value_wtonode.emplace(3, SemanticNode(3, {}));
                    this->mock_wtonode = makeReferenceWtonode(this->value_wtonode);
                    this->mock_wtonode.at(0)->neighbors.push_back({ 1, this->mock_wtonode.at(1) });
                    this->mock_wtonode.at(1)->neighbors.push_back({ 1, this->mock_wtonode.at(2) });
                    this->mock_wtonode.at(2)->neighbors.push_back({ 1, this->mock_wtonode.at(3) });
                    this->mock_wtonode.at(3)->neighbors.push_back({ 1, this->mock_wtonode.at(0) });
                    this->mock_emergingWords = { 0 };
                    break;
                case 2:
                    // 0 <-> 1 <-> 2
                    this->value_wtonode.emplace(0, SemanticNode(0, {}));
                    this->value_wtonode.emplace(1, SemanticNode(1, {}));
                    this->value_wtonode.emplace(2, SemanticNode(2, {}));
                    this->mock_wtonode = makeReferenceWtonode(this->value_wtonode);
                    this->mock_wtonode.at(0)->neighbors.push_back({ 1, this->mock_wtonode.at(1) });
                    this->mock_wtonode.at(1)->neighbors.push_back({ 1, this->mock_wtonode.at(0) });
                    this->mock_wtonode.at(1)->neighbors.push_back({ 1, this->mock_wtonode.at(2) });
                    this->mock_wtonode.at(2)->neighbors.push_back({ 1, this->mock_wtonode.at(1) });
                    this->mock_emergingWords = { 0 };
                    break;
                case 1:
                    // 1 -> 0
                    this->value_wtonode.emplace(0, SemanticNode(0, {}));
                    this->value_wtonode.emplace(1, SemanticNode(1, {}));
                    this->mock_wtonode = makeReferenceWtonode(this->value_wtonode);
                    this->mock_wtonode.at(1)->neighbors.push_back({ 1, this->mock_wtonode.at(0) });
                    this->mock_emergingWords = { 1 };
                    break;
                default:
                    // 0 <-> 1
                    this->value_wtonode.emplace(0, SemanticNode(0, {}));
                    this->value_wtonode.emplace(1, SemanticNode(1, {}));
                    this->mock_wtonode = makeReferenceWtonode(this->value_wtonode);
                    this->mock_wtonode.at(0)->neighbors.push_back({ 1, this->mock_wtonode.at(1) });
                    this->mock_wtonode.at(1)->neighbors.push_back({ 1, this->mock_wtonode.at(0) });
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
        return e
            && (*e).size() == 1
            && (*e)[0] == 0;
    });

    std::cout << "Topics" << std::endl;

    failedTests += genericTest("Simple emerging topic with 2 nodes", [](){
        MockCorpus m = MockCorpus(0, 1, 0, 0, 0, 0, 1, 0);
        // first 5 params are for finding emerging words (irrelevant), theta is
        // BFS depth & last would only be relevant if we had more than one
        // emerging word
        auto topics = m.findEmergingTopics(0);
        return topics
            && (*topics).size() == 1
            && (*topics)[0].size() == 2;
    });

    failedTests += genericTest("Missing back-connection", [](){
        MockCorpus m = MockCorpus(1, 1, 0, 0, 0, 0, 1, 0);
        auto topics = m.findEmergingTopics(0);
        return topics
            && (*topics).size() == 1
            && (*topics)[0].size() == 1;
    });

    failedTests += genericTest("Connection with multi-node path but insufficient lambda", [](){
        MockCorpus m = MockCorpus(2, 1, 0, 0, 0, 0, 1, 0);
        auto topics = m.findEmergingTopics(0);
        return topics
            && (*topics).size() == 1
            && (*topics)[0].size() == 2;
    });

    failedTests += genericTest("Connection with multi-node path but sufficient lambda", [](){
        MockCorpus m = MockCorpus(2, 1, 0, 0, 0, 0, 2, 0);
        auto topics = m.findEmergingTopics(0);
        return topics
            && (*topics).size() == 1
            && (*topics)[0].size() == 3;
    });

    failedTests += genericTest("Insufficient lambda on backwards search", [](){
        MockCorpus m = MockCorpus(3, 1, 0, 0, 0, 0, 2, 0);
        auto topics = m.findEmergingTopics(0);
        return topics
            && (*topics).size() == 1
            && (*topics)[0].size() == 2
            && (*topics)[0].contains(m.mock_wtonode.at(0))
            && (*topics)[0].contains(m.mock_wtonode.at(2));
    });

    failedTests += genericTest("Predecessors are found correctly", [](){
        MockCorpus m = MockCorpus(4, 2);
        Topic topic = {m.mock_wtonode.at(3), m.mock_wtonode.at(4), m.mock_wtonode.at(5)};

        auto predecessorOpt = m.findPredecessorTopic(topic, 1.0, 1);
        if (!predecessorOpt) return false;

        const Topic *predecessor = predecessorOpt.value();
        return predecessor->size() == 4;
    });

    failedTests += genericTest("Predecessor are chosen with threshold", [](){
        MockCorpus m = MockCorpus(4, 2);
        Topic topic = {m.mock_wtonode.at(3), m.mock_wtonode.at(4), m.mock_wtonode.at(5)};
        auto predecessor = m.findPredecessorTopic(topic, 0, 1);
        return !predecessor;
    });

    failedTests += genericTest("Topic evolution is found correctly", [](){
        MockCorpus2 m = MockCorpus2(2);
        auto topicIds = m.getTopicEvolution(0.01);
        return (std::get<1>(topicIds[0][0]) == 0 && std::get<0>(topicIds[0][0]) == (m.topicsByPeriod[0])[0])
            && (std::get<1>(topicIds[1][0]) == 0 && std::get<0>(topicIds[1][0]) == (m.topicsByPeriod[1])[0])
            && (std::get<1>(topicIds[1][1]) == 1 && std::get<0>(topicIds[1][1]) == (m.topicsByPeriod[1])[1]);
    });

    return failedTests;
}
