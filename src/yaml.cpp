#include "yaml.hpp"
#include "SemanticGraph.hpp"

std::string dumpTopicEvolution(
    const std::vector<std::vector<TopicData>> &data,
    const std::unordered_map<word_t, std::string> &wtostr
) {
    return dumpIterable(data, [&wtostr](const std::vector<TopicData> &periodData) {
        return dumpIterable(periodData, [&wtostr](const TopicData &topicData) {
            std::string dump = "topic:\n";
            dump += dumpIterable(topicWords(std::get<0>(topicData)), [&wtostr](const word_t &word) {
                return wtostr.at(word);
            });
            dump += "id: " + std::to_string(std::get<1>(topicData)) + "\n";
            dump += "health: " + std::to_string(std::get<2>(topicData)) + "\n";
            return dump;
        });
    });
};
