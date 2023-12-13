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
        return this->period_count;
    }

    public:
        std::unordered_map<word_t, SemanticNode> value_wtonode;
        std::unordered_map<word_t, SemanticNode *> mock_wtonode;
        int period_count;

        MockCorpus2(
                const int period_count,
                const dec_t c = 1,
                const dec_t alpha = 0,
                const dec_t beta = 0,
                const dec_t gamma = 0,
                const int theta = 1,
                const dec_t merge_threshold = 1,
                const dec_t evolution_threshold = 0.01
            ) : Corpus(c, alpha, beta, gamma, theta, merge_threshold, evolution_threshold) {
                this->period_count = period_count;
                this->value_wtonode.emplace(0, SemanticNode(0, {}));
                this->value_wtonode.emplace(1, SemanticNode(1, {}));
                this->value_wtonode.emplace(2, SemanticNode(2, {}));
                this->value_wtonode.emplace(3, SemanticNode(3, {}));
                this->value_wtonode.emplace(4, SemanticNode(4, {}));
                this->mock_wtonode = makeReferenceWtonode(this->value_wtonode);
                this->topics_by_period.push_back({
                    Topic({ this->mock_wtonode.at(0), this->mock_wtonode.at(1), this->mock_wtonode.at(2)})
                    });
                this->topics_by_period.push_back({
                    Topic({ this->mock_wtonode.at(0), this->mock_wtonode.at(1), this->mock_wtonode.at(2)}),
                    Topic({ this->mock_wtonode.at(2), this->mock_wtonode.at(3), this->mock_wtonode.at(4)})
                });
            }
};

class MockCorpus: public Corpus {
    const inline std::unordered_map<word_t, SemanticNode *> &wtonode_by_period(const int s) const override {
        return this->mock_wtonode;
    }

    std::optional<std::vector<word_t>> findEmergingWords(const int s) const override {
        return this->mock_emerging_words;
    };

    inline int nPeriods() const override {
        return this->period_count;
    }

    public:
        std::unordered_map<word_t, SemanticNode> value_wtonode;
        std::unordered_map<word_t, SemanticNode *> mock_wtonode;
        std::vector<word_t> mock_emerging_words;
        int period_count;

        MockCorpus(
            const int testing_case,
            const int period_number,
            const dec_t c = 1,
            const dec_t alpha = 0,
            const dec_t beta = 0,
            const dec_t gamma = 0,
            const int theta = 1,
            const dec_t merge_threshold = 1,
            const dec_t evolution_threshold = 0.01
        ) : Corpus(c, alpha, beta, gamma, theta, merge_threshold, evolution_threshold) {
            this->period_count = period_number;
            switch (testing_case) {
                case 4:
                    this->value_wtonode.emplace(0, SemanticNode(0, {}));
                    this->value_wtonode.emplace(1, SemanticNode(1, {}));
                    this->value_wtonode.emplace(2, SemanticNode(2, {}));
                    this->value_wtonode.emplace(3, SemanticNode(3, {}));
                    this->value_wtonode.emplace(4, SemanticNode(4, {}));
                    this->value_wtonode.emplace(5, SemanticNode(5, {}));
                    this->mock_wtonode = makeReferenceWtonode(this->value_wtonode);

                    this->topics_by_period.push_back({
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
                    this->mock_emerging_words = { 0 };
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
                    this->mock_emerging_words = { 0 };
                    break;
                case 1:
                    // 1 -> 0
                    this->value_wtonode.emplace(0, SemanticNode(0, {}));
                    this->value_wtonode.emplace(1, SemanticNode(1, {}));
                    this->mock_wtonode = makeReferenceWtonode(this->value_wtonode);
                    this->mock_wtonode.at(1)->neighbors.push_back({ 1, this->mock_wtonode.at(0) });
                    this->mock_emerging_words = { 1 };
                    break;
                default:
                    // 0 <-> 1
                    this->value_wtonode.emplace(0, SemanticNode(0, {}));
                    this->value_wtonode.emplace(1, SemanticNode(1, {}));
                    this->mock_wtonode = makeReferenceWtonode(this->value_wtonode);
                    this->mock_wtonode.at(0)->neighbors.push_back({ 1, this->mock_wtonode.at(1) });
                    this->mock_wtonode.at(1)->neighbors.push_back({ 1, this->mock_wtonode.at(0) });
                    this->mock_emerging_words = { 0 };
                    break;
            }

        }
};

int testCorpus() {
    int failed = 0;

    std::cout << std::endl << "TERM METRICS" << std::endl;
    std::cout << "Energy" << std::endl;

    failed += genericTest("Energy is zero for unchanged periods", [](){
        Corpus c = Corpus({
            { { "a" }, {}, {} },
            { { "a" }, {}, {} },
        }, 1, 1);
        return isEqual(c.energy(0, 1), 0);
    });

    failed += genericTest("Energy is calculated correctly", [](){
        Corpus c = Corpus({
            { { "a", "b", "b" }, {}, {} }, // a has nutrition 1/6
            { { "a", "a" }, {}, {} }, // a has nutrition 1/3
        }, 1, 1);
        return isEqual(c.energy(0, 1), 1.0/9 - 1.0/36);
    });

    std::cout << "ENR" << std::endl;

    failed += genericTest("ENR is calculated correctly", [](){
        Corpus c = Corpus({
            { { "a", "b", "b" }, {}, {} }, // a has nutrition 1/6
            { { "a", "a" }, {}, {} }, // a has nutrition 1/3
        }, 1, 1);
        return isEqual(c.enr(0, 1), (1.0/9 - 1.0/36) / (1.0/3));
    });

    std::cout << std::endl << "EMERGING WORDS & TOPICS" << std::endl;
    std::cout << "Words" << std::endl;

    failed += genericTest("Emerging terms are found correctly", [](){
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

    failed += genericTest("Simple emerging topic with 2 nodes", [](){
        MockCorpus m = MockCorpus(0, 1, 0, 0, 0, 0, 1, 0);
        // first 5 params are for finding emerging words (irrelevant), theta is
        // BFS depth & last would only be relevant if we had more than one
        // emerging word
        auto topics = m.findEmergingTopics(0);
        return topics
            && (*topics).size() == 1
            && (*topics)[0].size() == 2;
    });

    failed += genericTest("Missing back-connection", [](){
        MockCorpus m = MockCorpus(1, 1, 0, 0, 0, 0, 1, 0);
        auto topics = m.findEmergingTopics(0);
        return topics
            && (*topics).size() == 1
            && (*topics)[0].size() == 1;
    });

    failed += genericTest("Connection with multi-node path but insufficient lambda", [](){
        MockCorpus m = MockCorpus(2, 1, 0, 0, 0, 0, 1, 0);
        auto topics = m.findEmergingTopics(0);
        return topics
            && (*topics).size() == 1
            && (*topics)[0].size() == 2;
    });

    failed += genericTest("Connection with multi-node path but sufficient lambda", [](){
        MockCorpus m = MockCorpus(2, 1, 0, 0, 0, 0, 2, 0);
        auto topics = m.findEmergingTopics(0);
        return topics
            && (*topics).size() == 1
            && (*topics)[0].size() == 3;
    });

    failed += genericTest("Insufficient lambda on backwards search", [](){
        MockCorpus m = MockCorpus(3, 1, 0, 0, 0, 0, 2, 0);
        auto topics = m.findEmergingTopics(0);
        return topics
            && (*topics).size() == 1
            && (*topics)[0].size() == 2
            && (*topics)[0].contains(m.mock_wtonode.at(0))
            && (*topics)[0].contains(m.mock_wtonode.at(2));
    });

    failed += genericTest("Predecessors are found correctly", [](){
        MockCorpus m = MockCorpus(4, 2);
        Topic topic = {m.mock_wtonode.at(3), m.mock_wtonode.at(4), m.mock_wtonode.at(5)};

        auto predecessor_opt = m.findPredecessorTopic(topic, 1.0, 1);
        if (!predecessor_opt) return false;

        const Topic *predecessor = predecessor_opt.value();
        return predecessor->size() == 4;
    });

    failed += genericTest("Predecessor are chosen with threshold", [](){
        MockCorpus m = MockCorpus(4, 2);
        Topic topic = {m.mock_wtonode.at(3), m.mock_wtonode.at(4), m.mock_wtonode.at(5)};
        auto predecessor = m.findPredecessorTopic(topic, 0, 1);
        return !predecessor;
    });

    failed += genericTest("Topic evolution is found correctly", [](){
        MockCorpus2 m = MockCorpus2(2);
        auto topic_ids = m.getTopicEvolution();
        return (std::get<1>(topic_ids[0][0]) == 0 && std::get<0>(topic_ids[0][0]) == (m.topics_by_period[0])[0])
            && (std::get<1>(topic_ids[1][0]) == 0 && std::get<0>(topic_ids[1][0]) == (m.topics_by_period[1])[0])
            && (std::get<1>(topic_ids[1][1]) == 1 && std::get<0>(topic_ids[1][1]) == (m.topics_by_period[1])[1]);
    });

    return failed;
}
