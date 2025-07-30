#include "topicworldboss3rank.hpp"

#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activityworldboss3.hpp"

void TopicWorldBoss3Rank::OnTopicUpdate(const SubscriberMap & subscriber_map, const char * data, int length)
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

void TopicWorldBoss3Rank::OnTopicSubscribe(Role * role)
{
	ActivityWorldBoss3 * activity = (ActivityWorldBoss3 *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS_3);
	if (activity != NULL)
	{
		activity->SendRankInfo(role);
	}
}
