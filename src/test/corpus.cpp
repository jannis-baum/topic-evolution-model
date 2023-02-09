#include <iostream>
#include <unordered_map>

#include "tests.hpp"
#include "../Corpus.hpp"

class MockCorpus: public Corpus {
    std::unordered_map<word_t, SemanticNode> mock_wtonode;
    std::vector<word_t> mock_emergingWords;

    const std::unordered_map<word_t, SemanticNode> &wtonodeByPeriod(const int s) const override {
        return this->mock_wtonode;
    }

    std::vector<word_t> findEmergingWords(
        const int s,
        const dec_t c,
        const dec_t alpha,
        const dec_t beta,
        const dec_t gamma
    ) const override {
        return this->mock_emergingWords;
    };

    public:
        MockCorpus(const int testingCase): Corpus() {
            switch (testingCase) {
                default:
                    // two inter-connected nodes, 0 is emerging
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
        }, 1);
        return isEqual(c.energy(0, 1, 1), 0);
    });

    failedTests += genericTest("Energy is calculated correctly", [](){
        Corpus c = Corpus({
            { { "a", "b", "b" }, {}, {} }, // a has nutrition 1/6
            { { "a", "a" }, {}, {} }, // a has nutrition 1/3
        }, 1);
        return isEqual(c.energy(0, 1, 1), 1.0/9 - 1.0/36);
    });

    std::cout << "ENR" << std::endl;

    failedTests += genericTest("ENR is calculated correctly", [](){
        Corpus c = Corpus({
            { { "a", "b", "b" }, {}, {} }, // a has nutrition 1/6
            { { "a", "a" }, {}, {} }, // a has nutrition 1/3
        }, 1);
        return isEqual(c.enr(0, 1, 1), (1.0/9 - 1.0/36) / (1.0/3));
    });

    std::cout << std::endl << "EMERGING WORDS & TOPICS" << std::endl;
    std::cout << "Words" << std::endl;

    failedTests += genericTest("Emerging terms are found correctly", [](){
        Corpus c = Corpus({
            { { "a", "b", "b", "c", "c", "c" }, {}, {} }, // c is flood word for alpha = 1
            { { "a", "a", "b", "b", "c", "c", "c" }, {}, {} }, // a gets higher energy
        }, 1);
        auto e = c.findEmergingWords(1, 1, 1, 0, 0);
        return e.size() == 1 && e[0] == 0;
    });

    std::cout << "Topics" << std::endl;

    failedTests += genericTest("Simplest emerging topic", [](){
        MockCorpus m = MockCorpus(0);
        // first 5 params are for finding emerging words (irrelevant), theta is
        // BFS depth & last would only be relevant if we had more than one
        // emerging word
        auto topics = m.findEmergingTopics(0, 0, 0, 0, 0, 1, 0);
        return topics.size() == 1
            && topics[0].size() == 2;
    });

    return failedTests;
}
