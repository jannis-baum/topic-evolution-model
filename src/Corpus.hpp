#ifndef CORPUS_HPP
#define CORPUS_HPP

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "CorpusPeriod.hpp"
#include "SemanticGraph.hpp"
#include "topics.hpp"

class Corpus {
    // word_t (aka int) to string mapping
    std::unordered_map<word_t, std::string> wtostr;

    // s is period index
    virtual const std::unordered_map<word_t, SemanticNode> &wtonodeByPeriod(const int s) const;

    public:
        std::vector<CorpusPeriod> periods;
        std::vector<std::vector<Topic>> topicbyperiod;

        // () constructor for testing
        Corpus(): periods({}), wtostr({}) {};
        // construct Corpus from vector (periods) of vectors (documents) of
        // strings (words)
        Corpus(const std::vector<std::vector<std::vector<std::string>>> structuredCorpus, const dec_t delta);

        // see definitions.md or paper
        dec_t energy(const word_t word, const int s, const dec_t c) const;
        // ENR (energy-nutrition-ratio), see definitions.md or paper
        dec_t enr(const word_t word, const int s, const dec_t c) const;

        // see definitions.md or paper
        virtual std::vector<word_t> findEmergingWords(
            const int s,
            const dec_t c,
            const dec_t alpha,
            const dec_t beta,
            const dec_t gamma
        ) const;
        // see definitions.md or paper
        std::vector<Topic> findEmergingTopics(
            const int s,
            const dec_t c,
            const dec_t alpha,
            const dec_t beta,
            const dec_t gamma,
            const int theta,
            const dec_t mergeThreshold
        ) const;

        void findEmergingTopicsByPeriod(
            const dec_t c,
            const dec_t alpha,
            const dec_t beta,
            const dec_t gamma,
            const int theta,
            const dec_t mergeThreshold
        );

        //see definitions.md or paper
        bool isPersistent(Topic topic, int s, const dec_t c) const;

        Topic findPredecessorTopic(Topic topic, const dec_t distance_threshold, int s) const;

        // streaming (e.g. printing) operator <<
        friend std::ostream& operator<<(std::ostream& os, Corpus const &corpus) {
            for (const auto period : corpus.periods) os << period << std::endl;
            return os;
        };
};

#endif
