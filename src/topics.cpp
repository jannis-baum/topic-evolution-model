#include "topics.hpp"

dec_t topicDistance(const Topic topic1, const Topic topic2) {
    Topic intersection = {};
    Topic diff12 = {};
    for (const SemanticNode *node: topic1) {
        if (topic2.contains(node)) {
            intersection.insert(node);
        } else {
            diff12.insert(node);
        }
    }

    Topic diff21 = {};
    for (const SemanticNode *node: topic2) {
        if (!topic1.contains(node)) {
            diff21.insert(node);
        }
    }

    return std::min(diff12.size(), diff21.size()) / (dec_t)intersection.size();
}

void mergeTopics(std::vector<Topic> &topics, const std::pair<TopicIt, TopicIt> merge) {
    for (const auto &node: *merge.second) {
        merge.first->insert(node);
    }
    topics.erase(merge.second);
}

void mergeTopicsByThreshold(std::vector<Topic> &topics, const dec_t threshold) {
    typedef std::pair<std::pair<TopicIt, TopicIt>, dec_t> TopicsAndDistance;
    // [((topic1, topic2), distance)]
    // only for topic1 != topic2, distance > threshold and with asymmetrically
    // unique pairs
    std::vector<TopicsAndDistance> distances;
    for (auto topic1 = topics.begin(); topic1 < topics.end(); topic1++) {
        for (auto topic2 = topic1 + 1; topic2 < topics.end(); topic2++) {
            const auto distance = topicDistance(*topic1, *topic2);
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
    std::unordered_set<Topic *> merged;
    for (auto &[topicsToMerge, distance]: distances) {
        if (merged.contains(&(*topicsToMerge.first)) || merged.contains(&(*topicsToMerge.second))) continue;
        mergeTopics(topics, topicsToMerge);
        merged.insert(&(*topicsToMerge.first));
        merged.insert(&(*topicsToMerge.second));
    }

    // recurse for nested merging
    mergeTopicsByThreshold(topics, threshold);
}
