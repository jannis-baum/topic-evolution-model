#ifndef TOPICS_HPP
#define TOPICS_HPP

#include <ostream>
#include <unordered_set>

#include "SemanticGraph.hpp"
#include "types.hpp"

typedef std::unordered_set<const SemanticNode *> Topic;
typedef std::vector<Topic>::iterator TopicIt;

std::unordered_set<word_t> topicWords(const Topic &topic);
dec_t topicAvgDistance(const Topic topic1, const Topic topic2, dec_t **distances);
void mergeTopics(const std::pair<TopicIt, TopicIt> merge);
void mergeTopicsByThreshold(std::vector<Topic> &topics, const dec_t threshold, dec_t **word_distances);

// streaming (e.g. printing) operator <<
std::ostream& operator<<(std::ostream& os, Topic const &topic);

#endif
