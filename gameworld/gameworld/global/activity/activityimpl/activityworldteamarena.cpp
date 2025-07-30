#include "activityworldteamarena.hpp"
#include "world.h"
#include "internalcomm.h"

#include "obj/character/role.h"
#include "protocol/msgactivity.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/struct/global/worldteamarenaparam.hpp"

#include "global/activity/activitymanager.hpp"
#include "global/worldteamarenamanager/worldteamarenarankmanager.hpp"
#include "global/worldteamarenamanager/worldteamarenasignupmanager.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/worldteamarena/activityworldteamarenaconfig.hpp"


ActivityWorldTeamArena::ActivityWorldTeamArena(ActivityManager * activity_manager)
	:Activity(activity_manager, ACTIVITY_TYPE_WORLD_TEAM_ARENA), m_next_switch_season_timestamp(0), m_enter_end_timestamp(0), m_is_enter_flag(false)
{
	m_param.Reset();
}

ActivityWorldTeamArena::~ActivityWorldTeamArena()
{
}

bool ActivityWorldTeamArena::IsNeedSave()
{
	return CrossConfig::Instance().IsHiddenServer();  //这个是跨服活动，隐藏服才保存数据
}

void ActivityWorldTeamArena::Init(const ActivityData & arg_data)
{
	const ActivityWorldTeamArenaParam & arena_param = arg_data.world_team_arena;
	m_param = arena_param;
}

void ActivityWorldTeamArena::GetInitParam(ActivityData & arg_data)
{
	ActivityWorldTeamArenaParam & arena_param = arg_data.world_team_arena;
	arena_param = m_param;
}

void ActivityWorldTeamArena::OnActivityStatusChange(int from_status, int to_status)
{
	Activity::OnActivityStatusChange(from_status, to_status);

	const ActivityWorldTeamArenaOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityWorldTeamArenaConfig()->GetOtherCfg();
	SpecialLogicWorldTeamArena * fight_hall = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicWorldTeamArena(other_cfg.scene_id, COMMON_FB_KEY);
	if (NULL == fight_hall) return;

	switch (to_status)
	{
		case ACTIVITY_STATUS_STANDY:
		fight_hall->OnActivityStandy();
		WorldTeamArenaRankManager::Instance().OnActivityStandy();
		break;
		case ACTIVITY_STATUS_OPEN:		// 入场时间
		{
			m_is_enter_flag = true;
			m_enter_end_timestamp = other_cfg.countdown + static_cast<unsigned int>(EngineAdapter::Instance().Time());
			fight_hall->OnActivityEnterTime();
			this->SendEnterInfo();
			m_next_switch_season_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().NextDayInterval(0, 0, 0) + EngineAdapter::Instance().Time());
			m_param.activity_open_next_zero_timestamp = m_next_switch_season_timestamp;
		}
		break;
		case ACTIVITY_STATUS_CLOSE:
		{
			m_is_enter_flag = false;
			m_enter_end_timestamp = 0;
			this->SendEnterInfo();
			fight_hall->OnActivityEnd();
		}
		break;
	}
}

void ActivityWorldTeamArena::OnUserLogin(Role * role)
{
	if (NULL == role) return;

	Activity::OnUserLogin(role);
	this->SendActivityInfo(role);
	this->SendEnterInfo(role);
}

void ActivityWorldTeamArena::Update(unsigned long interval, time_t now_second)
{
	Activity::Update(interval, now_second);

	if (this->IsActivityOpen())
	{
		if (m_enter_end_timestamp != 0 && m_enter_end_timestamp <= now_second)
		{
			m_is_enter_flag = false;
			m_enter_end_timestamp = 0;
			this->SendEnterInfo();
			const ActivityWorldTeamArenaOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityWorldTeamArenaConfig()->GetOtherCfg();
			SpecialLogicWorldTeamArena * fight_hall = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicWorldTeamArena(other_cfg.scene_id, COMMON_FB_KEY);
			if (NULL == fight_hall) return;

			fight_hall->OnActivityOpen();
		}
	}
	if (0 != m_next_switch_season_timestamp && now_second >= m_next_switch_season_timestamp)
	{
		m_param.season_num += 1;
		m_next_switch_season_timestamp = 0;
		this->SendActivityInfo();
		this->OnHiddenSynActivityInfoToGame();
		this->SetDirty();
	}
}

void ActivityWorldTeamArena::OnCalcNextSwtichTimeByClose()
{
	//计算下一次活动准备开始时间
	time_t cur_month_zero_time = GetMonthZeroTime(EngineAdapter::Instance().Time());
	time_t cur_month_week_zero_time = GetWeekZeroTime(cur_month_zero_time);	//获取该月第一天周一0点

	UNSTD_STATIC_CHECK(ACTIVITY_WORLD_TEAM_ARENA_WEEK_NUM > 0);
	UNSTD_STATIC_CHECK(ACTIVITY_WORLD_TEAM_ARENA_WEEK_DAY_NUM > 0);
	//计算该月活动准备时间戳
	time_t cur_month_activity_standy_timestamp = cur_month_week_zero_time + (ACTIVITY_WORLD_TEAM_ARENA_WEEK_NUM - 1) * SECOND_PER_WEEK + 
		(ACTIVITY_WORLD_TEAM_ARENA_WEEK_DAY_NUM - 1) * SECOND_PER_DAY + ACTIVITY_WORLD_TEAM_ARENA_STANDY_HOUR * SECOND_PER_HOUR + 
		ACTIVITY_WORLD_TEAM_ARENA_STANDY_MIN * SECOND_PER_MIN;

	time_t now_second = EngineAdapter::Instance().Time();
	if (cur_month_activity_standy_timestamp >= now_second)	//还未过当月开启准备时间
	{
		m_next_status_switch_time = (unsigned int)cur_month_activity_standy_timestamp;
	}
	else   //已过当月开启准备时间,则计算下个月的
	{
		time_t next_month_zero_time = now_second + EngineAdapter::Instance().NextMouthInterval(1, 0, 0, 0);
		time_t next_month_week_zero_time = GetWeekZeroTime(next_month_zero_time);	//获取下个月第一天周一0点

		//活动准备时间是月的第一周的话,那么当前时间也有可能会超过这个时间,则还需要计算下下个月
		time_t next_month_activity_standy_timestamp = next_month_week_zero_time + (ACTIVITY_WORLD_TEAM_ARENA_WEEK_NUM - 1) * SECOND_PER_WEEK +
			(ACTIVITY_WORLD_TEAM_ARENA_WEEK_DAY_NUM - 1) * SECOND_PER_DAY + ACTIVITY_WORLD_TEAM_ARENA_STANDY_HOUR * SECOND_PER_HOUR +
			ACTIVITY_WORLD_TEAM_ARENA_STANDY_MIN * SECOND_PER_MIN;
		if (next_month_activity_standy_timestamp >= now_second)
		{
			m_next_status_switch_time = (unsigned int)next_month_activity_standy_timestamp;
		}
		else
		{
			//获取下下个月第一天周一0点
			time_t end_month_week_zero_time = (unsigned int)GetWeekZeroTime(GetMonthZeroTime(next_month_zero_time + 31 * SECOND_PER_DAY));	
			m_next_status_switch_time = end_month_week_zero_time + (ACTIVITY_WORLD_TEAM_ARENA_WEEK_NUM - 1) * SECOND_PER_WEEK +
				(ACTIVITY_WORLD_TEAM_ARENA_WEEK_DAY_NUM - 1) * SECOND_PER_DAY + ACTIVITY_WORLD_TEAM_ARENA_STANDY_HOUR * SECOND_PER_HOUR +
				ACTIVITY_WORLD_TEAM_ARENA_STANDY_MIN * SECOND_PER_MIN;
		}
	}
	UNSTD_STATIC_CHECK(ACTIVITY_WORLD_TEAM_ARENA_SIGN_UP_WEEK_DAY_NUM == 1);

	m_param.start_sign_up_timestamp = (unsigned int)GetWeekZeroTime(m_next_status_switch_time);
	m_param.end_sign_up_timestamp = m_next_status_switch_time + ACTIVITY_WORLD_TEAM_ARENA_STANDY_DUR_S;
	bool is_find = false;
	GLOBALCONFIG->GetDynamicConfig().GetActivityNextCfg(m_activity_type, &m_next_open_wday, &m_statustime_list_cfg, &is_find);
	this->SendActivityInfo();
	this->OnHiddenSynActivityInfoToGame();
	this->SetDirty();
}

void ActivityWorldTeamArena::SetActivityState(const SyncActivityStatus & as)
{
	Activity::SetActivityState(as);

	if (CrossConfig::Instance().IsHiddenServer()) return;
}

void ActivityWorldTeamArena::OnUserLoginOriginGame(int role_id)
{
	Protocol::SCActivityEnterInfo info;
	info.activity_type = ACTIVITY_TYPE_WORLD_TEAM_ARENA;
	info.is_enter_flag = m_is_enter_flag ? 1 : 0;
	info.enter_end_timestamp = m_enter_end_timestamp;

	InternalComm::Instance().SendToGameUserThroughCross(role_id, (const char *)&info, sizeof(info));
}

void ActivityWorldTeamArena::OnConnectCrossServerSucc()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->OnHiddenSynActivityInfoToGame();
	}
	else
	{
		crossgameprotocal::GameHiddenWorldTeamArenaActivityInfoSynReq ghwtaaisr;
		ghwtaaisr.server_id = LocalConfig::Instance().GetMergeServerId();

		InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghwtaaisr, sizeof(ghwtaaisr));
	}
}

void ActivityWorldTeamArena::OnSetActivityInfo(ActivityWorldTeamArenaParam data)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	m_param = data;
	this->SendActivityInfo();
}

void ActivityWorldTeamArena::GmEnterEndTimeFastforward()
{
	m_enter_end_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
}

void ActivityWorldTeamArena::GmSetSignUpTime(int day)
{
	if (day < 0)
	{
		day = 0;
	}

	m_param.start_sign_up_timestamp = (unsigned int)GetZeroTime(EngineAdapter::Instance().Time());
	m_param.end_sign_up_timestamp = (unsigned int)(EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextDayInterval(0, 0, 0) + day * SECOND_PER_DAY);
	this->SendActivityInfo();
	this->OnHiddenSynActivityInfoToGame();
	this->SetDirty();
}

void ActivityWorldTeamArena::CmdToRankToSignUp(int season, unsigned int sign_up_end_timestamp)
{
	if (season <= 0)
	{
		season = 1;
	}
	m_param.season_num = season;
	m_next_switch_season_timestamp = 0;
	m_param.start_sign_up_timestamp = (unsigned int)GetZeroTime(EngineAdapter::Instance().Time());
	m_param.end_sign_up_timestamp = sign_up_end_timestamp;
	this->SendActivityInfo();
	this->OnHiddenSynActivityInfoToGame();
	this->SetDirty();
}

void ActivityWorldTeamArena::CmdToAdjTime(unsigned int sign_up_start_timestamp, unsigned int sign_up_end_timestamp, unsigned int next_status_switch_timestamp)
{
	if (sign_up_start_timestamp > sign_up_end_timestamp)
	{
		std::swap(sign_up_start_timestamp, sign_up_end_timestamp);
	}

	m_param.start_sign_up_timestamp = sign_up_start_timestamp;
	m_param.end_sign_up_timestamp = sign_up_end_timestamp;
	m_next_status_switch_time = next_status_switch_timestamp;
	this->SendActivityInfo();
	this->OnHiddenSynActivityInfoToGame();
	this->SetDirty();
}

bool ActivityWorldTeamArena::IsCanSignUp()
{
	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	if (m_param.start_sign_up_timestamp <= now_second && now_second <= m_param.end_sign_up_timestamp)
	{
		return true;
	}

	return false;
}

void ActivityWorldTeamArena::SendActivityInfo(Role * role)
{
	Protocol::SCActivityWorldTeamArenaInfo info;
	info.season_num = m_param.season_num;
	info.start_sign_up_timestamp = m_param.start_sign_up_timestamp;
	info.end_sign_up_timestamp = m_param.end_sign_up_timestamp;
	info.activity_open_next_zero_timestamp = m_param.activity_open_next_zero_timestamp;

	if (NULL == role)
	{
		World::GetInstWorld()->SendToAllGateway((const char *)&info, sizeof(info));
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
	}
}

void ActivityWorldTeamArena::SendEnterInfo(Role * role)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	Protocol::SCActivityEnterInfo info;
	info.activity_type = ACTIVITY_TYPE_WORLD_TEAM_ARENA;
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

void ActivityWorldTeamArena::SendActivtyNotify(int notify_status)
{
	int length = 0;
	if (ACTIVITY_STATUS_STANDY == notify_status)
	{
		length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_world_team_arena_team_standy_brocast);
	}
	else if (ACTIVITY_STATUS_OPEN == notify_status)
	{
		length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_world_team_arena_team_can_enter_brocast);
	}
	else
	{
		length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_activity_state, m_activity_type, notify_status);
	}
	if (length <= 0) return;

	Protocol::SCSystemMsg sm;
	int sendlen = 0;
	SysMsgType msg_type = GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT);	// 跑马灯和系统栏都要显示
	if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, msg_type, 0, 0, this->GetActivityLimitLevel(m_activity_type)))
	{
		World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);		// 通知所有玩家
	}

	if (ActivityManager::IsCrossActivity(m_activity_type) && CrossConfig::Instance().IsHiddenServer())
	{
		InternalComm::Instance().SendToAllGameUserThroughCross((const char *)&sm, sendlen);
	}
}

void ActivityWorldTeamArena::OnHiddenSynActivityInfoToGame(int server_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::HiddenGameWorldTeamArenaSynActivityInfo hgwtasai;
	hgwtasai.activity_info = m_param;

	if (0 == server_id)
	{
		InternalComm::Instance().SendToAllGameThroughCross((const char *)&hgwtasai, sizeof(hgwtasai));
	}
	else
	{
		InternalComm::Instance().SendToGameThroughCross2((const char *)&hgwtasai, sizeof(hgwtasai), server_id);
	}
}
