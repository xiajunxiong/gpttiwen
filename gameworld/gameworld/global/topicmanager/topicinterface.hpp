#ifndef __TOPIC_INTERFACE_HPP__
#define __TOPIC_INTERFACE_HPP__

#include "topicdef.hpp"
#include "engineadapter.h"

class Role;
class ITopic
{
public:
	ITopic() {}
	virtual ~ITopic() {}

	virtual void OnTopicUpdate(const SubscriberMap& subscriber_map, const char* data, int length) = 0;
	virtual void OnTopicSubscribe(Role* role) = 0;
};

#endif