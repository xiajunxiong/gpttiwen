#include "randactivitycharismatictanabata.hpp"
#include "global/rank/personrank.hpp"
#include "global/rank/rankmanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gameworld/internalcomm.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"

RandActivityCharismaticTanabata::RandActivityCharismaticTanabata(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_CHARISMATIC_TANABATA)
{
}

RandActivityCharismaticTanabata::~RandActivityCharismaticTanabata()
{
}

void RandActivityCharismaticTanabata::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);
	
	if(ACTIVITY_STATUS_CLOSE == to_status)
	{
		if (CrossConfig::Instance().IsHiddenServer())
		{
			crossgameprotocal::HiddenCrossMeiLiRankRewardReq req;//活动关闭，排行奖励发放
			InternalComm::Instance().SendToCross((const char*)&req, sizeof(req));
		}
		RankManager::Instance().ClearPersonRankType(PERSON_RANK_TYPE_MEILI_QIXI); //活动重开排行榜清空
	}
}
