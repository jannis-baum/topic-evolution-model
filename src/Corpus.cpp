#include "Corpus.hpp"
#include "helpers.hpp"
#include "topics.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>
#include <unordered_set>

Corpus::Corpus(
    const std::vector<std::vector<std::vector<std::string>>> structured_corpus,
    const dec_t delta,
    const dec_t c,
    const dec_t alpha,
    const dec_t beta,
    const dec_t gamma,
    const int theta,
    const dec_t merge_threshold,
    const dec_t evolution_threshold)
: periods({})
, wtostr({})
, c(c), alpha(alpha), beta(beta), gamma(gamma), theta(theta)
, merge_threshold(merge_threshold), evolution_threshold(evolution_threshold) {
    // string to word_t (aka int) mapping
    std::unordered_map<std::string, word_t> strtow = {};
    
    // construct periods
    int s = 0;
    for (const auto period : structured_corpus) {
        std::vector<Document> documents = {};
        // construct documents
        for (const auto document : period) {
            std::vector<word_t> words = {};
            // index words & save mapping in both directions
            for (const auto word : document) {
                if (!strtow.contains(word)) {
                    int new_index = strtow.size();
                    strtow[word] = new_index;
                    wtostr[new_index] = word;
                }
                words.push_back(strtow[word]);
            }
            documents.push_back(Document(words, this->wtostr));
        }
        this->periods.push_back(CorpusPeriod(documents, this->wtostr, delta));
        // we are sure emerging topics exist since we just added the period
        this->topics_by_period.push_back(findEmergingTopics(s++).value());
    }
}

dec_t Corpus::energy(const word_t word, const int s) const {
    if (!this->periodExists(s)) return 0;
    if (s == 0) {
        return std::pow(this->periods[s].nutrition(word, this->c), 2);
    }
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

    auto emerging_words_opt = this->findEmergingWords(s);
    if (!emerging_words_opt) {
        return std::nullopt;
    }
    std::vector<word_t> emerging_words = *emerging_words_opt;
    std::vector<Topic> topics = {};
    const auto &wtonode = this->wtonode_by_period(s);

    // find emerging topics
    for (const auto e: emerging_words) {
        if (!wtonode.contains(e)) continue;
        const SemanticNode *node = wtonode.at(e);
        // start with node itself
        std::unordered_set<const SemanticNode *> topic = { node };

        // discover connected nodes with BSF within theta
        const auto theta = this->theta;
        node->bfs([e, theta, &topic](const SemanticNode *discovered) mutable {
            // add discovered to topic if original node can be discovered with
            // backwards BFS within theta
            discovered->bfs([e, discovered, &topic](const SemanticNode *back_discovered) mutable {
                if (back_discovered->word != e) return true;
                topic.insert(discovered);
                return false;
            }, theta);
            return true;
        }, theta);

        topics.push_back(topic);
    }

    mergeTopicsByThreshold(topics, this->merge_threshold);

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

    const auto &prev_topics = this->topics_by_period[s - 1];
    if (prev_topics.empty()) return std::nullopt;

    const Topic *predecessor = &prev_topics[0];
    for (auto it = prev_topics.begin(); it < prev_topics.end(); it++) {
        if (topicDistance(topic, *it) < topicDistance(topic, *predecessor)) {
            predecessor = &(*it);
        }
    }

    if (topicDistance(topic, *predecessor) <= distance_threshold) {
        return predecessor;
    }
    return std::nullopt;
}

TopicEvolution Corpus::getTopicEvolution() const {
    std::vector<std::vector<TopicData>> evolution;
    std::unordered_map<const Topic *, int> topic_ids;
    int next_id = 0;
    
    for (int s = 0; s < this->nPeriods(); s++) {
        evolution.push_back({});
        if (topics_by_period[s].empty()) continue;

        for (auto it = this->topics_by_period[s].begin(); it != this->topics_by_period[s].end(); it++) {
            auto predecessor_opt = this->findPredecessorTopic(*it, this->evolution_threshold, s);
            if (predecessor_opt) {
                topic_ids[&(*it)] = topic_ids[predecessor_opt.value()];
            } else {
                topic_ids[&(*it)] = next_id++;
            }
            evolution[s].push_back({*it, topic_ids[&(*it)], topicHealth(*it, s)});
        }
    }
    return evolution;
}
