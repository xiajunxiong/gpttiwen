#include "activityonlyfight.hpp"
#include "world.h"

#include "obj/character/role.h"
#include "config/logicconfigmanager.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "gameworld/internalcomm.h"
#include "protocol/msgactivity.hpp"

#include "global/datasynccheck/datasynccheck.hpp"

ActivityOnlyFight::ActivityOnlyFight(ActivityManager * activity_manager)
	:Activity(activity_manager, ACTIVITY_TYPE_ONLY_FIGHT), m_enter_end_timestamp(0), m_is_enter_flag(false)
{
}

ActivityOnlyFight::~ActivityOnlyFight()
{
}

void ActivityOnlyFight::OnActivityStatusChange(int from_status, int to_status)
{
	Activity::OnActivityStatusChange(from_status, to_status);

	switch (to_status)
	{
		case ACTIVITY_STATUS_STANDY:			
		{
			WorldStatus::Instance().ClearOnlyFightRankInfo();
			WorldStatus::Instance().OnlyFightStatusChange(to_status);
		}
		break;
		case ACTIVITY_STATUS_OPEN:
		{
			m_is_enter_flag = true;
			const OnlyFightOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetOnlyFightOtherCfg();
			m_enter_end_timestamp = other_cfg.countdown + static_cast<unsigned int>(EngineAdapter::Instance().Time());
			WorldStatus::Instance().OnlyFightStatusChange(to_status, m_is_enter_flag);
			this->SendEnterInfo();
		}
		break;
		case ACTIVITY_STATUS_CLOSE:
		{
			WorldStatus::Instance().OnlyFightStatusChange(to_status);
			WorldStatus::Instance().SetFindEndTimestamp(DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_ONLY_FIGHT);
		}
		break;
	}

}

void ActivityOnlyFight::OnUserLogin(Role * role)
{
	if (NULL == role) return;

	Activity::OnUserLogin(role);
	this->SendEnterInfo(role);
}

void ActivityOnlyFight::Update(unsigned long interval, time_t now_second)
{
	Activity::Update(interval, now_second);

	if (this->IsActivityOpen())
	{
		if (m_enter_end_timestamp != 0 && m_enter_end_timestamp <= now_second)
		{
			m_is_enter_flag = false;
			m_enter_end_timestamp = 0;
			WorldStatus::Instance().OnlyFightStatusChange(ACTIVITY_STATUS_OPEN, m_is_enter_flag);
			this->SendEnterInfo();
		}
		if (!m_is_enter_flag)
		{
			WorldStatus::Instance().CheckOnlyFightSceneRoleNum();
		}
	}
}

void ActivityOnlyFight::OnConnectCrossServerSucc()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		WorldStatus::Instance().HiddenSynOnlyFightDataToAllGame();
	}
	else
	{
		crossgameprotocal::GameHiddenSynOnlyFightInfoReq ghsofir;
		ghsofir.server_id = LocalConfig::Instance().GetMergeServerId();

		InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghsofir, sizeof(ghsofir));
		DataSyncCheck::Instance().AddCheck(crossgameprotocal::DATA_SYNC_CHECK_TYPE_ACTIVITY);
	}
}

void ActivityOnlyFight::SetActivityState(const SyncActivityStatus & as)
{
	Activity::SetActivityState(as);

	if (CrossConfig::Instance().IsHiddenServer()) return;

	if (as.activity_status == ACTIVITY_STATUS_STANDY)
	{
		WorldStatus::Instance().ClearOnlyFightRankInfo();
	}
}

void ActivityOnlyFight::OnUserLoginOriginGame(int role_id)
{
	Protocol::SCActivityEnterInfo info;
	info.activity_type = ACTIVITY_TYPE_ONLY_FIGHT;
	info.is_enter_flag = m_is_enter_flag ? 1 : 0;
	info.enter_end_timestamp = m_enter_end_timestamp;

	InternalComm::Instance().SendToGameUserThroughCross(role_id, (const char *)&info, sizeof(info));
}

void ActivityOnlyFight::GmEnterEndTimeFastforward()
{
	m_enter_end_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
}

void ActivityOnlyFight::SendEnterInfo(Role * role)
{
	if(!CrossConfig::Instance().IsHiddenServer()) return;

	Protocol::SCActivityEnterInfo info;
	info.activity_type = ACTIVITY_TYPE_ONLY_FIGHT;
	info.is_enter_flag = m_is_enter_flag ? 1 : 0;
	info.enter_end_timestamp = m_enter_end_timestamp;

	if (NULL == role)
	{
		World::GetInstWorld()->SendToAllGateway((const char *)&info, sizeof(info));
		InternalComm::Instance().SendToAllGameUserThroughCross((const char *)&info, sizeof(info));
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&info, sizeof(info));
	}
}
