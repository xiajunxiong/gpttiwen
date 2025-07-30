#include "topicteamchannel.hpp"
#include "obj/character/role.h"
#include "global/team/teammanager.hpp"
#include "engineadapter.h"

void TopicTeamChannel::OnTopicUpdate(const SubscriberMap& subscriber_map, const char* data, int length)
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

void TopicTeamChannel::OnTopicSubscribe(Role* role)
{
	TeamManager::Instance().GetTeamChannel()->SendTeamList(role);
}

