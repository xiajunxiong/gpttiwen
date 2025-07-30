#ifndef __TOPIC_WORLD_BOSS_2_RANK_HPP__
#define __TOPIC_WORLD_BOSS_2_RANK_HPP__

#include "global/topicmanager/topicinterface.hpp"

class TopicWorldBoss2Rank : public ITopic
{
public:
	TopicWorldBoss2Rank() {}
	virtual ~TopicWorldBoss2Rank() {}

	virtual void OnTopicUpdate(const SubscriberMap& subscriber_map, const char* data, int length);
	virtual void OnTopicSubscribe(Role* role);
};

#endif