#include "activityguildfight.hpp"
#include "world.h"
#include "internalcomm.h"
#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/guildfight/activityguildfightconfig.hpp"
#include "global/guild/guildmanager.hpp"
#include "global/guild/guild.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "protocol/msgactivity.hpp"
#include "obj/character/role.h"
#include "global/guildhonorbattlemanager/guildhonorbattlemanager.hpp"

#include "servercommon/serverconfig/crossconfig.hpp"

ActivityGuildFight::ActivityGuildFight(ActivityManager * activity_manager) : Activity(activity_manager, ACTIVITY_TYPE_GUILD_FIGHT), m_enter_end_timestamp(0), m_is_enter_flag(false)
{
}

ActivityGuildFight::~ActivityGuildFight()
{
}

void ActivityGuildFight::OnActivityStatusChange(int from_status, int to_status)
{
	Activity::OnActivityStatusChange(from_status, to_status);

	const GuildFightOtherCfg & gf_other = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetGuildFightOtherCfg();
	SpecialLogicGuildFight * fight_hall = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicGuildFight(gf_other.scene_id, COMMON_FB_KEY);
	if (NULL == fight_hall) return;

	switch (to_status)
	{
	case ACTIVITY_STATUS_STANDY:
		fight_hall->OnActivityStandy();
		this->ClearLastTitle();
		break;
	case ACTIVITY_STATUS_OPEN:		// 入场时间
		{
			m_is_enter_flag = true;
			m_enter_end_timestamp = gf_other.countdown + static_cast<unsigned int>(EngineAdapter::Instance().Time());
			fight_hall->OnActivityEnterTime();
			this->SendEnterInfo();
		}
		break;
	case ACTIVITY_STATUS_CLOSE:
		fight_hall->OnActivityEnd();
		WorldStatus::Instance().SetFindEndTimestamp(DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_GUILD_FIGHT);
		break;
	}
}

void ActivityGuildFight::OnUserLogin(Role * role)
{
	if (NULL == role) return;

	Activity::OnUserLogin(role);

	const GuildFightOtherCfg & gf_other = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetGuildFightOtherCfg();
	SpecialLogicGuildFight * fight_hall = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicGuildFight(gf_other.scene_id, COMMON_FB_KEY);
	if (NULL == fight_hall) return;

	fight_hall->OnRoleLogin(role);
	this->SendEnterInfo(role);
}

void ActivityGuildFight::Update(unsigned long interval, time_t now_second)
{
	Activity::Update(interval, now_second);

	if (this->IsActivityOpen())
	{
		if (m_enter_end_timestamp != 0 && m_enter_end_timestamp <= now_second)
		{
			m_is_enter_flag = false;
			m_enter_end_timestamp = 0;
			this->SendEnterInfo();
			const GuildFightOtherCfg & gf_other = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetGuildFightOtherCfg();
			SpecialLogicGuildFight * fight_hall = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicGuildFight(gf_other.scene_id, COMMON_FB_KEY);
			if (NULL == fight_hall) return;

			fight_hall->OnActivityOpen();
		}
	}
}

void ActivityGuildFight::OnUserLoginOriginGame(int role_id)
{
	Protocol::SCActivityEnterInfo info;
	info.activity_type = ACTIVITY_TYPE_GUILD_FIGHT;
	info.is_enter_flag = m_is_enter_flag ? 1 : 0;
	info.enter_end_timestamp = m_enter_end_timestamp;
	
	InternalComm::Instance().SendToGameUserThroughCross(role_id, (const char *)&info, sizeof(info));
}

void ActivityGuildFight::GmEnterEndTimeFastforward()
{
	m_enter_end_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
}

void ActivityGuildFight::ClearLastTitle()
{
	const GuildManager::GuildMap& guild_manager = GuildManager::Instance().GetGuildMap();
	for (GuildManager::GuildMapConstIt iter = guild_manager.begin(), end = guild_manager.end(); iter != end; ++iter)
	{
		Guild *guild = iter->second;
		if (NULL != guild)
		{
			guild->SetGuildFightTitle(0);
		}
	}
}

void ActivityGuildFight::SendEnterInfo(Role * role)
{
	if(!CrossConfig::Instance().IsHiddenServer()) return;

	Protocol::SCActivityEnterInfo info;
	info.activity_type = ACTIVITY_TYPE_GUILD_FIGHT;
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
