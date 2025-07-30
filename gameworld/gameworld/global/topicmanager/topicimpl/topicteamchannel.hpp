#ifndef __TOPIC_TEAM_CHANNEL_HPP__
#define __TOPIC_TEAM_CHANNEL_HPP__

#include "global/topicmanager/topicinterface.hpp"

class TopicTeamChannel : public ITopic
{
public:
	TopicTeamChannel() {}
	virtual ~TopicTeamChannel() {}

	virtual void OnTopicUpdate(const SubscriberMap& subscriber_map, const char* data, int length);
	virtual void OnTopicSubscribe(Role* role);
};

#endif