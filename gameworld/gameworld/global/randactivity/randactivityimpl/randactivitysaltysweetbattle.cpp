#include "randactivitysaltysweetbattle.hpp"
#include "config/randactivityconfig/impl/randactivitysaltysweetbattleconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "world.h"
#include "obj/character/role.h"
#include "global/randactivity/randactivitymanager.hpp"
#include "item/knapsack.h"
#include "other/rolemodulemanager.hpp"
#include "config/activityconfig/randactivityopencfg.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "gameworld/internalcomm.h"

RandActivitySaltySweetBattle::RandActivitySaltySweetBattle(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE)
{

}

RandActivitySaltySweetBattle::~RandActivitySaltySweetBattle()
{
}

void RandActivitySaltySweetBattle::Init(const ActivityData & arg_data)
{
	RandActivity::Init(arg_data);

	const RandActivityData * ra_data = &arg_data.rand_activity_data;
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(ra_data->data));
	memcpy(&m_param, ra_data->data, sizeof(m_param));
}

void RandActivitySaltySweetBattle::GetInitParam(ActivityData & arg_data)
{
	RandActivity::GetInitParam(arg_data);

	const RandActivityData * ra_data = &arg_data.rand_activity_data;
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(ra_data->data));
	memcpy((void *)ra_data->data, &m_param, sizeof(m_param));
}

void RandActivitySaltySweetBattle::OnUserLogin(Role * user)
{
	RandActivity::OnUserLogin(user);

	if (this->GetStatus() == ACTIVITY_STATUS_OPEN)
	{
		this->RASendGameInfo(user);
	}
}

void RandActivitySaltySweetBattle::Update(unsigned long interval, time_t now_second)
{
	RandActivity::Update(interval, now_second);
	if (CrossConfig::Instance().IsHiddenServer())return;
	if (m_rand_activity_status != ACTIVITY_STATUS_OPEN) return;
	
	if (m_param.status_mod == SALTY_SWEET_MOD_VOTE && m_param.add_value_tamp > 0 && m_param.add_value_tamp <= now_second)
	{
		const RandActivitySaltySweetBattleConfig * activity = static_cast<const RandActivitySaltySweetBattleConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE));
		if (NULL != activity)
		{
			const SaltySweetOtherCfg & other_cfg = activity->GetOtherCfg();
			int rand_vote = rand() % 2;
			if (rand_vote == 1)
			{
				this->OnRoleVote(SALTY_SWEET_TYPE_SALTY, other_cfg.salty_add_vote);
			}
			else
			{
				this->OnRoleVote(SALTY_SWEET_TYPE_SWEET, other_cfg.sweet_add_vote);
			}
			m_param.add_value_tamp = other_cfg.times + (unsigned int)now_second;
		}
	}

	if (m_param.activity_end_tamp > 0 && m_param.activity_end_tamp <= now_second)
	{
		if (m_param.status_mod == SALTY_SWEET_MOD_VOTE)
		{
			const RandActivitySaltySweetBattleConfig * activity = static_cast<const RandActivitySaltySweetBattleConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE));
			if (NULL != activity)
			{
				const SaltySweetOtherCfg & other_cfg = activity->GetOtherCfg();
				m_param.activity_end_tamp = other_cfg.settlement_times + (unsigned int)now_second;
			}
			
			m_param.status_mod = SALTY_SWEET_MOD_COUNTING;
			this->RASendGameInfo(NULL, true);
		}
		else if (m_param.status_mod == SALTY_SWEET_MOD_COUNTING)
		{
			m_param.status_mod = SALTY_SWEET_MOD_RESULT;
			this->EndGame();
			this->RASendGameInfo(NULL, true);
		}
		this->OnDataChangeSendCross();
		gamelog::g_log_rand_activity.printf(LL_INFO, "%s status_mod[%d] activity_end_tamp[%d]", __FUNCTION__, m_param.status_mod, m_param.activity_end_tamp);
	}
}

void RandActivitySaltySweetBattle::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);
	if (CrossConfig::Instance().IsHiddenServer())return;
	if (to_status == ACTIVITY_STATUS_OPEN)
	{
		//设定活动
		const RandActivitySaltySweetBattleConfig * activity = static_cast<const RandActivitySaltySweetBattleConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE));
		if (NULL != activity)
		{
			tm * activity_loacl_time = NULL;
			time_t activity_end_tamp;
			activity_end_tamp = (time_t)m_end_time;
			activity_loacl_time = gmtime(&activity_end_tamp);
			if (NULL != activity_loacl_time)
			{
				const SaltySweetOtherCfg & other_cfg = activity->GetOtherCfg();
				tm end_tm;
				end_tm.tm_year = activity_loacl_time->tm_year;
				end_tm.tm_mon = activity_loacl_time->tm_mon;
				end_tm.tm_mday = activity_loacl_time->tm_mday;
				end_tm.tm_hour = other_cfg.vote_time_end / 100;
				end_tm.tm_min = other_cfg.vote_time_end % 100;
				end_tm.tm_sec = 0;

				m_param.activity_end_tamp = (unsigned int)mktime(&end_tm);
				m_param.add_value_tamp = other_cfg.times + (unsigned int)EngineAdapter::Instance().Time();
			}
			m_param.status_mod = SALTY_SWEET_MOD_VOTE;
		}

		this->RASendGameInfo(NULL, true);
	}
	else if (to_status == ACTIVITY_STATUS_CLOSE)
	{
		m_param.NextReset();
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s from_status[%d] to_status[%d] status_mod[%d] activity_end_tamp[%d]", __FUNCTION__,
		from_status, to_status, m_param.status_mod, m_param.activity_end_tamp);
}

void RandActivitySaltySweetBattle::SyncActivityDataFromCross(CrossRandActivityDataParam & param)
{
	RandActivity::SyncActivityDataFromCross(param);
	if (CrossConfig::Instance().IsHiddenServer())
	{
		UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(param.data));
		m_param = *(RASaltySweetBattleServerParam *)param.data;
	}
}

void RandActivitySaltySweetBattle::OnGameDataChange(int pre_win_type, int salty_value, int sweet_value, int status_mod, int to_uid)
{
	if (!CrossConfig::Instance().IsHiddenServer())return;
	m_param.pre_win_type = pre_win_type;
	m_param.salty_value = salty_value;
	m_param.sweet_value = sweet_value;
	m_param.status_mod = status_mod;
	if (to_uid > 0)
	{
		Protocol::SCRASaltySweetBattleInfo protocol;
		protocol.salty_value = m_param.salty_value;
		protocol.sweet_value = m_param.sweet_value;
		protocol.status_mod = m_param.status_mod;
		World::GetInstWorld()->SendToRole(to_uid, (const char *)&protocol, sizeof(protocol));
	}
}

void RandActivitySaltySweetBattle::OnRoleVote(int add_type, int add_value, Role * role)
{
	if (CrossConfig::Instance().IsHiddenServer())return;
	if (m_param.status_mod != SALTY_SWEET_MOD_VOTE)return;

	if (add_type == SALTY_SWEET_TYPE_SALTY)m_param.salty_value += add_value;
	else if(add_type == SALTY_SWEET_TYPE_SWEET) m_param.sweet_value += add_value;
	else
	{
		m_param.salty_value += add_value;
		m_param.sweet_value += add_value;
	}

	if (m_param.salty_value < 0)m_param.salty_value = MAX_INT;
	if (m_param.sweet_value < 0)m_param.sweet_value = MAX_INT;
	this->OnDataChangeSendCross();
	this->RASendGameInfo(role);
}

void RandActivitySaltySweetBattle::OnUidVote(int add_type, int add_value, int uid)
{
	if (CrossConfig::Instance().IsHiddenServer())return;
	if (m_param.status_mod != SALTY_SWEET_MOD_VOTE)return;

	if (add_type == SALTY_SWEET_TYPE_SALTY)m_param.salty_value += add_value;
	else if (add_type == SALTY_SWEET_TYPE_SWEET) m_param.sweet_value += add_value;
	else
	{
		m_param.salty_value += add_value;
		m_param.sweet_value += add_value;
	}

	if (m_param.salty_value < 0)m_param.salty_value = MAX_INT;
	if (m_param.sweet_value < 0)m_param.sweet_value = MAX_INT;
	this->OnDataChangeSendCross(uid);
}

void RandActivitySaltySweetBattle::GetWinType(int * pre_win, int * status)
{
	*pre_win = m_param.pre_win_type;
	*status = m_param.status_mod;
}

void RandActivitySaltySweetBattle::RASendGameInfo(Role * role, bool is_brocast)
{
	Protocol::SCRASaltySweetBattleInfo protocol;
	protocol.salty_value = m_param.salty_value;
	protocol.sweet_value = m_param.sweet_value;
	protocol.status_mod = m_param.status_mod;

	if (role != NULL)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}
}

void RandActivitySaltySweetBattle::OnDataChangeSendCross(int to_uid)
{
	if (CrossConfig::Instance().IsHiddenServer())return;
	crossgameprotocal::GameHiddenSaltySweetBattleChangeReq change_data;
	change_data.unique_server_id = LocalConfig::Instance().GetUniqueServerID();
	change_data.pre_win_type = m_param.pre_win_type;
	change_data.salty_value = m_param.salty_value;
	change_data.sweet_value = m_param.sweet_value;
	change_data.status_mod = m_param.status_mod;
	change_data.to_uid = to_uid;
	InternalComm::Instance().SendToHiddenThroughCross((const char*)&change_data, sizeof(change_data));
}

void RandActivitySaltySweetBattle::EndGame()
{
	if (m_param.salty_value == m_param.sweet_value)
	{
		int win_type = (rand() % SALTY_SWEET_TYPE_SWEET) + SALTY_SWEET_TYPE_SALTY;
		if (win_type == SALTY_SWEET_TYPE_SALTY)
		{
			m_param.salty_value += 1;
		}
		else
		{
			m_param.sweet_value += 1;
		}
		m_param.pre_win_type = m_param.salty_value > m_param.sweet_value? SALTY_SWEET_TYPE_SALTY : SALTY_SWEET_TYPE_SWEET;
	}
	else if (m_param.salty_value < m_param.sweet_value)
	{
		m_param.pre_win_type = SALTY_SWEET_TYPE_SWEET;
	}
	else
	{
		m_param.pre_win_type = SALTY_SWEET_TYPE_SALTY;
	}
	this->OnDataChangeSendCross();
}
