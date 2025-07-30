#ifndef __TOPIC_MANAGER_HPP__
#define __TOPIC_MANAGER_HPP__

#include "servercommon/servercommon.h"
#include "topicdef.hpp"

/*
* \brief 主题订阅管理系统
* \note 专门用于需要客户端停留在界面中才会收到实时更新的信息的需求
* \note 在客户端打开某界面时请求订阅，此时先下发最新的全部信息，然后在信息更新时主动推送给客户端，客户端关闭界面或下线时取消订阅
*/
class Role;
class ITopic;
class TopicManager
{
public:
	static TopicManager* Instance();
	
	bool OnRoleSubscribe(Role* role, int topic);
	bool OnRoleUnSubscribe(Role* role, int topic);
	void OnRoleLogout(Role* role);
	void OnTopicUpdate(int topic, const char* data, int length);

private:
	bool AddSubscribe(int role_id, const GSNetID& netid, int topic);
	bool RemoveSubscribe(int role_id, int topic);

	ITopic* m_topic_list[TOPIC_TYPE_MAX];
	SubscriberMap m_subscriber_map[TOPIC_TYPE_MAX];

	INSTANCE_FORBID_COPY_AND_EXTERIAL_CREATE(TopicManager);
};

#endif