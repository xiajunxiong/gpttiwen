#ifndef __TOPIC_WORLD_BOSS_rank_HPP__
#define __TOPIC_WORLD_BOSS_rank_HPP__

#include "global/topicmanager/topicinterface.hpp"

class TopicWorldBossRank : public ITopic
{
public:
	TopicWorldBossRank() {}
	virtual ~TopicWorldBossRank() {}

	virtual void OnTopicUpdate(const SubscriberMap& subscriber_map, const char* data, int length);
	virtual void OnTopicSubscribe(Role* role);
};

#endif