#ifndef __TOPIC_WORLD_BOSS_3_RANK_HPP__
#define __TOPIC_WORLD_BOSS_3_RANK_HPP__

#include "global/topicmanager/topicinterface.hpp"

class TopicWorldBoss3Rank : public ITopic
{
public:
	TopicWorldBoss3Rank() {}
	virtual ~TopicWorldBoss3Rank() {}

	virtual void OnTopicUpdate(const SubscriberMap& subscriber_map, const char* data, int length);
	virtual void OnTopicSubscribe(Role* role);
};

#endif