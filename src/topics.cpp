#include <algorithm>
#include <cmath>
#include <set>

#include "topics.hpp"

std::ostream& operator<<(std::ostream& os, Topic const &topic) {
    bool first = true;
    for (const auto &node: topic) {
        if (!first) os << ", ";
        os << node->word;
        first = false;
    }
    return os;
};

std::unordered_set<word_t> topicWords(const Topic &topic) {
    std::unordered_set<word_t> words;
    for (const auto &node: topic) {
        words.insert(node->word);
    }
    return words;
}

dec_t topicAvgDistance(const Topic topic1, const Topic topic2, dec_t **distances) {
    dec_t distance = 0;
    int count = 0;
    for (const auto &node1: topic1) {
        for (const auto &node2: topic2) {
            const auto d = distances[node1->word][node2->word];
            distance += (d * d);
            count++;
        }
    }
    return std::sqrt(distance / (dec_t)count);
}

void mergeTopics(const std::pair<TopicIt, TopicIt> merge) {
    for (const auto &node: *merge.second) {
        merge.first->insert(node);
    }
}

void mergeTopicsByThreshold(std::vector<Topic> &topics, const dec_t threshold, dec_t **word_distances) {
    typedef std::pair<std::pair<TopicIt, TopicIt>, dec_t> TopicsAndDistance;
    // [((topic1, topic2), distance)]
    // only for topic1 != topic2, distance > threshold and with asymmetrically
    // unique pairs
    std::vector<TopicsAndDistance> distances;
    for (auto topic1 = topics.begin(); topic1 < topics.end(); topic1++) {
        for (auto topic2 = topic1 + 1; topic2 < topics.end(); topic2++) {
            const auto distance = topicAvgDistance(*topic1, *topic2, word_distances);
            if (distance > threshold) continue;
            distances.push_back({ { topic1, topic2 }, distance });
        }
    }

    // everything already merged
    if (distances.empty()) return;
    
    // sort to merge every topic with best match
    std::sort(distances.begin(), distances.end(), [](const TopicsAndDistance &a, const TopicsAndDistance & b) {
        return a.second < b.second;
    });

    // merge topics & remove from topics vector
    std::unordered_set<int> merged;
    // set orders smallest to largest
    std::set<int> remove;
    for (auto &[topics_to_merge, _]: distances) {
        int offset1 = topics_to_merge.first - topics.begin();
        int offset2 = topics_to_merge.second - topics.begin();
        if (
            merged.contains(offset1) || merged.contains(offset2) ||
            remove.contains(offset1) || remove.contains(offset2)
        ) continue;
        mergeTopics(topics_to_merge);
        merged.insert(offset1);
        remove.insert(offset2);
    }

    // crbegin/crend are for backwards traversal
    for (auto it = remove.crbegin(); it != remove.crend(); it++) {
        topics.erase(topics.begin() + *it);
    }

    // recurse for nested merging
    mergeTopicsByThreshold(topics, threshold, word_distances);
}
