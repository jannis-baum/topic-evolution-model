#include "Corpus.hpp"
#include "helpers.hpp"
#include "topics.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>
#include <unordered_set>

Corpus::Corpus(
    const std::vector<std::vector<std::vector<std::string>>> structuredCorpus,
    const dec_t delta,
    const dec_t c,
    const dec_t alpha,
    const dec_t beta,
    const dec_t gamma,
    const int theta,
    const dec_t mergeThreshold)
: periods({})
, wtostr({})
, c(c), alpha(alpha), beta(beta), gamma(gamma), theta(theta), mergeThreshold(mergeThreshold) {
    // string to word_t (aka int) mapping
    std::unordered_map<std::string, word_t> strtow = {};
    
    // construct periods
    int s = 0;
    for (const auto period : structuredCorpus) {
        std::vector<Document> documents = {};
        // construct documents
        for (const auto document : period) {
            std::vector<word_t> words = {};
            // index words & save mapping in both directions
            for (const auto word : document) {
                if (!strtow.contains(word)) {
                    int newIndex = strtow.size();
                    strtow[word] = newIndex;
                    wtostr[newIndex] = word;
                }
                words.push_back(strtow[word]);
            }
            documents.push_back(Document(words, this->wtostr));
        }
        this->periods.push_back(CorpusPeriod(documents, this->wtostr, delta));
        // we are sure emerging topics exist since we just added the period
        this->topicsByPeriod.push_back(findEmergingTopics(s++).value());
    }
}

dec_t Corpus::energy(const word_t word, const int s) const {
    if (!this->periodExists(s)) return 0;
    dec_t energy = 0;
    // see definitions.md or paper
    for (int i = 1; i <= s; i++) {
        energy += (dec_t)1 / i * (
            std::pow(this->periods[s].nutrition(word, this->c), 2) -
            std::pow(this->periods[s - i].nutrition(word, this->c), 2)
        );
    }
    return energy;
}

dec_t Corpus::enr(const word_t word, const int s) const {
    if (!this->periodExists(s)) return 0;
    // see definitions.md or paper
    return this->energy(word, s) / this->periods[s].nutrition(word, this->c);
}

std::optional<std::vector<word_t>> Corpus::findEmergingWords(const int s) const {
    if (!this->periodExists(s)) return std::nullopt;

    auto candidates = this->periods[s].findNonFloodWords(this->c, this->alpha);
    // find energy threshold
    std::vector<dec_t> energies = {};
    for (const auto & word : candidates) {
        energies.push_back(this->energy(word, s));
    }
    dec_t threshold = mstdThreshold(energies, this->beta);

    // erase candidates that don't abide energy & enr thresholds
    for (int i = 0; i < energies.size(); i++) {
        if (
            energies[i] <= threshold ||
            (s != 0 && this->enr(candidates[i], s) <= this->enr(candidates[i], s - 1) * this->gamma)
        ) {
            candidates[i] = -1;
        }
    }
    std::erase(candidates, -1);

    return candidates;
}

std::optional<std::vector<Topic>> Corpus::findEmergingTopics(const int s) const {
    if (!this->periodExists(s)) return std::nullopt;

    auto emergingWordsOpt = this->findEmergingWords(s);
    if (!emergingWordsOpt) {
        return std::nullopt;
    }
    std::vector<word_t> emergingWords = *emergingWordsOpt;
    std::vector<Topic> topics = {};
    const auto &wtonode = this->wtonodeByPeriod(s);

    // find emerging topics
    for (const auto e: emergingWords) {
        if (!wtonode.contains(e)) continue;
        const SemanticNode *node = &wtonode.at(e);
        // start with node itself
        std::unordered_set<const SemanticNode *> topic = { node };

        // discover connected nodes with BSF within theta
        const auto theta = this->theta;
        node->bfs([e, theta, &topic](const SemanticNode *discovered) mutable {
            // add discovered to topic if original node can be discovered with
            // backwards BFS within theta
            discovered->bfs([e, discovered, &topic](const SemanticNode *backDiscovered) mutable {
                if (backDiscovered->word != e) return true;
                topic.insert(discovered);
                return false;
            }, theta);
            return true;
        }, theta);

        topics.push_back(topic);
    }

    mergeTopicsByThreshold(topics, this->mergeThreshold);

    return topics;
}

dec_t Corpus::topicHealth(const Topic &topic, int s) const {
    dec_t topic_mean_energy = 0;
    for (auto it = topic.begin(); it != topic.end(); it++) {
        topic_mean_energy += this->energy((*it)->word, s);
    }
    return topic_mean_energy / topic.size();
}

// s is the index of the period that topic is in
std::optional<const Topic *> Corpus::findPredecessorTopic(const Topic &topic, const dec_t distance_threshold, int s) const {
    if (!this->periodExists(s)) return std::nullopt;
    if (!this->periodExists(s - 1)) return std::nullopt;

    const auto &previousTopics = this->topicsByPeriod[s - 1];
    if (previousTopics.empty()) return std::nullopt;

    const Topic *predecessor = &previousTopics[0];
    for (auto it = previousTopics.begin(); it < previousTopics.end(); it++) {
        if (topicDistance(topic, *it) < topicDistance(topic, *predecessor)) {
            predecessor = &(*it);
        }
    }

    if (topicDistance(topic, *predecessor) <= distance_threshold) {
        return predecessor;
    }
    return std::nullopt;
}

std::vector<std::vector<std::tuple<Topic, int, dec_t>>> Corpus::getTopicEvolution(const dec_t distance_threshold) const {
    std::vector<std::vector<std::tuple<Topic, int, dec_t>>> topicIdsByPeriod;
    std::unordered_map<const Topic *, int> topicIds;
    int idcount = 0;
    
    for (int s = 0; s < this->topicsByPeriod.size(); s++) {
        topicIdsByPeriod.push_back({});
        if (topicsByPeriod[s].empty()) continue;

        for (auto topicIt = (this->topicsByPeriod[s]).begin(); topicIt != (this->topicsByPeriod[s]).end(); topicIt++) {
            auto predecessorOpt = this->findPredecessorTopic(*topicIt, distance_threshold, s);
            if (predecessorOpt) {
                topicIds[&(*topicIt)] = topicIds[*predecessorOpt];
            }
            else {
                topicIds[&(*topicIt)] = idcount;
                idcount++;
            }
            topicIdsByPeriod[s].push_back({*topicIt, topicIds[&(*topicIt)], topicHealth(*topicIt, s)});
        }
    }
    return topicIdsByPeriod;
}
