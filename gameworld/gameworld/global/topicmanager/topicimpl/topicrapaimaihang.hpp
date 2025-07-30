#ifndef __TOPIC_RA_PAI_MAI_HANG_HPP__
#define __TOPIC_RA_PAI_MAI_HANG_HPP__

#include "global/topicmanager/topicinterface.hpp"

class TopicRAPaiMaiHang : public ITopic
{
public:
	TopicRAPaiMaiHang() {}
	virtual ~TopicRAPaiMaiHang() {}

	virtual void OnTopicUpdate(const SubscriberMap& subscriber_map, const char* data, int length);
	virtual void OnTopicSubscribe(Role* role);
};


#endif