#include "randactivitysports.hpp"
#include "gameworld/global/colosseum/colosseummanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"


RandAcitivitySports::RandAcitivitySports(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_SPORTS)
{
}

RandAcitivitySports::~RandAcitivitySports()
{
}

void RandAcitivitySports::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);
	if (ACTIVITY_STATUS_CLOSE == to_status)
	{
		//竞技场排名结算
		if (CrossConfig::Instance().IsHiddenServer())return;
		ColosseumManager::Instance().OnRandActivityEnd();
	}
}
