#include <algorithm>
#include <cmath>
#include <iterator>
#include <unordered_map>
#include <unordered_set>

#include "metrics.hpp"

const std::vector<dec_t> getMetrics(const TopicEvolution evolution) {
    std::unordered_map<int, int> topic_count_by_id;

    int topic_count = 0;
    int word_count = 0;
    dec_t total_health = 0;

    std::unordered_set<int> prev_topic_ids = {};
    int connected_periods = 0;
    int current_pathl = 1;
    int longest_pathl = 1;

    for (const auto &period: evolution) {
        bool is_connected = false;
        std::unordered_set<int> topic_ids = {};

        for (const auto &topic: period) {
            topic_count += 1;
            word_count += std::get<0>(topic).size();
            total_health += std::get<2>(topic);

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
        if (is_connected) {
            connected_periods += 1;
            current_pathl += 1;
            longest_pathl = current_pathl > longest_pathl ? current_pathl : longest_pathl;
        } else {
            current_pathl = 1;
        }
    }

    // number of occurrences of most common topic
    const auto most_topics_it = std::max_element(
        std::begin(topic_count_by_id), std::end(topic_count_by_id),
        [](const auto &a, const auto &b) {
            return a.second < b.second;
        }
    );
    const auto most_topics = most_topics_it == std::end(topic_count_by_id)
        ? (dec_t)NAN
        : most_topics_it->second;
    
    // number of metrics has to match magic number defined in script/process.py!
    std::vector<dec_t> metrics = {
        // METRIC 1: abs(1 - n_ids/n_nodes)
        // i.e. how uncommon are "undiverse" are the topics?
        abs(1 - (dec_t)topic_count_by_id.size() / (dec_t)topic_count),
        // METRIC 2: largest group / n_nodes
        // i.e. how big is the most common topic relative to all topics?
        (dec_t)most_topics / (dec_t)topic_count,
        // METRIC 3: n_{periods with incoming} / (n_periods - 1)
        // i.e. how (relatively) many periods have a connection to their predecessor?
        (dec_t)connected_periods / (dec_t)(evolution.size() - 1),
        // METRIC 4: n_{longest connected periods} / (n_periods)
        // i.e. how (relatively) long is the longest chain of connected periods?
        (dec_t)longest_pathl / (dec_t)evolution.size(),
        // METRIC 5: n_topics / n_periods
        (dec_t)topic_count / (dec_t)evolution.size(),
        // METRIC 6: n_words / n_topics
        (dec_t)word_count / (dec_t)topic_count,
        // METRIC 7: avg topic health
        total_health / (dec_t)topic_count
    };
    for (auto &metric : metrics) if (std::isnan(metric)) metric = 0;
    return metrics;
}
