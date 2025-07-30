#include "topicmarket.hpp"
#include "global/marketmanager/marketmanager.hpp"

void TopicMarket::OnTopicUpdate(const SubscriberMap & subscriber_map, const char * data, int length)
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

void TopicMarket::OnTopicSubscribe(Role * role)
{
	MarketManager::Instance().SendAllInfo(role);
}
