#ifndef TOPICS_HPP
#define TOPICS_HPP

#include <ostream>
#include <unordered_set>

#include "SemanticGraph.hpp"
#include "types.hpp"

typedef std::unordered_set<const SemanticNode *> Topic;
typedef std::vector<Topic>::iterator TopicIt;

std::unordered_set<word_t> topicWords(const Topic &topic);
dec_t topicDistance(const Topic topic1, const Topic topic2);
void mergeTopics(std::vector<Topic> &topics, const std::pair<TopicIt, TopicIt> merge);
void mergeTopicsByThreshold(std::vector<Topic> &topics, const dec_t threshold);

// streaming (e.g. printing) operator <<
std::ostream& operator<<(std::ostream& os, Topic const &topic);

#endif
