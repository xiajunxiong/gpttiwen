#include "roleactivityhappylook.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityhappylookconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"

#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivitywanlinggongying.hpp"
#include "other/roleactivity/impl/roleactivityshanhaimet.hpp"
#include "roleactivityshanhaimeet.hpp"

RoleActivityHappyLook::RoleActivityHappyLook(int type) :RoleActivity(type)
{
}

RoleActivityHappyLook::~RoleActivityHappyLook()
{
}

void RoleActivityHappyLook::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAHappyLookParam));
}

void RoleActivityHappyLook::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAHappyLookParam));
}

void RoleActivityHappyLook::Update(time_t now_second)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_HAPPY_LOOK))
		return;

	if (m_param.challenge_end_timestamp > 0)
	{
		if (now_second < m_param.challenge_end_timestamp)
			return;

		// 挑战失败
		m_param.challenge_is_succ = 0;
		m_param.challenge_end_timestamp = 0;
		
		this->RASendInfo();
		this->SetRoleDataChange();
		// log
		gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], is_succ[%d], challenge_num[%d], fail_timestamp[%d]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_HAPPY_LOOK,
			0, m_param.ra_frequency_num, (unsigned int)EngineAdapter::Instance().Time());
	}
}

void RoleActivityHappyLook::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_HAPPY_LOOK))
		return;

	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_HAPPY_LOOK);

	m_param.ra_frequency_num = 0;

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityHappyLook::OnLogin()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_HAPPY_LOOK))
		return;

	if (m_param.challenge_index != -1 && m_param.challenge_end_timestamp != 0)
	{
		m_param.challenge_is_succ = 0;
		m_param.challenge_end_timestamp = 0;

		this->RASendInfo();
		this->SetRoleDataChange();
		// log
		gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], is_succ[%d], challenge_num[%d], fail_timestamp[%d]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_HAPPY_LOOK,
			0, m_param.ra_frequency_num, (unsigned int)EngineAdapter::Instance().Time());
	}
}

void RoleActivityHappyLook::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_HAPPY_LOOK))
		return;

	this->RASendInfo();
}

void RoleActivityHappyLook::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_HAPPY_LOOK != rand_act_type)
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

		this->SetRoleDataChange();
	}
}

void RoleActivityHappyLook::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_HAPPY_LOOK))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_HAPPY_LOOK);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();
		}
		break;
	case 1:
		{
			this->OnStartGame(raor->param_1);
		}
		break;
	case 2:
		{
			this->OnEndGame(raor->param_1);
		}
		break;
	case 3:
		{
			this->OnFetchReward();
		}
		break;
	}
}

void RoleActivityHappyLook::RASendInfo()
{
	Protocol::SCRAHappyLookInfo info;
	info.ra_frequency_num = m_param.ra_frequency_num;
	info.challenge_index = m_param.challenge_index;
	info.challenge_is_succ = m_param.challenge_is_succ;
	info.challenge_end_timestamp = m_param.challenge_end_timestamp;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityHappyLook::OnStartGame(int index)
{
	if (m_param.challenge_end_timestamp != 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_SHAN_HAI_BOSS_NOT_CHALLENGE_COUNT);
		return;
	}

	if (m_param.challenge_is_succ >= 0)
	{
		if (m_param.challenge_is_succ == 0)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RA_HAPPY_LOOK_NOT_FETCH_FAIL);
			return;
		}
		else
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RA_HAPPY_LOOK_NOT_FETCH_SUCC);
			return;
		}
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityHappyLookConfig * config = static_cast<const RandActivityHappyLookConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_HAPPY_LOOK));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (m_param.ra_frequency_num >= config->GetOtherCfg().frequency)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_TODAY_JOIN_COUNT_LIMIT);
		return;
	}

	const RAHappyLookRewardCfg * cfg = config->GetRewardCfg(ramgr, index);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	m_param.ra_frequency_num += 1;
	m_param.challenge_index = index;
	m_param.challenge_end_timestamp = (unsigned int)EngineAdapter::Instance().Time() + cfg->time;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	// log
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], challenge_num[%d], begin_timestamp[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_HAPPY_LOOK,
		m_param.ra_frequency_num, (unsigned int)EngineAdapter::Instance().Time());

	{
		RoleActivityWanLingGongYing * role_activity = static_cast<RoleActivityWanLingGongYing *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING));
		if (NULL != role_activity)
		{
			role_activity->OnCompleteTask(RAND_ACTIVITY_TYPE_HAPPY_LOOK);
		}

		//活动-山海相逢
		RoleActivityShanHaiMeet * shan_hai_meet = static_cast<RoleActivityShanHaiMeet *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MEET));
		if (NULL != shan_hai_meet)
		{
			shan_hai_meet->OnFinishType(RA_SHAN_HAI_MEET_ACT_TYPE_1, 1);
		}
	}
	{
		//山海初遇
		RoleActivityShanHaiMet * role_activity = static_cast<RoleActivityShanHaiMet *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MET));
		if (NULL != role_activity)
		{
			role_activity->AddTaskParam(RAShanHaiMet::TSAK_TYPE_HAPPY_LOOK, 1);
		}
	}
}

void RoleActivityHappyLook::OnEndGame(int is_succ)
{
	if (m_param.challenge_end_timestamp == 0)
	{
		return;
	}

	m_param.challenge_is_succ = is_succ != 0 ? 1 : 0;
	m_param.challenge_end_timestamp = 0;

	this->RASendInfo();
	this->SetRoleDataChange();
	// log
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], is_succ[%d], challenge_num[%d], succ_timestamp[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_HAPPY_LOOK,
		is_succ != 0 ? 1 : 0, m_param.ra_frequency_num, (unsigned int)EngineAdapter::Instance().Time());
}

void RoleActivityHappyLook::OnFetchReward()
{
	if (m_param.challenge_index < 0 || m_param.challenge_is_succ < 0)
		return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityHappyLookConfig * config = static_cast<const RandActivityHappyLookConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_HAPPY_LOOK));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int item_count = 0;
	const ItemConfigData * item_list = NULL;
	if (m_param.challenge_is_succ == 0)
	{
		item_count = config->GetOtherCfg().reward_count;
		item_list = config->GetOtherCfg().reward_item;
	}
	else
	{
		const RAHappyLookRewardCfg * cfg = config->GetRewardCfg(ramgr, m_param.challenge_index);
		if (NULL == cfg)
			return;

		item_count = cfg->reward_count;
		item_list = cfg->reward_item;
	}

	if (item_count <= 0 || NULL == item_list)
		return;

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList(item_count, item_list, false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(item_list, item_count, SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_HAPPY_LOOK);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutList(item_count, item_list, PUT_REASON_RA_HAPPY_LOOK);
	}

	// log
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] fetch_type[%d] reward_count[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_HAPPY_LOOK,
		m_param.challenge_is_succ, item_count);

	m_param.ResetQuency();
	this->RASendInfo();
	this->SetRoleDataChange();
}
