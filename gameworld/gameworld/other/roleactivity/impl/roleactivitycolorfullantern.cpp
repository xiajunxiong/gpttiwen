#include "roleactivitycolorfullantern.hpp"
#include "gamelog.h"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "item/knapsack.h"
#include "obj/character/role.h"

#include "other/rolemodulemanager.hpp"

#include "protocol/randactivity/msgraburningsummer.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitycolorfullanternconfig.hpp"

#include "other/roleactivity/impl/roleactivityshanhaimet.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityshanhaimeet.hpp"

RoleActivityColorfulLantern::RoleActivityColorfulLantern(int type)
	:RoleActivity(type), next_refresh_light_timestamp(0)
{
	memset(m_light_list, 0, sizeof(m_light_list));
}

RoleActivityColorfulLantern::~RoleActivityColorfulLantern()
{
}

void RoleActivityColorfulLantern::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAColorfulLanternParam));
}

void RoleActivityColorfulLantern::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAColorfulLanternParam));
}

void RoleActivityColorfulLantern::Update(time_t now_second)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_COLORFUL_LANTERN))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_COLORFUL_LANTERN);
	if (0 != m_param.game_end_timestamp && (unsigned int)now_second >= m_param.game_end_timestamp)
	{
		m_param.day_join_times++;
		m_param.activity_join_times++;
		m_param.day_score_count += m_param.single_score_count;
		m_param.activity_score_count += m_param.single_score_count;
		m_param.day_single_score_count = m_param.day_single_score_count >= m_param.single_score_count ? m_param.day_single_score_count : m_param.single_score_count;
		m_param.activity_single_score_count = m_param.activity_single_score_count >= m_param.single_score_count ? m_param.activity_single_score_count : m_param.single_score_count;
		m_param.last_game_score_count = m_param.single_score_count;
		this->SendGameInfo(Protocol::RA_COLORFUL_LANTERN_GAME_NOTICE_TYPE_GAME_OVER);
		this->SendAllInfo();
		m_param.game_end_timestamp = 0;
		m_param.single_score_count = 0;
		this->OnFinishGame();
		this->SetRoleDataChange();
		next_refresh_light_timestamp = 0;
		memset(m_light_list, 0, sizeof(m_light_list));
		gamelog::g_log_burning_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], join_times[%d, %d] single_score[%d, %d], score_count[%d, %d]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_COLORFUL_LANTERN,
			m_param.day_join_times, m_param.activity_join_times, m_param.day_single_score_count, m_param.activity_single_score_count, m_param.day_score_count, m_param.activity_score_count);
	}
	else if (0 != next_refresh_light_timestamp && now_second >= next_refresh_light_timestamp)
	{
		this->OnRefreshLight((unsigned int)now_second);
	}
}

void RoleActivityColorfulLantern::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_COLORFUL_LANTERN))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_COLORFUL_LANTERN);

	const RandActivityColorfulLanternConfig * config = static_cast<const RandActivityColorfulLanternConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COLORFUL_LANTERN));
	if (NULL == config) return;

	unsigned int now_second = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (0 != m_param.game_end_timestamp)
	{
		if (now_second <= m_param.game_end_timestamp && now_second >= m_param.game_end_timestamp - config->GetOtherCfg().game_time)
		{
			this->SendGameInfo(Protocol::RA_COLORFUL_LANTERN_GAME_NOTICE_TYPE_IN_THE_GAME);
			this->OnRefreshLight(now_second);
		}
		//当天,游戏结束,且处在游戏结束后15分钟内登陆
		else if (m_param.last_start_day_id == EngineAdapter::Instance().DayID() && now_second > m_param.game_end_timestamp)
		{
			m_param.day_join_times++;
			m_param.activity_join_times++;
			m_param.day_score_count += m_param.single_score_count;
			m_param.activity_score_count += m_param.single_score_count;
			m_param.day_single_score_count = m_param.day_single_score_count >= m_param.single_score_count ? m_param.day_single_score_count : m_param.single_score_count;
			m_param.activity_single_score_count = m_param.activity_single_score_count >= m_param.single_score_count ? m_param.activity_single_score_count : m_param.single_score_count;
			m_param.last_game_score_count = m_param.single_score_count;
			if (now_second <= m_param.game_end_timestamp + 15 * SECOND_PER_MIN)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_COLORFUL_LANTERN_OFFLINE_LOGIN);
			}
			m_param.single_score_count = 0;
			m_param.game_end_timestamp = 0;
			this->OnFinishGame();
			this->SetRoleDataChange();
			gamelog::g_log_burning_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], join_times[%d, %d] single_score[%d, %d], score_count[%d, %d]", __FUNCTION__,
				m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_COLORFUL_LANTERN,
				m_param.day_join_times, m_param.activity_join_times, m_param.day_single_score_count, m_param.activity_single_score_count, m_param.day_score_count, m_param.activity_score_count);
		}
		//在玩家离线时游戏已经早结束了, 前一天就已结束的游戏不能影响新的一天
		else if (m_param.last_start_day_id != EngineAdapter::Instance().DayID() && (unsigned int)now_second > m_param.game_end_timestamp)
		{
			m_param.activity_join_times++;
			m_param.activity_score_count += m_param.single_score_count;
			m_param.activity_single_score_count = m_param.activity_single_score_count >= m_param.single_score_count ? m_param.activity_single_score_count : m_param.single_score_count;
			m_param.last_game_score_count = m_param.single_score_count;
			m_param.game_end_timestamp = 0;
			m_param.single_score_count = 0;
			this->OnFinishGame();
			this->SetRoleDataChange();
			gamelog::g_log_burning_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], join_times[%d, %d] single_score[%d, %d], score_count[%d, %d]", __FUNCTION__,
				m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_COLORFUL_LANTERN,
				m_param.day_join_times, m_param.activity_join_times, m_param.day_single_score_count, m_param.activity_single_score_count, m_param.day_score_count, m_param.activity_score_count);
		}
	}

	this->SendAllInfo();
}

void RoleActivityColorfulLantern::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_COLORFUL_LANTERN != rand_act_type)
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
	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		next_refresh_light_timestamp = 0;
		memset(m_light_list, 0, sizeof(m_light_list));
		this->SetRoleDataChange();
	}
}

void RoleActivityColorfulLantern::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_COLORFUL_LANTERN))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_COLORFUL_LANTERN);
	const RandActivityColorfulLanternConfig * config = static_cast<const RandActivityColorfulLanternConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COLORFUL_LANTERN));
	if (NULL == config) return;

	std::map<int, RAColorfulLanternBaseCfg> task_list;
	config->GetTaskList(ramgr, task_list);
	if (!task_list.empty())
	{
		for (std::map<int, RAColorfulLanternBaseCfg>::iterator it = task_list.begin(); it != task_list.end(); it++)
		{
			if (COLORFUL_LANTERN_TASK_TYPE_DAY == it->second.task_type)
			{
				m_param.m_task_list_flag.UnSetBit(it->first - 1);
			}
		}
	}

	m_param.day_join_times = 0;
	m_param.day_score_count = 0;
	m_param.day_single_score_count = 0;
	this->SetRoleDataChange();
	this->SendAllInfo();
	gamelog::g_log_burning_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], join_times[%d, %d] single_score[%d, %d], score_count[%d, %d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_COLORFUL_LANTERN, 
		m_param.day_join_times, m_param.activity_join_times, m_param.day_single_score_count, m_param.activity_single_score_count, m_param.day_score_count, m_param.activity_score_count);
}

void RoleActivityColorfulLantern::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	if (NULL == raor)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_COLORFUL_LANTERN))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_COLORFUL_LANTERN);
	UNSTD_STATIC_CHECK(RA_COLORFUL_LANTERN_REQ_TYPE_MAX == 5);
	switch (raor->opera_type)
	{
		case RA_COLORFUL_LANTERN_REQ_TYPE_START_GAME:
		{
			this->OnStartGame();
		}
		break;
		case RA_COLORFUL_LANTERN_REQ_TYPE_CATCH_LANTERN:
		{
			this->OnCatchLantern(raor->param_1);
		}
		break;
		case RA_COLORFUL_LANTERN_REQ_TYPE_FETCH_REWARD:
		{
			this->OnFetchTaskReward(raor->param_1);
		}
		break;
		case RA_COLORFUL_LANTERN_REQ_TYPE_FORCE_CLOSE:
		{
			this->OnForceClose();
		}
		break;
		case RA_COLORFUL_LANTERN_REQ_TYPE_FLOW_AWAY:
		{
			this->OnFlowAway(raor->param_1);
		}
		break;
	}
}

void RoleActivityColorfulLantern::OnStartGame()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}
	const RandActivityColorfulLanternConfig * config = static_cast<const RandActivityColorfulLanternConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COLORFUL_LANTERN));
	if (NULL == config) return;

	unsigned int now_second = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (now_second <= m_param.game_end_timestamp && now_second >= m_param.game_end_timestamp - config->GetOtherCfg().game_time)
	{
		return;	//处于游戏状态不可重复开始
	}
	if (m_param.day_join_times >= 3)	//运营让写死次数
	{
		m_role_module_mgr->NoticeNum(errornum::EN_TODAY_JOIN_COUNT_LIMIT);
		return;
	}
	
	m_param.last_start_day_id = EngineAdapter::Instance().DayID();
	m_param.single_score_count = 0;
	m_param.game_end_timestamp = now_second + config->GetOtherCfg().game_time;
	this->OnRefreshLight(now_second);
	this->SendGameInfo(Protocol::RA_COLORFUL_LANTERN_GAME_NOTICE_TYPE_START_GAME);
	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityColorfulLantern::OnCatchLantern(int type)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}
	const RandActivityColorfulLanternConfig * config = static_cast<const RandActivityColorfulLanternConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COLORFUL_LANTERN));
	if (NULL == config) return;
	unsigned int now_second = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (now_second > m_param.game_end_timestamp || now_second < m_param.game_end_timestamp - config->GetOtherCfg().game_time)
	{
		return;
	}
	if (type < 0 || type >= MAX_RA_COLORFUL_LANTERN_LIGHT_TYPE_NUM)
	{
		return;
	}
	//判断花灯数量
	if (m_light_list[type] <= 0)
	{
		return;
	}

	//剔除花灯数量
	m_light_list[type] -= 1;
	m_param.single_score_count += config->GetLightScoreNum(type);
	this->SetRoleDataChange();
	this->SendGameInfo(Protocol::RA_COLORFUL_LANTERN_GAME_NOTICE_TYPE_IN_THE_GAME);
	this->SendLightInfo();
}

void RoleActivityColorfulLantern::OnFetchTaskReward(int task_id)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}
	const RandActivityColorfulLanternConfig * config = static_cast<const RandActivityColorfulLanternConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COLORFUL_LANTERN));
	if (NULL == config) return;

	const RAColorfulLanternBaseCfg * base_cfg = config->GetBaseCfg(ramgr, task_id);
	if (NULL == base_cfg)
	{
		return;
	}
	if (m_param.m_task_list_flag.IsBitSet(task_id - 1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}
	int param1 = 0;
	switch (base_cfg->task_param)
	{
		case COLORFUL_LANTERN_TASK_MODE_SINGLE_ENTER_SCORE:
		{
			param1 = (COLORFUL_LANTERN_TASK_TYPE_DAY == base_cfg->task_type) ? m_param.day_single_score_count : m_param.activity_single_score_count;
  		}
		break;
		case COLORFUL_LANTERN_TASK_MODE_ADD_UP_SCORE:
		{
			param1 = (COLORFUL_LANTERN_TASK_TYPE_DAY == base_cfg->task_type) ? m_param.day_score_count : m_param.activity_score_count;
		}
		break;
		case COLORFUL_LANTERN_TASK_MODE_ENTER_NUM:
		{
			param1 = (COLORFUL_LANTERN_TASK_TYPE_DAY == base_cfg->task_type) ? m_param.day_join_times : m_param.activity_join_times;
		}
		break;
	default:
		return;
	}
	if (param1 < base_cfg->param1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_NOT_ELIGIBLE);
		return;
	}
	bool is_put_succ = false;
	if (m_role_module_mgr->GetKnapsack()->CheckForPut(base_cfg->reward_item.item_id, base_cfg->reward_item.num, false))
	{
		if (m_role_module_mgr->GetKnapsack()->Put(base_cfg->reward_item, PUT_REASON_COLORFUL_LANTERN))
		{
			is_put_succ = true;
		}
	}
	if(!is_put_succ)
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&base_cfg->reward_item, 1, SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_COLORFUL_LANTERN);
	}
	m_param.m_task_list_flag.SetBit(task_id - 1);
	this->SetRoleDataChange();
	this->SendAllInfo();
	gamelog::g_log_burning_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], task_id[%d], join_times[%d, %d] single_score[%d, %d], score_count[%d, %d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_COLORFUL_LANTERN, task_id,
		m_param.day_join_times, m_param.activity_join_times, m_param.day_single_score_count, m_param.activity_single_score_count, m_param.day_score_count, m_param.activity_score_count);
}

void RoleActivityColorfulLantern::OnForceClose()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}
	const RandActivityColorfulLanternConfig * config = static_cast<const RandActivityColorfulLanternConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COLORFUL_LANTERN));
	if (NULL == config) return;

	unsigned int now_second = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (now_second > m_param.game_end_timestamp || now_second < m_param.game_end_timestamp - config->GetOtherCfg().game_time)
	{
		return;
	}

	m_param.game_end_timestamp = 0;
	m_param.day_join_times++;
	m_param.activity_join_times++;
	m_param.day_score_count += m_param.single_score_count;
	m_param.activity_score_count += m_param.single_score_count;
	m_param.day_single_score_count = m_param.day_single_score_count >= m_param.single_score_count ? m_param.day_single_score_count : m_param.single_score_count;
	m_param.activity_single_score_count = m_param.activity_single_score_count >= m_param.single_score_count ? m_param.activity_single_score_count : m_param.single_score_count;
	m_param.last_game_score_count = m_param.single_score_count;
	next_refresh_light_timestamp = 0;
	memset(m_light_list, 0, sizeof(m_light_list));
	this->SendGameInfo(Protocol::RA_COLORFUL_LANTERN_GAME_NOTICE_TYPE_GAME_OVER);
	this->SendAllInfo();
	m_param.single_score_count = 0;
	this->OnFinishGame();
	this->SetRoleDataChange();
	gamelog::g_log_burning_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], join_times[%d, %d] single_score[%d, %d], score_count[%d, %d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_COLORFUL_LANTERN, 
		m_param.day_join_times, m_param.activity_join_times, m_param.day_single_score_count, m_param.activity_single_score_count, m_param.day_score_count, m_param.activity_score_count);
}

void RoleActivityColorfulLantern::OnFlowAway(int type)
{
	if (type < 0 || type >= MAX_RA_COLORFUL_LANTERN_LIGHT_TYPE_NUM)
	{
		return;
	}
	if (m_light_list[type] <= 0)
	{
		return;
	}
	m_light_list[type] -= 1;
	this->SendLightInfo();
}

void RoleActivityColorfulLantern::OnRefreshLight(unsigned int now_second)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}
	const RandActivityColorfulLanternConfig * config = static_cast<const RandActivityColorfulLanternConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COLORFUL_LANTERN));
	if (NULL == config) return;

	std::vector<int> light_list;
	config->GetRefreshLightList(light_list);

	for (int i = 0; i < (int)light_list.size(); i++)
	{
		if(light_list[i] < 0 || light_list[i] >= MAX_RA_COLORFUL_LANTERN_LIGHT_TYPE_NUM) continue;

		m_light_list[light_list[i]] += 1;
	}
	next_refresh_light_timestamp = now_second + config->GetOtherCfg().refresh_time;
	this->SendLightInfo();
}

void RoleActivityColorfulLantern::OnFinishGame()
{
	return;
	//山海初遇
	RoleActivityShanHaiMet * role_activity = static_cast<RoleActivityShanHaiMet *>
		(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MET));
	if (NULL != role_activity)
	{
		role_activity->AddTaskParam(RAShanHaiMet::TSAK_TYPE_COLORFUL_LANTERN, 1);
	}

	//活动-山海相逢
	RoleActivityShanHaiMeet * shan_hai_meet = static_cast<RoleActivityShanHaiMeet *>
		(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MEET));
	if (NULL != shan_hai_meet)
	{
		shan_hai_meet->OnFinishType(RA_SHAN_HAI_MEET_ACT_TYPE_6, 1);
	}
}

void RoleActivityColorfulLantern::SendAllInfo()
{
	Protocol::SCRAColorfulLanternAllInfo info;
	info.last_game_score_count = m_param.last_game_score_count;
	info.day_single_score_count = m_param.day_single_score_count;
	info.activity_single_score_count = m_param.activity_single_score_count;
	info.day_join_times = m_param.day_join_times;
	info.activity_join_times = m_param.activity_join_times;
	info.day_score_count = m_param.day_score_count;
	info.activity_score_count = m_param.activity_score_count;
	info.m_task_list_flag = m_param.m_task_list_flag;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityColorfulLantern::SendGameInfo(int notice_type)
{
	Protocol::SCRAColorfulLanternGameInfo info;
	info.notice_type = notice_type;
	info.single_score_count = m_param.single_score_count;
	info.game_end_timestamp = m_param.game_end_timestamp;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityColorfulLantern::SendLightInfo()
{
	Protocol::SCRAColorfulLanternLightInfo info;
	for (int i = 0; i < MAX_RA_COLORFUL_LANTERN_LIGHT_TYPE_NUM; i++)
	{
		info.light_list[i] = m_light_list[i];
	}
	m_role_module_mgr->NetSend(&info, sizeof(info));
}
