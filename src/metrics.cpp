#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <unordered_set>

#include "metrics.hpp"

const std::vector<dec_t> getMetrics(const TopicEvolution evolution) {
    std::unordered_map<int, int> topic_count_by_id;
    int topic_count = 0;

    std::unordered_set<int> prev_topic_ids = {};
    int connected_periods = 0;

    for (const auto &period: evolution) {
        bool is_connected = false;
        std::unordered_set<int> topic_ids = {};

        for (const auto &topic: period) {
            topic_count += 1;
            const auto id = std::get<1>(topic);
            topic_ids.insert(id);
            if (topic_count_by_id.contains(id)) {
                topic_count_by_id[id] += 1;
            } else {
                topic_count_by_id[id] = 1;
            };
            if (prev_topic_ids.contains(id)) is_connected = true;
        }

        prev_topic_ids = topic_ids;
        if (is_connected) connected_periods += 1;
    }

    // number of occurrences of most common topic
    const auto most_topics = std::max_element(
        std::begin(topic_count_by_id), std::end(topic_count_by_id),
        [](const auto &a, const auto &b) {
            return a.second < b.second;
        }
    )->second;
    
    return {
        // METRIC 1: abs(1 - n_ids/n_nodes)
        // i.e. how uncommon are "undiverse" are the topics?
        abs(1 - (dec_t)topic_count_by_id.size() / (dec_t)topic_count),
        // METRIC 2: largest group / n_nodes
        // i.e. how big is the most common topic relative to all topics?
        (dec_t)most_topics / (dec_t)topic_count,
        // METRIC 3: n_{periods with incoming} / (n_periods - 1)
        // i.e. how (relatively) many periods have a connection to their predecessor?
        (dec_t)connected_periods / (dec_t)evolution.size()
    };
}
