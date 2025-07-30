#ifndef __TOPIC_CHIEF_PEAK_BATTLE_HPP__
#define __TOPIC_CHIEF_PEAK_BATTLE_HPP__

#include "global/topicmanager/topicinterface.hpp"

class TopicChiefPeakBattle : public ITopic
{
public:
	TopicChiefPeakBattle() {}
	virtual ~TopicChiefPeakBattle() {}

	virtual void OnTopicUpdate(const SubscriberMap& subscriber_map, const char* data, int length);
	virtual void OnTopicSubscribe(Role* role);
};

#endif