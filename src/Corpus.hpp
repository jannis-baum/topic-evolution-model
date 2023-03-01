#ifndef CORPUS_HPP
#define CORPUS_HPP

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <optional>

#include "CorpusPeriod.hpp"
#include "SemanticGraph.hpp"
#include "topics.hpp"

// Topic, ID, health
typedef std::tuple<Topic, int, dec_t> TopicData;

class Corpus {
    // tuning parameters (see definitions.md or paper)
    const dec_t c;
    const dec_t alpha;
    const dec_t beta;
    const dec_t gamma;
    const int theta;
    const dec_t mergeThreshold;

    // s is period index
    virtual inline const std::unordered_map<word_t, SemanticNode *> &wtonodeByPeriod(const int s) const {
        return this->periods[s].wtonode;
    };

    inline bool periodExists(const int s) const {
        return s < this->nPeriods() && s >= 0;
    }

    public:
        // word_t (aka int) to string mapping
        std::unordered_map<word_t, std::string> wtostr;

        std::vector<CorpusPeriod> periods;
        std::vector<std::vector<Topic>> topicsByPeriod;

        // constructor for testing
        Corpus(
            const dec_t c,
            const dec_t alpha,
            const dec_t beta,
            const dec_t gamma,
            const int theta,
            const dec_t mergeThreshold
        ):
            periods({}), wtostr({}),
            c(c), alpha(alpha), beta(beta), gamma(gamma), theta(theta), mergeThreshold(mergeThreshold) {};
        // construct Corpus from vector (periods) of vectors (documents) of
        // strings (words)
        Corpus(
            const std::vector<std::vector<std::vector<std::string>>> structuredCorpus,
            const dec_t delta,
            const dec_t c = 1,
            const dec_t alpha = 0,
            const dec_t beta = 0,
            const dec_t gamma = 0,
            const int theta = 1,
            const dec_t mergeThreshold = 1
        );

        virtual inline int nPeriods() const {
            return this->periods.size(); 
        };
        // see definitions.md or paper
        virtual dec_t energy(const word_t word, const int s) const;
        // ENR (energy-nutrition-ratio), see definitions.md or paper
        dec_t enr(const word_t word, const int s) const;

        // see definitions.md or paper
        virtual std::optional<std::vector<word_t>> findEmergingWords(const int s) const;
        // see definitions.md or paper
        std::optional<std::vector<Topic>> findEmergingTopics(const int s) const;

        //see definitions.md or paper
        dec_t topicHealth(const Topic &topic, int s) const;

        std::optional<const Topic*> findPredecessorTopic(const Topic &topic, const dec_t distance_threshold, int s) const;
        std::vector<std::vector<TopicData>> getTopicEvolution(const dec_t distance_threshold) const;

        // streaming (e.g. printing) operator <<
        friend std::ostream& operator<<(std::ostream& os, Corpus const &corpus) {
            for (const auto period : corpus.periods) os << period << std::endl;
            return os;
        };
};

#endif
