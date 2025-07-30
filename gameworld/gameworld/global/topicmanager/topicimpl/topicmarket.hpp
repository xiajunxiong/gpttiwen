#ifndef __TOPIC_MARKET_HPP__
#define __TOPIC_MARKET_HPP__

#include "global/topicmanager/topicinterface.hpp"

class TopicMarket : public ITopic
{
public:
	TopicMarket() {}
	virtual ~TopicMarket() {}

	virtual void OnTopicUpdate(const SubscriberMap& subscriber_map, const char* data, int length);
	virtual void OnTopicSubscribe(Role* role);
};


#endif