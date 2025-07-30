#include "roleactivitygodtoken.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitygodtokenconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/funopen/funopen.hpp"
#include "protocol/randactivity/msgragodtoken.hpp"

#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/event/eventhandler.hpp"

RoleActivityGodToken::RoleActivityGodToken(int type)
	:RoleActivity(type), ra_god_token_cacl_time(0), m_mail_reclaim_num(0)
{

}

RoleActivityGodToken::~RoleActivityGodToken()
{

}

void RoleActivityGodToken::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAGodTokenParam));
}

void RoleActivityGodToken::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RAGodTokenParam));
}

void RoleActivityGodToken::OnLogin()
{
	this->OnRAGodTokenCaclTimeSeq();

	if (0 == m_param.ra_god_token_is_open)
	{
		this->CheckRoleGodTokenIsOpen();
	}

	if (this->IsRoleraGodTokenOpen() && 0 == m_param.ra_god_token_daily_task_level && 0 == m_param.ra_god_token_weekly_task_level)
	{
		ra_god_token_cacl_time = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 2;
	}
}

void RoleActivityGodToken::OnLoginSendInfo()
{
	this->SendRAGodTokenInfo();
}

void RoleActivityGodToken::Update(time_t now_second)
{
	//离线时活动开启 延迟2秒刷新日、周任务（考虑 OnDayChange、OnWeekChange情况）
	if ((unsigned int)now_second >= ra_god_token_cacl_time && 0 != ra_god_token_cacl_time)
	{
		ra_god_token_cacl_time = 0;
		if (this->IsRoleraGodTokenOpen())
		{
			if (0 == m_param.ra_god_token_daily_task_level && 0 == m_param.ra_god_token_weekly_task_level)
			{
				this->OnRAGodTokenRefreshTask(RA_GOD_TOKEN_TASK_TYPE_WEEKLY);
				this->OnRAGodTokenRefreshTask(RA_GOD_TOKEN_TASK_TYPE_DAILY);
				this->SendRAGodTokenInfo();
			}
		}
	}

	if (0 == m_param.weekly_task_add_mark)
	{//周任务6个改成10个，没到周刷新时间 给加多四个任务
		this->AddWeeklyTask();
		m_param.weekly_task_add_mark = 2;
	}
	else if (1 == m_param.weekly_task_add_mark)
	{
		this->ChangeWeelyTask();
		m_param.weekly_task_add_mark = 2;
	}

	//检测赛季结束
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL != ramgr)
	{
		if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_GOD_TOKEN) && this->IsRoleraGodTokenOpen())
		{
			const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
			if (NULL == config)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return;
			}

			if (0 == m_param.time_seq_timestamp)
			{
				m_param.time_seq_timestamp = config->GetTimeCfg(m_param.ra_god_token_time_seq);
			}

			int time_seq = config->GetTimeSeqCfg((unsigned int)now_second);
			if (0 > time_seq || time_seq == m_param.ra_god_token_time_seq) return;

			unsigned int begin_time = config->GetTimeCfg(time_seq);
			if (0 >= begin_time) return;

			if (begin_time > m_param.time_seq_timestamp)
			{
				this->OnTimeSeqChange();
			}
			else
			{
				m_param.ra_god_token_time_seq = (char)time_seq;
				m_param.time_level = m_role_module_mgr->GetRole()->GetLevel();
				this->SendRAGodTokenInfo();
			}
		}
	}
}

void RoleActivityGodToken::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	memset(m_param.ra_god_token_daily_type_count, 0, sizeof(m_param.ra_god_token_daily_type_count));

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL != ramgr && ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_GOD_TOKEN) && this->IsRoleraGodTokenOpen())
	{
		m_param.ra_god_token_week_task_limit_act_flag.Reset();

		//刷新日任务、第一次刷新在update
		if (0 != m_param.ra_god_token_daily_task_level)
		{
			this->OnRAGodTokenRefreshTask(RA_GOD_TOKEN_TASK_TYPE_DAILY);
		}

		for (int i = 0; i < RA_GOD_TOKEN_EXCHANGE_MAXNUM; ++i)
		{
			const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
			if (NULL == config)
			{
				return;
			}

			const RAGodTokenExchangeCfg * e_cfg = config->GetExchangeCfg(m_param.ra_god_token_time_seq, i);
			if (NULL == e_cfg) continue;

			if (0 != e_cfg->daily_exchange)
			{
				m_param.ra_god_token_exchange_count[i] = 0;
			}
		}
		this->SendRAGodTokenInfo();
		this->SetRoleDataChange();
	}
}

void RoleActivityGodToken::OnWeekChange()
{
	memset(m_param.ra_god_token_weekly_type_count, 0, sizeof(m_param.ra_god_token_weekly_type_count));

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL != ramgr && ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_GOD_TOKEN) && this->IsRoleraGodTokenOpen())
	{
		//刷新周任务、第一次刷新在update
		if (0 != m_param.ra_god_token_weekly_task_level)
		{
			this->OnRAGodTokenRefreshTask(RA_GOD_TOKEN_TASK_TYPE_WEEKLY);
		}
	}
}

void RoleActivityGodToken::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_GOD_TOKEN != rand_act_type)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity *activity = ramgr->ForceGetRandActivity(rand_act_type);
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_god_token_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		if (-1 != m_param.ra_god_token_time_seq)
		{
			//离线时活动关闭 上线检测前发放
			this->OnRAGodTokenPatchLevelReward();		//先把上一轮没领取的等级奖励补发
			this->OnRAGodTokenReclaimItem();
		}

		m_param.Reset();
		m_param.ra_god_token_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->SetRoleDataChange();
	}
}

void RoleActivityGodToken::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_GOD_TOKEN))
	{
		//m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}

	if (!this->IsRoleraGodTokenOpen())
	{
		return;
	}

	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN);

	switch (raor->opera_type)
	{
	case Protocol::RA_GOD_TOKEN_OP_TYPE_INFO:
		{
			this->SendRAGodTokenInfo();
		}
		break;
	case Protocol::RA_GOD_TOKEN_OP_TYPE_FETCH_COMMON:
		{
			this->OnRAGodTokenFetchCommonReward(raor->param_1);
		}
		break;
	case Protocol::RA_GOD_TOKEN_OP_TYPE_FETCH_SENIOR:
		{
			this->OnRAGodTokenFetchSeniorReward(raor->param_1);
		}
		break;
	case Protocol::RA_GOD_TOKEN_OP_TYPE_BUY_EXP:
		{
			this->OnRAGodTokenBuyExp(raor->param_1); 
		}
		break;
	case Protocol::RA_GOD_TOKEN_OP_TYPE_EXCHANGE:
		{
			this->OnRAGodTokenExchange(raor->param_1);
		}
		break;
	case Protocol::RA_GOD_TOKEN_OP_TYPE_FETCH_SURPLUS:
		{
			this->OnRAGodTokenFetchSurplusReward();
		}
		break;
	case Protocol::RA_GOD_TOKEN_OP_TYPE_FETCH_DAILY_TASK:
		{
			this->OnRAGodTokenFetchDailyReward(raor->param_1);
		}
		break;
	case Protocol::RA_GOD_TOKEN_OP_TYPE_FETCH_WEEKLY_TASK:
		{
			this->OnRAGodTokenFetchWeeklyReward(raor->param_1);
		}
		break;
	}
}

void RoleActivityGodToken::OnRAOpenOnlineSend()
{
	this->OnRAGodTokenCaclTimeSeq();
	this->CheckRoleGodTokenIsOpen();
	if (this->IsRoleraGodTokenOpen())
	{
		this->OnRAGodTokenRefreshTask(RA_GOD_TOKEN_TASK_TYPE_WEEKLY);
		this->OnRAGodTokenRefreshTask(RA_GOD_TOKEN_TASK_TYPE_DAILY);
	}
}

void RoleActivityGodToken::OnRoleLevelUp(int old_level, int to_level)
{
	if (!IsRoleraGodTokenOpen())
	{
		this->OnRAGodTokenCaclTimeSeq();
		this->CheckRoleGodTokenIsOpen();
		if (this->IsRoleraGodTokenOpen())
		{
			this->OnRAGodTokenRefreshTask(RA_GOD_TOKEN_TASK_TYPE_WEEKLY);
			this->OnRAGodTokenRefreshTask(RA_GOD_TOKEN_TASK_TYPE_DAILY);
			this->SendRAGodTokenInfo();
		}
	}
}

void RoleActivityGodToken::OnRAGodTokenAddCount(int type, int count /*= 1*/, int param /*= 0*/)
{
	if (0 >= type || type >= RA_GOD_TOKEN_DAY_TYPE_MAX) return;

	/*RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_GOD_TOKEN)) return;
	if (!this->IsRoleraGodTokenOpen()) return;
	if (!this->IsRAGodTokenTaskType(RA_GOD_TOKEN_TASK_TYPE_DAILY, type)) return;

	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL == config) return;

	const RAGodTokenTaskCfg * cfg = config->GetTaskCfg(m_param.ra_god_token_time_seq, m_param.ra_god_token_daily_task_level, type);
	if (NULL == cfg) return;

	if (m_param.ra_god_token_daily_type_count[type] >= cfg->param) return;

	if (m_param.ra_god_token_daily_type_count[type] + count >= cfg->param)
	{
		m_param.ra_god_token_daily_type_count[type] = cfg->param;
	}
	else
	{
		m_param.ra_god_token_daily_type_count[type] += count;
	}*/

	m_param.ra_god_token_daily_type_count[type] += count;
	this->SetRoleDataChange();

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRAGodTokenAddCount user[%d, %s], level[%d], act_type[%d], day_type[%d], count[%d], param[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_GOD_TOKEN,
		type, count, param);

	this->CheckDailyReward(type);

	if (this->IsRoleraGodTokenOpen() && this->IsRAGodTokenTaskType(RA_GOD_TOKEN_TASK_TYPE_DAILY, type))
	{
		this->SendRAGodTokenInfo();
	}
}

void RoleActivityGodToken::OnRAGodTokenWeekAddCount(int type, int count /*= 1*/, int param /*= 0*/)
{
	if (0 >= type || type >= RA_GOD_TOKEN_WEEK_TYPE_MAX) return;

	if (type == RA_GOD_TOKEN_TYPE_WEEK_LIMIT_ACTIVITY)
	{
		if (m_param.ra_god_token_week_task_limit_act_flag.IsBitSet(param)) return;
	}

	/*RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_GOD_TOKEN)) return;
	if (!this->IsRoleraGodTokenOpen()) return;
	if (!this->IsRAGodTokenTaskType(RA_GOD_TOKEN_TASK_TYPE_WEEKLY, type)) return;

	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL == config) return;

	const RAGodTokenTaskCfg * cfg = config->GetWeekTaskCfg(m_param.ra_god_token_time_seq, m_param.ra_god_token_weekly_task_level, type);
	if (NULL == cfg) return;

	if (m_param.ra_god_token_weekly_type_count[type] >= cfg->param) return;

	if (m_param.ra_god_token_weekly_type_count[type] + count >= cfg->param)
	{
		m_param.ra_god_token_weekly_type_count[type] = cfg->param;
	}
	else
	{
		m_param.ra_god_token_weekly_type_count[type] += count;
	}*/

	m_param.ra_god_token_weekly_type_count[type] += count;

	if (type == RA_GOD_TOKEN_TYPE_WEEK_LIMIT_ACTIVITY)
	{
		m_param.ra_god_token_week_task_limit_act_flag.SetBit(param);
	}
	this->SetRoleDataChange();

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRAGodTokenWeekAddCount user[%d, %s], level[%d], act_type[%d], day_type[%d], count[%d], param[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_GOD_TOKEN,
		type, count, param);

	this->CheckWeekReward(type);

	if (this->IsRoleraGodTokenOpen() && this->IsRAGodTokenTaskType(RA_GOD_TOKEN_TASK_TYPE_WEEKLY, type))
	{
		this->SendRAGodTokenInfo();
	}
}

void RoleActivityGodToken::CheckRoleGodTokenIsOpen()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_GOD_TOKEN))
	{
		return;
	}

	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int open_level = config->GetMinOpenLevel(m_param.ra_god_token_time_seq);
	if (0 >= open_level) return;

	if (m_role_module_mgr->GetRole()->GetLevel() >= open_level)
	{
		m_param.ra_god_token_is_open = 1;
		this->SendRAGodTokenInfo();
		this->SetRoleDataChange();
	}
}

void RoleActivityGodToken::OnReSetRAGodToken(bool is_gm /*= false*/)
{
	if (is_gm)
	{
		m_param.ra_god_token_time_seq = -1;
	}
	m_param.ra_god_token_is_open = 0;
	m_param.ra_god_token_is_active_token = 0;

	m_param.ra_god_token_level = 0;
	m_param.ra_god_token_exp = 0;

	m_param.ra_god_token_fetch_common_flag.Reset();
	m_param.ra_god_token_fetch_senior_flag.Reset();

	memset(m_param.ra_god_token_exchange_count, 0, sizeof(m_param.ra_god_token_exchange_count));

	m_param.ra_god_token_daily_task_level = 0;
	memset(m_param.ra_god_token_daily_task_type, 0, sizeof(m_param.ra_god_token_daily_task_type));
	memset(m_param.ra_god_token_daily_type_count, 0, sizeof(m_param.ra_god_token_daily_type_count));
	m_param.ra_god_token_daily_reward_flag.Reset();

	m_param.ra_god_token_weekly_task_level = 0;
	memset(m_param.ra_god_token_weekly_task_type, 0, sizeof(m_param.ra_god_token_weekly_task_type));
	memset(m_param.ra_god_token_weekly_type_count, 0, sizeof(m_param.ra_god_token_weekly_type_count));
	m_param.ra_god_token_week_reward_flag.Reset();

	m_param.surplus_exp = 0;
	m_param.surplus_receive_count = 0;
	m_param.surplus_senior_count = 0;

	m_param.ra_god_token_week_task_limit_act_flag.Reset();

	m_param.time_level = m_role_module_mgr->GetRole()->GetLevel();
	m_param.is_change_level_flag = 1;

	this->SetRoleDataChange();
}

void RoleActivityGodToken::SendRAGodTokenInfo()
{
	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL == config)
	{
		return;
	}

	Protocol::SCRAGodTokenInfo info;
	info.time_level = m_param.time_level;
	info.time_seq = m_param.ra_god_token_time_seq;
	info.is_open = m_param.ra_god_token_is_open;
	info.is_buy_token = m_param.ra_god_token_is_active_token;
	info.sh_reserver = 0;

	info.token_level = m_param.ra_god_token_level;
	info.token_exp = m_param.ra_god_token_exp;

	memcpy(info.fetch_common_flag.byte_list, m_param.ra_god_token_fetch_common_flag.byte_list, sizeof(info.fetch_common_flag.byte_list));
	memcpy(info.fetch_senior_flag.byte_list, m_param.ra_god_token_fetch_senior_flag.byte_list, sizeof(info.fetch_senior_flag.byte_list));

	info.daily_level = m_param.ra_god_token_daily_task_level;
	memcpy(info.daily_type, m_param.ra_god_token_daily_task_type, sizeof(info.daily_type));
	for (int i = 0; i < RA_GOD_TOKEN_DAILY_TASK_NUM; ++i)
	{
		if (m_param.ra_god_token_daily_reward_flag.IsBitSet(m_param.ra_god_token_daily_task_type[i]))
		{
			info.daily_type_finish_count[i] = -1;
		}
		else
		{
			info.daily_type_finish_count[i] = m_param.ra_god_token_daily_type_count[m_param.ra_god_token_daily_task_type[i]];
		}
	}

	info.weekly_level = m_param.ra_god_token_weekly_task_level;
	memcpy(info.weekly_type, m_param.ra_god_token_weekly_task_type, sizeof(info.weekly_type));
	for (int i = 0; i < RA_GOD_TOKEN_WEEKLY_TASK_NUM; ++i)
	{
		if (m_param.ra_god_token_week_reward_flag.IsBitSet(m_param.ra_god_token_weekly_task_type[i]))
		{
			info.weekly_type_finish_count[i] = -1;
		}
		else
		{
			info.weekly_type_finish_count[i] = m_param.ra_god_token_weekly_type_count[m_param.ra_god_token_weekly_task_type[i]];
		}
	}

	info.surplus_exp = m_param.surplus_exp;
	info.surplus_receive_count = m_param.surplus_receive_count;

	info.count = config->GetExchangeCfgLen(m_param.ra_god_token_time_seq);
	for (int i = 0; i < info.count; ++i)
	{
		info.exchange_count[i] = m_param.ra_god_token_exchange_count[i];
	}

	int length = sizeof(info) - (sizeof(info.exchange_count[0]) * (RA_GOD_TOKEN_EXCHANGE_MAXNUM - info.count));
	m_role_module_mgr->NetSend(&info, length);
}

void RoleActivityGodToken::OnTimeSeqChange()
{
	/*this->OnRAGodTokenPatchLevelReward();
	this->OnRAGodTokenReclaimItem();
	this->OnReSetRAGodToken();*/
	this->OnRAGodTokenCaclTimeSeq();
	this->CheckRoleGodTokenIsOpen();
	if (this->IsRoleraGodTokenOpen())
	{
		this->OnRAGodTokenRefreshTask(RA_GOD_TOKEN_TASK_TYPE_WEEKLY);
		this->OnRAGodTokenRefreshTask(RA_GOD_TOKEN_TASK_TYPE_DAILY);
		this->SendRAGodTokenInfo();
	}
}

void RoleActivityGodToken::OnRAGodTokenRefreshTask(int task_type)
{
	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig * >
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL == config)
	{
		return;
	}

	switch (task_type)
	{
	case RA_GOD_TOKEN_TASK_TYPE_DAILY:
		{
			char day_type_list[RA_GOD_TOKEN_TASK_NUM];
			memset(day_type_list, 0, sizeof(day_type_list));

			m_param.ra_god_token_daily_reward_flag.Reset();

			m_param.ra_god_token_daily_task_level = m_role_module_mgr->GetRole()->GetLevel();
			config->RandDailyTask(m_param.ra_god_token_time_seq, m_param.ra_god_token_daily_task_level, day_type_list);
			memcpy(m_param.ra_god_token_daily_task_type, day_type_list, sizeof(m_param.ra_god_token_daily_task_type));

			//保证周任务在日任务之前刷新
			this->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_LOGIN);
			this->OnRAGodTokenWeekAddCount(RA_GOD_TOKEN_TYPE_WEEK_LOGIN);

			this->SetRoleDataChange();
		}
		break;
	case RA_GOD_TOKEN_TASK_TYPE_WEEKLY:
		{
			char week_type_list[RA_GOD_TOKEN_WEEKLY_TASK_NUM];
			memset(week_type_list, 0, sizeof(week_type_list));

			m_param.ra_god_token_week_reward_flag.Reset();

			m_param.ra_god_token_weekly_task_level = m_role_module_mgr->GetRole()->GetLevel();
			config->RandWeeklyTask(m_param.ra_god_token_time_seq, m_param.ra_god_token_weekly_task_level, week_type_list);
			memcpy(m_param.ra_god_token_weekly_task_type, week_type_list, sizeof(m_param.ra_god_token_weekly_task_type));

			this->SetRoleDataChange();
		}
		break;
	default:
		break;
	}
}

bool RoleActivityGodToken::OnRAGodTokenBuyToken(bool is_gm /*= false*/)
{
	if (!this->IsRoleraGodTokenOpen()) return false;

	if (1 == m_param.ra_god_token_is_active_token)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_GOD_TOKEN_IS_ACTIVE);
		return false;
	}

	m_param.ra_god_token_is_active_token = 1;
	this->OnRAGodTokenPatchSurplusSeniorReward();
	this->SendRAGodTokenInfo();
	this->SetRoleDataChange();

	if (is_gm)
	{
		const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
		if (NULL != config)
		{
			const RAGodTokenOtherCfg& other_cfg = config->GetOtherCfg();
			EventHandler::Instance().OnAddChongzhi(m_role_module_mgr->GetRole(), other_cfg.buy_token, false);
		}
	}

	EventHandler::Instance().OnRAGodTokenBuyToken(m_role_module_mgr->GetRole());

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], act_type[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(),
		m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_GOD_TOKEN);
	return true;
}

int RoleActivityGodToken::OnBuyGiftItem(ARG_OUT int * price)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return 1;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return 1;
	}
	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}
	if (NULL != price)
	{
		*price = config->GetOtherCfg().buy_token;
		return 0;
	}

	if (!this->IsRoleraGodTokenOpen()) return 1;

	if (1 == m_param.ra_god_token_is_active_token)
	{
		return 2;
	}

	m_param.ra_god_token_is_active_token = 1;
	this->OnRAGodTokenPatchSurplusSeniorReward();
	this->SendRAGodTokenInfo();
	this->SetRoleDataChange();


	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], act_type[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(),
			m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_GOD_TOKEN);
	return 0;
}

void RoleActivityGodToken::OnRAGodTokenCaclTimeSeq()
{
	unsigned int now_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig * >
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL == config)
	{
		return;
	}

	int time_seq = config->GetTimeSeqCfg(now_time);
	unsigned int begin_time = config->GetTimeCfg(time_seq);

	if (time_seq == m_param.ra_god_token_time_seq && -1 != m_param.ra_god_token_time_seq && begin_time == m_param.time_seq_timestamp)
	{
		if (1 != m_param.is_change_level_flag)
		{
			m_param.time_level = m_role_module_mgr->GetRole()->GetLevel();
			m_param.is_change_level_flag = 1;
		}
		return;
	}
	else
	{
		if (-1 != m_param.ra_god_token_time_seq && begin_time > m_param.time_seq_timestamp)
		{
			this->OnRAGodTokenPatchLevelReward();
			this->OnRAGodTokenReclaimItem();
			this->OnReSetRAGodToken();
		}

		m_param.ra_god_token_time_seq = (char)time_seq;
		m_param.time_seq_timestamp = config->GetTimeCfg(time_seq);
		m_param.time_level = m_role_module_mgr->GetRole()->GetLevel();
		m_param.is_change_level_flag = 1;
		this->SetRoleDataChange();
	}
}

bool RoleActivityGodToken::IsRAGodTokenTaskType(int task_type, int type)
{
	switch (task_type)
	{
	case RA_GOD_TOKEN_TASK_TYPE_DAILY:
		{
			for (int i = 0; i < RA_GOD_TOKEN_DAILY_TASK_NUM; ++i)
			{
				if (type == m_param.ra_god_token_daily_task_type[i])
				{
					return true;
				}
			}
		}
		break;
	case RA_GOD_TOKEN_TASK_TYPE_WEEKLY:
		{
			for (int i = 0; i < RA_GOD_TOKEN_WEEKLY_TASK_NUM; ++i)
			{
				if (type == m_param.ra_god_token_weekly_task_type[i])
				{
					return true;
				}
			}
		}
		break;
	default:
		break;
	}

	return false;
}

void RoleActivityGodToken::OnRAGodTokenFetchCommonReward(int seq)
{
	if (0 > seq) return;

	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAGodTokenOtherCfg& other_cfg = config->GetOtherCfg();
	if (seq >= other_cfg.max_leve)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const RAGodTokenLevelCfg * cfg = config->GetLevelCfg(m_param.ra_god_token_time_seq, seq, m_param.time_level);
	if (NULL == cfg) return;

	if (m_param.ra_god_token_level < cfg->open_level - 1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_GOD_TOKEN_LEVEL_NOT_ENOUGH);
		return;
	}

	if (m_param.ra_god_token_fetch_common_flag.IsBitSet(seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_BOSS_CHAPTER_REWARD_CLAIMED);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->common_item.item_id, cfg->common_item.num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
		return;
	}

	//给予
	if (!m_role_module_mgr->GetKnapsack()->Put(cfg->common_item, PUT_REASON_RA_GOD_TOKEN))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_param.ra_god_token_fetch_common_flag.SetBit(seq);

	this->SendRAGodTokenInfo();
	this->SetRoleDataChange();

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRAGodTokenFetchCommonReward user[%d, %s], level[%d], act_type[%d], fetch_seq[%d], fetch_item[%d %d %d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_GOD_TOKEN,
		seq, cfg->common_item.item_id, cfg->common_item.num, cfg->common_item.is_bind ? 1 : 0);

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityGodToken::OnRAGodTokenFetchSeniorReward(int seq)
{
	if (0 > seq) return;

	if (1 != m_param.ra_god_token_is_active_token)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_GOD_TOKEN_PLEASE_ACTIVE);
		return;
	}

	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAGodTokenOtherCfg& other_cfg = config->GetOtherCfg();
	if (seq >= other_cfg.max_leve)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const RAGodTokenLevelCfg * cfg = config->GetLevelCfg(m_param.ra_god_token_time_seq, seq, m_param.time_level);
	if (NULL == cfg) return;

	if (m_param.ra_god_token_level < cfg->open_level - 1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_GOD_TOKEN_LEVEL_NOT_ENOUGH);
		return;
	}

	if (m_param.ra_god_token_fetch_senior_flag.IsBitSet(seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_BOSS_CHAPTER_REWARD_CLAIMED);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)cfg->senior_item.size(), &cfg->senior_item[0]))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
		return;
	}

	//给予
	if (!m_role_module_mgr->GetKnapsack()->PutList((short)cfg->senior_item.size(), &cfg->senior_item[0], PUT_REASON_RA_GOD_TOKEN))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}
	if (0 < cfg->senior_gold)
	{
		m_role_module_mgr->GetMoney()->AddGold(cfg->senior_gold, __FUNCTION__);
	}

	m_param.ra_god_token_fetch_senior_flag.SetBit(seq);

	this->SendRAGodTokenInfo();
	this->SetRoleDataChange();

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRAGodTokenFetchSeniorReward user[%d, %s], level[%d], act_type[%d], fetch_seq[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_GOD_TOKEN, seq);

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityGodToken::OnRAGodTokenPatchLevelReward()
{
	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAGodTokenOtherCfg& other_cfg = config->GetOtherCfg();

	int add_gold = 0;
	std::map<ItemID, GodTokenItemData> item_list;

	//补发未领取的等级奖励
	for (int i = 0; i < other_cfg.max_leve; ++i)
	{
		const RAGodTokenLevelCfg* cfg = config->GetLevelCfg(m_param.ra_god_token_time_seq, i, m_param.time_level);
		if(NULL == cfg) continue;

		if (m_param.ra_god_token_level < cfg->open_level)
		{
			break;
		}

		if (!m_param.ra_god_token_fetch_common_flag.IsBitSet(i))
		{
			m_param.ra_god_token_fetch_common_flag.SetBit(i);

			//回收物品不补发
			if (cfg->common_item.item_id == other_cfg.reclaim_item)
			{
				m_mail_reclaim_num += cfg->common_item.num;
			}
			else
			{
				std::map<ItemID, GodTokenItemData>::iterator zit = item_list.find(cfg->common_item.item_id);
				if (zit == item_list.end())
				{
					if (cfg->common_item.is_bind) item_list[cfg->common_item.item_id].is_bind_num = cfg->common_item.num;
					else						 item_list[cfg->common_item.item_id].no_bind_num = cfg->common_item.num;
				}
				else
				{
					if (cfg->common_item.is_bind) item_list[cfg->common_item.item_id].is_bind_num += cfg->common_item.num;
					else						 item_list[cfg->common_item.item_id].no_bind_num += cfg->common_item.num;
				}
			}
		}

		if (1 != m_param.ra_god_token_is_active_token)
		{
			continue;
		}

		if (!m_param.ra_god_token_fetch_senior_flag.IsBitSet(i))
		{
			m_param.ra_god_token_fetch_senior_flag.SetBit(i);

			if (0 < cfg->senior_gold) add_gold += cfg->senior_gold;

			for (int idx = 0; idx < (int)cfg->senior_item.size(); ++idx)
			{
				//回收物品不补发
				if (cfg->senior_item[idx].item_id == other_cfg.reclaim_item)
				{
					m_mail_reclaim_num += cfg->senior_item[idx].num;
				}
				else
				{
					std::map<ItemID, GodTokenItemData>::iterator zit = item_list.find(cfg->senior_item[idx].item_id);
					if (zit == item_list.end())
					{
						if (cfg->senior_item[idx].is_bind)	item_list[cfg->senior_item[idx].item_id].is_bind_num = cfg->senior_item[idx].num;
						else								item_list[cfg->senior_item[idx].item_id].no_bind_num = cfg->senior_item[idx].num;
					}
					else
					{
						if (cfg->senior_item[idx].is_bind)	item_list[cfg->senior_item[idx].item_id].is_bind_num += cfg->senior_item[idx].num;
						else								item_list[cfg->senior_item[idx].item_id].no_bind_num += cfg->senior_item[idx].num;
					}
				}
			}	
		}
	}

	//补发未领取的溢出奖励
	if (m_param.surplus_receive_count > 0)
	{
		const RAGodTokenSurplusCfg * s_cfg = config->GetSurplusCfg();
		if (NULL != s_cfg)
		{
			for (int idx = 0; idx < (int)s_cfg->common_item.size(); ++idx)
			{
				//回收物品不补发
				if (s_cfg->common_item[idx].item_id == other_cfg.reclaim_item)
				{
					m_mail_reclaim_num += s_cfg->common_item[idx].num;
				}
				else
				{
					std::map<ItemID, GodTokenItemData>::iterator zit = item_list.find(s_cfg->common_item[idx].item_id);
					if (zit == item_list.end())
					{
						if (s_cfg->common_item[idx].is_bind)
						{
							item_list[s_cfg->common_item[idx].item_id].is_bind_num = s_cfg->common_item[idx].num * m_param.surplus_receive_count;
						}
						else item_list[s_cfg->common_item[idx].item_id].no_bind_num = s_cfg->common_item[idx].num * m_param.surplus_receive_count;
					}
					else
					{
						if (s_cfg->common_item[idx].is_bind)
						{
							item_list[s_cfg->common_item[idx].item_id].is_bind_num += s_cfg->common_item[idx].num * m_param.surplus_receive_count;
						}
						else item_list[s_cfg->common_item[idx].item_id].no_bind_num += s_cfg->common_item[idx].num * m_param.surplus_receive_count;
					}
				}
			}

			if (1 == m_param.ra_god_token_is_active_token)
			{
				for (int idx = 0; idx < (int)s_cfg->senior_item.size(); ++idx)
				{
					//回收物品不补发
					if (s_cfg->senior_item[idx].item_id == other_cfg.reclaim_item)
					{
						m_mail_reclaim_num += s_cfg->senior_item[idx].num;
					}
					else
					{
						std::map<ItemID, GodTokenItemData>::iterator zit = item_list.find(s_cfg->senior_item[idx].item_id);
						if (zit == item_list.end())
						{
							if (s_cfg->senior_item[idx].is_bind)
							{
								item_list[s_cfg->senior_item[idx].item_id].is_bind_num = s_cfg->senior_item[idx].num * m_param.surplus_receive_count;
							}
							else item_list[s_cfg->senior_item[idx].item_id].no_bind_num = s_cfg->senior_item[idx].num * m_param.surplus_receive_count;
						}
						else
						{
							if (s_cfg->senior_item[idx].is_bind)
							{
								item_list[s_cfg->senior_item[idx].item_id].is_bind_num += s_cfg->senior_item[idx].num * m_param.surplus_receive_count;
							}
							else item_list[s_cfg->senior_item[idx].item_id].no_bind_num += s_cfg->senior_item[idx].num * m_param.surplus_receive_count;
						}
					}		
				}
			}
		}

		m_param.surplus_receive_count = 0;
	}

	std::vector<ItemConfigData> reward_list;
	for (std::map<ItemID, GodTokenItemData>::const_iterator it = item_list.begin(); it != item_list.end(); ++it)
	{
		if (it->second.is_bind_num > 0)
		{
			ItemConfigData item(it->first, true, it->second.is_bind_num);
			reward_list.push_back(item);
		}
		
		if(it->second.no_bind_num > 0)
		{
			ItemConfigData item(it->first, false, it->second.no_bind_num);
			reward_list.push_back(item);
		}
	}

	int time = (int)reward_list.size() / MAX_ATTACHMENT_ITEM_NUM;
	if ((int)reward_list.size() % MAX_ATTACHMENT_ITEM_NUM > 0)
	{
		time += 1;
	}

	if ((int)reward_list.size() > 0)
	{
		for (int index = 0; index < time; ++index)
		{
			MailContentParam contentparam;
			contentparam.Reset();

			int length_1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_god_token_level_reward_title, m_param.time_seq_timestamp);
			int length_2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_god_token_level_reward_content, m_param.time_seq_timestamp);

			for (int i = 0; i < (int)reward_list.size() && i < MAX_ATTACHMENT_ITEM_NUM; ++i)
			{
				int it_idx = index * MAX_ATTACHMENT_ITEM_NUM + i;
				if (it_idx >= (int)reward_list.size()) break;

				contentparam.item_list[i] = ItemDataWrapper(reward_list[it_idx].item_id, reward_list[it_idx].num, reward_list[it_idx].is_bind);
			}

			if (0 == index)
			{
				if(0 < add_gold) contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] = add_gold;
			}

			if (length_1 > 0 && length_2 > 0)
			{
				MailRoute::MailToUser(m_role_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
			}
		}
	}

	this->SetRoleDataChange();
}

void RoleActivityGodToken::OnRAGodTokenReclaimItem()
{
	//if (CrossConfig::Instance().IsHiddenServer())return;
	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAGodTokenOtherCfg & cfg = config->GetOtherCfg();
	const ItemBase* item_base = ITEMPOOL->GetItem(cfg.reclaim_item);
	if (NULL == item_base) return;

	int num = m_role_module_mgr->GetKnapsack()->ReclaimItem(cfg.reclaim_item);

	num += m_mail_reclaim_num;
	m_mail_reclaim_num = 0;

	if (num > 0)
	{
		MailContentParam contentparam;
		int length1 = 0;
		int length2 = 0;

		//发放邮件
		int sell_price = item_base->GetSellPrice(NULL) * num;

		contentparam.Reset();
		length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_item_expire_notice_subject);
		length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_item_expire_notice_content, RAND_ACTIVITY_TYPE_GOD_TOKEN, item_base->GetItemId(), num);

		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = sell_price;

		if (length1 > 0 && length2 > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
}

void RoleActivityGodToken::OnRAGodTokenBuyExp(int count)
{
	if (0 >= count) return;

	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAGodTokenOtherCfg & cfg = config->GetOtherCfg();

	Int64 consume_gold = (Int64)count * cfg.buy_exp;

	//消耗元宝
	if (m_role_module_mgr->GetMoney()->GetGold() < consume_gold)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}
	if (!m_role_module_mgr->GetMoney()->UseGold(consume_gold, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}

	int add_exp = RA_GOD_TOKEN_EXCHANGE_BUY_EXP_ADD * count;
	this->OnRAGodTokenAddExp(add_exp);

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRAGodTokenBuyExp user[%d, %s], level[%d], act_type[%d], buy_count[%d], consume_gold[%lld], add_exp[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_GOD_TOKEN,
		count, consume_gold, add_exp);

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityGodToken::OnRAGodTokenExchange(int seq)
{
	if (0 > seq) return;

	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAGodTokenExchangeCfg * cfg = config->GetExchangeCfg(m_param.ra_god_token_time_seq, seq);
	if (NULL == cfg) return;

	if (cfg->open_level > m_param.time_level || cfg->end_level < m_param.time_level)
	{
		return;
	}

	if (cfg->open_token > m_param.ra_god_token_level)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_GOD_TOKEN_LEVEL_NOT_ENOUGH);
		return;
	}

	if (0 != cfg->daily_exchange)
	{
		if (m_param.ra_god_token_exchange_count[seq] >= cfg->daily_exchange)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_EXCHANGE_TIMES_ERR);
			return;
		}
	}
	else
	{
		if (m_param.ra_god_token_exchange_count[seq] >= cfg->all_exchange)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_EXCHANGE_TIMES_ERR);
			return;
		}
	}

	if (!m_role_module_mgr->GetKnapsack()->HasItem(cfg->consume_item.item_id, cfg->consume_item.num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_EXCHANGE_ITEM_ERR);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->item.item_id, cfg->item.num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->consume_item.item_id, cfg->consume_item.num, __FUNCTION__))
	{
		gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRAGodTokenExchange err[uid:%d,name:%s] consumeitem2[item_id:%d,num:%d,is_bind:%d]",
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), cfg->consume_item.item_id, cfg->consume_item.num, cfg->consume_item.is_bind ? 1 : 0);
		return;
	}

	//给予
	if (!m_role_module_mgr->GetKnapsack()->Put(cfg->item, PUT_REASON_RA_GOD_TOKEN))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_param.ra_god_token_exchange_count[seq]++;

	this->SendRAGodTokenInfo();
	this->SetRoleDataChange();

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRAGodTokenExchange user[%d, %s], level[%d], act_type[%d], exchange_seq[%d], consume_item[%d %d %d], exchange_item[%d %d %d], exchange_count[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_GOD_TOKEN, seq,
		cfg->consume_item.item_id, cfg->consume_item.num, cfg->consume_item.is_bind ? 1 : 0, cfg->item.item_id, cfg->item.num, cfg->item.is_bind ? 1 : 0, m_param.ra_god_token_exchange_count[seq]);

	this->OnParticipateRA(__FUNCTION__);
}

bool RoleActivityGodToken::OnRAGodTokenAddExp(int add_exp)
{
	if (0 >= add_exp) return false;

	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return false;
	}

	const RAGodTokenOtherCfg& other_cfg = config->GetOtherCfg();

	//---------战令等级 高于配置等级 转化为溢出奖励
	if (m_param.ra_god_token_level >= other_cfg.max_leve)
	{
		const RAGodTokenSurplusCfg* cfg = config->GetSurplusCfg();
		if (NULL == cfg) return false;

		m_param.surplus_exp += add_exp;

		if (m_param.surplus_exp >= cfg->open_exp)
		{
			m_param.surplus_receive_count++;
			m_param.surplus_exp -= cfg->open_exp;
		}

		this->SendRAGodTokenInfo();
		this->SetRoleDataChange();

		gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRAGodTokenAddExp[surplus_exp] user[%d, %s], level[%d], act_type[%d], add_exp[%d], receive_count[%d], exp[%d]",
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_GOD_TOKEN,
			add_exp, m_param.surplus_receive_count, m_param.surplus_exp);
		return true;
	}

	//--------小于配置最大等级
	m_param.ra_god_token_exp += add_exp;
	while (1)
	{
		const RAGodTokenLevelCfg * cfg = config->GetLevelCfg(m_param.ra_god_token_time_seq, m_param.ra_god_token_level, m_param.time_level);
		if (NULL == cfg) break;

		if (m_param.ra_god_token_exp >= cfg->need_exp)
		{
			m_param.ra_god_token_exp -= cfg->need_exp;
			m_param.ra_god_token_level++;
		}
		else break;

		if (m_param.ra_god_token_level >= other_cfg.max_leve)
		{
			this->OnRAGodTokenAddExp(m_param.ra_god_token_exp);
			m_param.ra_god_token_exp = 0;
			break;
		}
	}
	this->SendRAGodTokenInfo();
	this->SetRoleDataChange();

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRAGodTokenAddExp user[%d, %s], level[%d], act_type[%d], add_exp[%d], token_level[%d], exp[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_GOD_TOKEN,
		add_exp, m_param.ra_god_token_level, m_param.ra_god_token_exp);
	return true;

}

void RoleActivityGodToken::OnRAGodTokenFetchSurplusReward()
{
	if (0 >= m_param.surplus_receive_count)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_MAX_RECEIVE_NUM);
		return;
	}

	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAGodTokenSurplusCfg * cfg = config->GetSurplusCfg();
	if (NULL == cfg) return;

	std::vector<ItemConfigData> reward_list;
	reward_list.assign(cfg->common_item.begin(), cfg->common_item.end());
	if (1 == m_param.ra_god_token_is_active_token)
	{
		reward_list.insert(reward_list.end(), cfg->senior_item.begin(), cfg->senior_item.end());
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)reward_list.size(), &reward_list[0]))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
		return;
	}

	//给予
	if (!m_role_module_mgr->GetKnapsack()->PutList((short)reward_list.size(), &reward_list[0], PUT_REASON_RA_GOD_TOKEN))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (1 != m_param.ra_god_token_is_active_token)
	{
		m_param.surplus_senior_count++;
	}
	m_param.surplus_receive_count--;
	this->SendRAGodTokenInfo();
	this->SetRoleDataChange();

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRAGodTokenFetchSurplusReward user[%d, %s], level[%d], act_type[%d], can_receive_count[%d] senior_receive_count[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_GOD_TOKEN,
		m_param.surplus_receive_count, m_param.surplus_senior_count);

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityGodToken::OnRAGodTokenPatchSurplusSeniorReward()
{
	if (0 >= m_param.surplus_senior_count) return;

	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAGodTokenSurplusCfg* cfg = config->GetSurplusCfg();
	if (NULL == cfg) return;

	static MailContentParam contentparam;
	contentparam.Reset();

	int length_1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_god_token_surplus_title);
	int length_2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_god_token_surplus_reward_content);

	for (int i = 0; i < (int)cfg->senior_item.size(); ++i)
	{
		contentparam.item_list[i] = ItemDataWrapper(cfg->senior_item[i].item_id, cfg->senior_item[i].num * m_param.surplus_senior_count, cfg->senior_item[i].is_bind);
	}

	if (length_1 > 0 && length_2 > 0)
	{
		MailRoute::MailToUser(m_role_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
	}

	m_param.surplus_senior_count = 0;
}

void RoleActivityGodToken::OnRAGodTokenFetchDailyReward(int seq)
{
	if (0 > seq || seq >= RA_GOD_TOKEN_DAILY_TASK_NUM)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	int type = m_param.ra_god_token_daily_task_type[seq];
	if (!this->IsRAGodTokenTaskType(RA_GOD_TOKEN_TASK_TYPE_DAILY, type))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_param.ra_god_token_daily_reward_flag.IsBitSet(type))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL == config)
	{
		return;
	}

	const RAGodTokenTaskCfg * cfg = config->GetTaskCfg(m_param.ra_god_token_time_seq, m_param.ra_god_token_daily_task_level, type);
	if (NULL == cfg) return;


	if (m_param.ra_god_token_daily_type_count[type] >= cfg->param)
	{
		if (this->OnRAGodTokenAddExp(cfg->get_exp))
		{
			m_param.ra_god_token_daily_reward_flag.SetBit(type);
		}
	}

	this->SendRAGodTokenInfo();
	this->SetRoleDataChange();

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRAGodTokenFetchDailyReward user[%d, %s], level[%d], act_type[%d], seq[%d], dialy_task_type[%d], finish_count[%d], add_vip_exp[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_GOD_TOKEN,
		seq, type, m_param.ra_god_token_daily_type_count[type], cfg->get_exp);

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityGodToken::OnRAGodTokenFetchWeeklyReward(int seq)
{
	if (0 > seq || seq >= RA_GOD_TOKEN_WEEKLY_TASK_NUM)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	int type = m_param.ra_god_token_weekly_task_type[seq];
	if (!this->IsRAGodTokenTaskType(RA_GOD_TOKEN_TASK_TYPE_WEEKLY, type))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_param.ra_god_token_week_reward_flag.IsBitSet(type))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL == config)
	{
		return;
	}

	const RAGodTokenTaskCfg * cfg = config->GetWeekTaskCfg(m_param.ra_god_token_time_seq, m_param.ra_god_token_weekly_task_level, type);
	if (NULL == cfg) return;

	if (m_param.ra_god_token_weekly_type_count[type] >= cfg->param)
	{
		if (this->OnRAGodTokenAddExp(cfg->get_exp))
		{
			m_param.ra_god_token_week_reward_flag.SetBit(type);
		}
	}

	this->SendRAGodTokenInfo();
	this->SetRoleDataChange();

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRAGodTokenFetchWeeklyReward user[%d, %s], level[%d], act_type[%d], seq[%d], weekly_task_type[%d], finish_count[%d], add_vip_exp[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_GOD_TOKEN,
		seq, type, m_param.ra_god_token_weekly_type_count[type], cfg->get_exp);

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityGodToken::CheckDailyReward(int type)
{
	int seq = -1;
	for (int i = 0; i < RA_GOD_TOKEN_DAILY_TASK_NUM; ++i)
	{
		if (type == m_param.ra_god_token_daily_task_type[i])
		{
			seq = i;
			break;
		}
	}

	if (0 > seq) return;

	if (m_param.ra_god_token_daily_reward_flag.IsBitSet(type))
	{
		return;
	}

	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL == config)
	{
		return;
	}

	const RAGodTokenTaskCfg * cfg = config->GetTaskCfg(m_param.ra_god_token_time_seq, m_param.ra_god_token_daily_task_level, type);
	if (NULL == cfg) return;

	if (m_param.ra_god_token_daily_type_count[type] < cfg->param) return;

	if (!this->OnRAGodTokenAddExp(cfg->get_exp)) return;

	m_param.ra_god_token_daily_reward_flag.SetBit(type);

	this->SetRoleDataChange();
	//this->SendRAGodTokenInfo();

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRAGodTokenFetchDailyReward user[%d, %s], level[%d], act_type[%d], seq[%d], dialy_task_type[%d], finish_count[%d], add_vip_exp[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_GOD_TOKEN,
		seq, type, m_param.ra_god_token_daily_type_count[type], cfg->get_exp);
}

void RoleActivityGodToken::CheckWeekReward(int type)
{
	int seq = -1;
	for (int i = 0; i < RA_GOD_TOKEN_WEEKLY_TASK_NUM; ++i)
	{
		if (type == m_param.ra_god_token_weekly_task_type[i])
		{
			seq = i;
			break;
		}
	}

	if (0 > seq) return;

	if (m_param.ra_god_token_week_reward_flag.IsBitSet(type))
	{
		return;
	}

	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL == config)
	{
		return;
	}

	const RAGodTokenTaskCfg * cfg = config->GetWeekTaskCfg(m_param.ra_god_token_time_seq, m_param.ra_god_token_weekly_task_level, type);
	if (NULL == cfg) return;

	if (m_param.ra_god_token_weekly_type_count[type] < cfg->param) return;

	if (!this->OnRAGodTokenAddExp(cfg->get_exp)) return;

	m_param.ra_god_token_week_reward_flag.SetBit(type);

	this->SetRoleDataChange();
	this->SendRAGodTokenInfo();

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivity::OnRAGodTokenFetchWeeklyReward user[%d, %s], level[%d], act_type[%d], seq[%d], weekly_task_type[%d], finish_count[%d], add_vip_exp[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_GOD_TOKEN,
		seq, type, m_param.ra_god_token_weekly_type_count[type], cfg->get_exp);
}

void RoleActivityGodToken::AddWeeklyTask()
{
	int count = 0;
	for (int i = 0; i < RA_GOD_TOKEN_WEEKLY_TASK_NUM; ++i)
	{
		if (RA_GOD_TOKEN_TYPE_BEGIN >= m_param.ra_god_token_weekly_task_type[i] || RA_GOD_TOKEN_WEEK_TYPE_MAX <= m_param.ra_god_token_weekly_task_type[i])
		{
			count++;
		}
	}
	if (0 >= count) return;

	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL == config)
	{
		return;
	}

	char week_type_list[RA_GOD_TOKEN_WEEKLY_TASK_NUM];
	memset(week_type_list, 0, sizeof(week_type_list));
	config->RandWeeklyTask(m_param.ra_god_token_time_seq, m_param.ra_god_token_weekly_task_level, week_type_list);
	
	std::vector<int> type_list;
	for (int i = 0; i < RA_GOD_TOKEN_WEEKLY_TASK_NUM; ++i)
	{
		bool has_mark = false;
		for (int idx = 0; idx < RA_GOD_TOKEN_WEEKLY_TASK_NUM; ++idx)
		{
			if (week_type_list[i] == m_param.ra_god_token_weekly_task_type[idx])
			{
				has_mark = true;
				break;
			}
		}
		if(!has_mark) type_list.push_back(week_type_list[i]);
	}

	for (std::vector<int>::const_iterator it = type_list.begin(); it != type_list.end(); ++it)
	{
		for (int i = 0; i < RA_GOD_TOKEN_WEEKLY_TASK_NUM; ++i)
		{
			if (RA_GOD_TOKEN_TYPE_BEGIN >= m_param.ra_god_token_weekly_task_type[i] || RA_GOD_TOKEN_WEEK_TYPE_MAX <= m_param.ra_god_token_weekly_task_type[i])
			{
				m_param.ra_god_token_weekly_task_type[i] = (char)(*it);
				break;
			}
		}
	}

	this->SendRAGodTokenInfo();
}

void RoleActivityGodToken::ChangeWeelyTask()
{
	const RandActivityGodTokenConfig * config = static_cast<const RandActivityGodTokenConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL == config)
	{
		return;
	}

	char week_type_list[RA_GOD_TOKEN_WEEKLY_TASK_NUM];
	memset(week_type_list, 0, sizeof(week_type_list));
	config->RandWeeklyTask(m_param.ra_god_token_time_seq, m_param.ra_god_token_weekly_task_level, week_type_list);

	//让4个新加的任务先置0
	for (int i = 6; i < RA_GOD_TOKEN_WEEKLY_TASK_NUM; ++i)
	{
		m_param.ra_god_token_weekly_task_type[i] = 0;
	}

	std::vector<int> type_list;
	for (int i = 0; i < RA_GOD_TOKEN_WEEKLY_TASK_NUM; ++i)
	{
		bool has_mark = false;
		for (int idx = 0; idx < RA_GOD_TOKEN_WEEKLY_TASK_NUM; ++idx)
		{
			if (week_type_list[i] == m_param.ra_god_token_weekly_task_type[idx])
			{
				has_mark = true;
				break;
			}
		}
		if (!has_mark) type_list.push_back(week_type_list[i]);
	}

	for (std::vector<int>::const_iterator it = type_list.begin(); it != type_list.end(); ++it)
	{
		for (int i = 0; i < RA_GOD_TOKEN_WEEKLY_TASK_NUM; ++i)
		{
			if (RA_GOD_TOKEN_TYPE_BEGIN >= m_param.ra_god_token_weekly_task_type[i] || RA_GOD_TOKEN_WEEK_TYPE_MAX <= m_param.ra_god_token_weekly_task_type[i])
			{
				m_param.ra_god_token_weekly_task_type[i] = (char)(*it);
				break;
			}
		}
	}

	this->SendRAGodTokenInfo();
}
