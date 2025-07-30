#include "topicrapaimaihang.hpp"

#include "obj/character/role.h"

#include "global/randactivity/randactivity.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityimpl/randactivitypaimaihang.hpp"

void TopicRAPaiMaiHang::OnTopicUpdate(const SubscriberMap & subscriber_map, const char * data, int length)
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

void TopicRAPaiMaiHang::OnTopicSubscribe(Role * role)
{
	if(NULL == role) return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL == ramgr) return;
	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_PAI_MAI_HANG))
	{
		return;
	}
	RandActivityPaiMaiHang * activity = (RandActivityPaiMaiHang * )ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_PAI_MAI_HANG);
	if (NULL != activity)
	{
		activity->SendNoticeInfo(role);
	}
}
