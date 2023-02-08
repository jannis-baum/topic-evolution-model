#ifndef TOPICS_HPP
#define TOPICS_HPP

#include <unordered_set>

#include "SemanticGraph.hpp"

typedef std::unordered_set<const SemanticNode *> Topic;
typedef std::vector<Topic>::iterator TopicIt;

dec_t topicDistance(const Topic topic1, const Topic topic2);

void mergeTopics(std::vector<Topic> &topics, const std::pair<TopicIt, TopicIt> merge);
void mergeTopicsByThreshold(std::vector<Topic> &topics, const dec_t threshold);

#endif
