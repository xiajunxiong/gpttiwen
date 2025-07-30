#include "roleactivityhunqifanli.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityhunqifanliconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "engineadapter.h"
#include "protocol/randactivity/msgrahunqifanli.hpp"

RoleActivityHunQiFanLi::RoleActivityHunQiFanLi(int type) :RoleActivity(type)
{
}

RoleActivityHunQiFanLi::~RoleActivityHunQiFanLi()
{
}

void RoleActivityHunQiFanLi::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAHunQiFanLiParam));
}

void RoleActivityHunQiFanLi::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAHunQiFanLiParam));
}

void RoleActivityHunQiFanLi::OnLoginSendInfo()
{
	this->RASendInfo();
}

void RoleActivityHunQiFanLi::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.DayReset();
	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityHunQiFanLi::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
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

		this->RASendInfo();
		this->SetRoleDataChange();
	}
}

void RoleActivityHunQiFanLi::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	if (NULL == raor) return;
	
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}
	this->CheckInitRandActivity(m_role_activity_type);

	switch (raor->opera_type)
	{
	case Protocol::RA_HUN_QI_FAN_LI_REQ_TYPE_INFO:
		{
			this->RASendInfo();
		}
		break;
	case Protocol::RA_HUN_QI_FAN_LI_REQ_TYPE_LOTTERY:
		{
			this->Lottery();
		}
		break;
	case Protocol::RA_HUN_QI_FAN_LI_REQ_TYPE_LOTTERY_COUNT_REWARD:
		{
			this->ReceiveLotteryCountReward(raor->param_1);
		}
		break;
	}
}

// 一步一日志, 好谨慎的代码ovo
void RoleActivityHunQiFanLi::OnAddChongZhi(long long chongzhi)
{	
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		gamelog::g_log_role_activity.printf(LL_INFO, "%s error[NULL == ramgr] user[%d, %s], level[%d], activity_type[%d]  ", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type);
		return;
	}

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		gamelog::g_log_role_activity.printf(LL_INFO, "%s error[activity not open] user[%d, %s], level[%d], activity_type[%d]  ", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type);
		return;
	}

	const RandActivityHunQiFanLiConfig * config = static_cast<const RandActivityHunQiFanLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		gamelog::g_log_role_activity.printf(LL_INFO, "%s error[NULL == config] user[%d, %s], level[%d], activity_type[%d]  ", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type);
		return;
	}

	const std::vector<std::pair<int, int> >* chongzhi_cfg = config->GetChongZhiCfg(ramgr);
	if (NULL == chongzhi_cfg)
	{
		gamelog::g_log_role_activity.printf(LL_INFO, "%s error[NULL == chongzhi_cfg] user[%d, %s], level[%d], activity_type[%d]  ", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type);
		return;
	}

	m_param.m_today_recharge += chongzhi;

	for (size_t i = 0; i < chongzhi_cfg->size(); i++)
	{
		if(m_param.m_recharge_reward_flag.IsBitSet(i)) continue;
		if ((*chongzhi_cfg)[i].first > m_param.m_today_recharge) continue;

		m_param.m_last_lottery_count += (*chongzhi_cfg)[i].second;
		m_param.m_recharge_reward_flag.SetBit(i);
	}

	this->RASendInfo();
	this->SetRoleDataChange();
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s] chongzhi[%lld] m_today_recharge[%d] m_last_lottery_count[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), chongzhi, m_param.m_today_recharge, m_param.m_last_lottery_count);
}

void RoleActivityHunQiFanLi::Lottery()
{
	if (m_param.m_last_lottery_count <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_ENOUGH);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityHunQiFanLiConfig * config = static_cast<const RandActivityHunQiFanLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	int turntable_length = 0;
	BitMap<MAX_RA_HUN_QI_FAN_LI_BITMIP> temp_rotary_table_state = m_param.m_cur_rotary_table_state;
	const ItemConfigData* reward_cfg = config->RandRewardCfg(ramgr, temp_rotary_table_state, &turntable_length);
	if (NULL == reward_cfg) return;

	if (m_role_module_mgr->GetKnapsack()->CheckForPut2(*reward_cfg))
	{
		m_role_module_mgr->GetKnapsack()->Put(*reward_cfg, PUT_REASON_RA_HUN_QI_FAN_LI);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(reward_cfg, 1, SEND_MAIL_TYPE_ACTIVITY_REWARD_BAG_FULL, false, RAND_ACTIVITY_TYPE_HUN_QI_FAN_LI);
	}

	m_param.m_cur_rotary_table_state = temp_rotary_table_state;
	m_param.m_last_lottery_count--;
	m_param.m_total_lottery_count++;

	{	// 判断是否需要刷新轮盘
		bool is_can_refresh = true;
		for (int i = 0; i < turntable_length; i++)
		{
			if (!m_param.m_cur_rotary_table_state.IsBitSet(i))
			{
				is_can_refresh = false;
				break;
			}
		}

		if (is_can_refresh)
		{
			m_param.m_cur_rotary_table_state.Reset();
		}
	}

	this->RASendInfo();
	this->SetRoleDataChange();
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s] m_last_lottery_count[%d] m_total_lottery_count[%d] reward[%d, %d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_param.m_last_lottery_count, m_param.m_total_lottery_count, reward_cfg->item_id, reward_cfg->num);
}

void RoleActivityHunQiFanLi::ReceiveLotteryCountReward(int seq)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityHunQiFanLiConfig * config = static_cast<const RandActivityHunQiFanLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	const std::pair<int, ItemConfigData>* phase_reward_cfg = config->GetPhaseRewardCfg(ramgr, seq);
	if (NULL == phase_reward_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	// 判断当前奖励是否已领取
	if (m_param.m_lottery_reward_flag.IsBitSet(seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	{// 判断前置奖励是否领取了
		const std::pair<int, ItemConfigData>* front_phase_reward_cfg = config->GetPhaseRewardCfg(ramgr, seq - 1);
		if (NULL != front_phase_reward_cfg)
		{
			if (!m_param.m_lottery_reward_flag.IsBitSet(seq - 1))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return;
			}
		}
	}

	if (m_param.m_total_lottery_count < phase_reward_cfg->first)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_HUN_QI_FAN_LI_LOTTERY_COUNT_NOT_ENOUGH);
		return;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPut2(phase_reward_cfg->second))
	{
		m_role_module_mgr->GetKnapsack()->Put(phase_reward_cfg->second, PUT_REASON_RA_HUN_QI_FAN_LI);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&phase_reward_cfg->second, 1, SEND_MAIL_TYPE_ACTIVITY_REWARD_BAG_FULL, false, RAND_ACTIVITY_TYPE_HUN_QI_FAN_LI);
	}

	m_param.m_lottery_reward_flag.SetBit(seq);

	this->RASendInfo();
	this->SetRoleDataChange();
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s] seq[%d] reward[%d, %d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), seq, phase_reward_cfg->second.item_id, phase_reward_cfg->second.num);
}

void RoleActivityHunQiFanLi::RASendInfo()
{
	static Protocol::SCRAHunQiFanLiInfo msg;
	msg.m_last_lottery_count = m_param.m_last_lottery_count;
	msg.m_total_lottery_count = m_param.m_total_lottery_count;
	msg.m_today_recharge = m_param.m_today_recharge;
	msg.m_recharge_reward_flag = m_param.m_recharge_reward_flag;
	msg.m_lottery_reward_flag = m_param.m_lottery_reward_flag;
	msg.m_cur_rotary_table_state = m_param.m_cur_rotary_table_state;

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}