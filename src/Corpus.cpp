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
        this->topicbyperiod.push_back(findEmergingTopics(s++));
    }
}

dec_t Corpus::energy(const word_t word, const int s) const {
    dec_t energy = 0;
    for (int i = 1; i <= s; i++) {
        // see definitions.md or paper
        energy += (dec_t)1 / i * (
            std::pow(this->periods[s].nutrition(word, this->c), 2) -
            std::pow(this->periods[s - i].nutrition(word, this->c), 2)
        );
    }
    return energy;
}

dec_t Corpus::enr(const word_t word, const int s) const {
    // see definitions.md or paper
    return this->energy(word, s) / this->periods[s].nutrition(word, this->c);
}

std::vector<word_t> Corpus::findEmergingWords(const int s) const {
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

const std::unordered_map<word_t, SemanticNode> &Corpus::wtonodeByPeriod(const int s) const {
    return this->periods[s].wtonode;
}

std::vector<Topic> Corpus::findEmergingTopics(const int s) const {
    auto emergingWords = this->findEmergingWords(s);
    std::vector<Topic> topics = {};
    const auto &wtonode = this->wtonodeByPeriod(s);

    // find emergin topics
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

bool Corpus::isPersistent(Topic topic, int s) const {
    dec_t overall_mean_energy = 0, topic_mean_energy = 0;
    const std::unordered_map<word_t, SemanticNode> period_words = this->periods[s].wtonode;
    
    if (!period_words.size() || !topic.size())
        return 0;

    for (auto it = period_words.begin(); it != period_words.end(); it++) {
        overall_mean_energy += energy(it->first, s);
    }
    overall_mean_energy /= period_words.size();
    
    for (auto it = topic.begin(); it != topic.end(); it++) {
        topic_mean_energy += energy((*it)->word, s);
    }
    topic_mean_energy /= topic.size();

    return topic_mean_energy >= overall_mean_energy;
}
// s is the index of the period preceeding the period that topic comes from
Topic Corpus::findPredecessorTopic(Topic topic, const dec_t distance_threshold, int s) const {
    std::vector<Topic> previous_topics = this->topicbyperiod[s];
    Topic predecessor = previous_topics[0];

    for(auto it = previous_topics.begin(); it!=previous_topics.end(); it++) {
        if (topicDistance(topic, *it) < topicDistance(topic, predecessor))
            predecessor = *it;
    }

    if (topicDistance(topic, predecessor) <= distance_threshold) {
        return predecessor;
    }
    return {};
}
