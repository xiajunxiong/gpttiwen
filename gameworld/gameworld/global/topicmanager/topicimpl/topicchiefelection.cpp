#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activitychiefelection.hpp"

#include "topicchiefelection.hpp"

void TopicChiefElection::OnTopicUpdate(const SubscriberMap& subscriber_map, const char* data, int length)
{
	static GSNetID gsnetid[256];
	int rolenum = 0;

	SubscriberMap::const_iterator it = subscriber_map.begin();
	for (; it != subscriber_map.end(); ++it)
	{
		gsnetid[rolenum++] = it->second.netid;
		if (rolenum >= 256)
		{
			EngineAdapter::Instance().NetMulSend(gsnetid, rolenum, data, length);
			rolenum = 0;
		}
	}

	if (rolenum > 0)
	{
		EngineAdapter::Instance().NetMulSend(gsnetid, rolenum, data, length);
	}
}

void TopicChiefElection::OnTopicSubscribe(Role * role)
{
	ActivityChiefElection * activity = (ActivityChiefElection *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_CHIEF_ELECTION);
	if (activity != NULL)
	{
		activity->SendMatchInfo(role);
	}
}
