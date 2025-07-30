#ifndef __TOPIC_MANAGER_HPP__
#define __TOPIC_MANAGER_HPP__

#include "servercommon/servercommon.h"
#include "topicdef.hpp"

/*
* \brief ���ⶩ�Ĺ���ϵͳ
* \note ר��������Ҫ�ͻ���ͣ���ڽ����вŻ��յ�ʵʱ���µ���Ϣ������
* \note �ڿͻ��˴�ĳ����ʱ�����ģ���ʱ���·����µ�ȫ����Ϣ��Ȼ������Ϣ����ʱ�������͸��ͻ��ˣ��ͻ��˹رս��������ʱȡ������
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