#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activitychiefpeakbattle.hpp"

#include "topicchiefpeakbattle.hpp"

void TopicChiefPeakBattle::OnTopicUpdate(const SubscriberMap& subscriber_map, const char* data, int length)
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

void TopicChiefPeakBattle::OnTopicSubscribe(Role * role)
{
	ActivityChiefPeakBattle * activity = (ActivityChiefPeakBattle *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_CHIEF_PEAK_BATTLE);
	if (activity != NULL)
	{
		activity->SendMatchInfo(role);
	}
}
