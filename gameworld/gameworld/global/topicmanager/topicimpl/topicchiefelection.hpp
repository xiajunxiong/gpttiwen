#ifndef __TOPIC_CHIEF_ELECTION_HPP__
#define __TOPIC_CHIEF_ELECTION_HPP__

#include "global/topicmanager/topicinterface.hpp"

class TopicChiefElection : public ITopic
{
public:
	TopicChiefElection() {}
	virtual ~TopicChiefElection() {}

	virtual void OnTopicUpdate(const SubscriberMap& subscriber_map, const char* data, int length);
	virtual void OnTopicSubscribe(Role* role);
};

#endif