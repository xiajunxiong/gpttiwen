#include "roleactivityshanhaizhengfeng.hpp"
#include "gamelog.h"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "item/knapsack.h"
#include "obj/character/role.h"
#include "battle/battle_manager_local.hpp"

#include "other/rolemodulemanager.hpp"
#include "other/roleattribute/roleexpconfig.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "protocol/randactivity/msgraburningsummer.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityshanhaizhengfengconfig.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "roleactivityshanhaimeet.hpp"

#include "other/roleactivity/impl/roleactivityshanhaimet.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"

RoleActivityShanHaiZhengFeng::RoleActivityShanHaiZhengFeng(int type)
	:RoleActivity(type)
{
}

RoleActivityShanHaiZhengFeng::~RoleActivityShanHaiZhengFeng()
{
}

void RoleActivityShanHaiZhengFeng::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RASummerLiLianParam));
}

void RoleActivityShanHaiZhengFeng::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RASummerLiLianParam));
}

void RoleActivityShanHaiZhengFeng::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG);
	this->SendAllInfo();
}

void RoleActivityShanHaiZhengFeng::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG != rand_act_type)
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
		m_param.open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG);
		gamelog::g_log_burning_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], open_day[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(),
			m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type, m_param.open_day);
		this->SetRoleDataChange();
	}
}

void RoleActivityShanHaiZhengFeng::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG);
	m_param.open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG);
	this->SendAllInfo();
	gamelog::g_log_burning_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], open_day[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), 
		m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type, m_param.open_day);
}

void RoleActivityShanHaiZhengFeng::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG);
	switch (raor->opera_type)
	{
		case Protocol::RA_SHAN_HAI_ZHENG_FENG_REQ_TYPE_CHALLENGE:
		{
			this->OnChallenge(raor->param_1);
		}
		break;
		case Protocol::RA_SHAN_HAI_ZHENG_FENG_REQ_TYPE_FETCH_ALL_PASS_REWARD:
		{
			this->OnFetchAllPassReward();
		}	
		break;
	default:
		break;
	}


}

void RoleActivityShanHaiZhengFeng::OnBattleFinish(int activity_time, bool is_win)
{
	if (activity_time <= 0 || activity_time > MAX_RA_SUMMER_LI_LIAN_BOSS_NUM) return;

	if (!is_win)
	{
		//this->OnSendResultNotice(Protocol::SCRAResultNotice::RAND_ACTIVITY_RESULT_NOTICE_TYPE_BATTLE_FAIL, NULL, NULL, activity_time);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG);

	const RandActivityShanHaiZhengFengConfig * config = static_cast<const RandActivityShanHaiZhengFengConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG));
	if (NULL == config) return;

	if (!this->IsLevelUnLock(activity_time)) 
	{
		return;
	}
	if (activity_time > m_param.open_day)
	{
		return;
	}

	const RAShanHaiZhengFengBaseCfg * base_cfg = config->GetBaseCfg(ramgr, activity_time);
	if (NULL == base_cfg)
	{
		return;
	}
	const RAShanHaiZhengFengRewardCfg * reward_cfg = config->GetRewardCfg(m_param.m_first_flag.IsBitSet(activity_time) ? base_cfg->common_reward_group_id : base_cfg->frist_reward_group_id);
	if (NULL == reward_cfg)
	{
		return;
	}
	const RoleLevelCfg* level_cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetLevelCfg(m_role_module_mgr->GetRole()->GetLevel());
	if (NULL == level_cfg) return;

	int add_exp = static_cast<int>((level_cfg->exp * reward_cfg->exp) / RAShanHaiZhengFengRewardCfg::EXP_BASE_NUM);
	if (add_exp > 0)
	{
		m_role_module_mgr->GetRole()->AddExp(add_exp, ADD_EXP_TYPE_SUMMER_LI_LIAN, __FUNCTION__);
	}
	std::vector<RAMoneyInfo> money_list;
	if (reward_cfg->coin > 0)
	{
		m_role_module_mgr->GetMoney()->AddCoinBind(reward_cfg->coin, __FUNCTION__);
		money_list.push_back(RAMoneyInfo(MONEY_TYPE_COIN_BIND, reward_cfg->coin));
	}
	if (!reward_cfg->reward_list.empty())
	{
		bool is_put_succ = false;
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)reward_cfg->reward_list.size(), &reward_cfg->reward_list[0]))
		{
			if (m_role_module_mgr->GetKnapsack()->PutList((short)reward_cfg->reward_list.size(), &reward_cfg->reward_list[0], PUT_REASON_SHAN_HAI_ZHENG_FENG_REWARD))
			{
				is_put_succ = true;
			}
		}
		if (!is_put_succ)
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->reward_list[0], (int)reward_cfg->reward_list.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG);
		}
	}
	this->OnSendResultNotice(Protocol::SCRAResultNotice::RAND_ACTIVITY_RESULT_NOTICE_TYPE_BATTLE_WIN, &reward_cfg->reward_list, &money_list, activity_time, 0, add_exp);

	m_param.m_first_flag.SetBit(activity_time);
	this->SendAllInfo();

	gamelog::g_log_burning_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], open_day[%d] activity_time[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(),
		m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type, m_param.open_day, activity_time);

	//活动-山海相逢
	RoleActivityShanHaiMeet * shan_hai_meet = static_cast<RoleActivityShanHaiMeet *>
		(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MEET));
	if (NULL != shan_hai_meet)
	{
		shan_hai_meet->OnFinishType(RA_SHAN_HAI_MEET_ACT_TYPE_2, 1);
	}

	//山海初遇
	RoleActivityShanHaiMet * role_activity = static_cast<RoleActivityShanHaiMet *>
		(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHAN_HAI_MET));
	if (NULL != role_activity)
	{
		role_activity->AddTaskParam(RAShanHaiMet::TSAK_TYPE_SUMMER_LI_LIAN, 1);
	}
}

void RoleActivityShanHaiZhengFeng::SendAllInfo()
{
	Protocol::SCRAShanHaiZhengFengAllInfo info;
	info.open_day = m_param.open_day;
	info.is_fetch_all_pass_flag = m_param.is_fetch_all_pass_reward_flag;
	info.reserve_ch = 0;
	info.m_first_flag = m_param.m_first_flag;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityShanHaiZhengFeng::OnChallenge(int activity_time)
{
	if(activity_time <= 0 || activity_time > MAX_RA_SUMMER_LI_LIAN_BOSS_NUM) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityShanHaiZhengFengConfig * config = static_cast<const RandActivityShanHaiZhengFengConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	if (!this->IsLevelUnLock(activity_time))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_SUMMER_LI_LIAN_LEVEL_LOCK);
		return;
	}
	if (activity_time > m_param.open_day)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_SUMMER_LI_LIAN_LEVEL_TODAY_LOCK);
		return;
	}

	const RAShanHaiZhengFengBaseCfg * base_cfg = config->GetBaseCfg(ramgr, activity_time);
	if (NULL == base_cfg)
	{
		return;
	}

	const RAShanHaiZhengFengBossGroupCfg * boss_cfg = config->GetBossGroupCfg(base_cfg->boss_group_id, m_role_module_mgr->GetRole()->GetLevel());
	if (NULL == boss_cfg)
	{
		return;
	}

	int monster_index = m_param.open_day - activity_time;
	if (monster_index >= ARRAY_LENGTH(boss_cfg->monster_gourp_list))
	{
		monster_index = ARRAY_LENGTH(boss_cfg->monster_gourp_list) - 1;
	}

	BattleManagerLocal::GetInstance().StartBattleModeFightReq(m_role_module_mgr->GetRole(), BATTLE_MODE_SHAN_HAI_ZHENG_FENG, boss_cfg->monster_gourp_list[monster_index], 0, activity_time, true);

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityShanHaiZhengFeng::OnFetchAllPassReward()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityShanHaiZhengFengConfig * config = static_cast<const RandActivityShanHaiZhengFengConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	if (0 != m_param.is_fetch_all_pass_reward_flag)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_TEAM_FIGHT_BOX_ALREADY_FETCH);
		return;
	}

	const std::map<int, RAShanHaiZhengFengBaseCfg> * base_map = config->GetBaseMapCfg(ramgr);
	if (NULL == base_map)
	{
		return;
	}

	for (std::map<int, RAShanHaiZhengFengBaseCfg>::const_iterator it = base_map->begin(); it != base_map->end(); it++)
	{
		if (it->first <= 0 || it->first > MAX_RA_SUMMER_LI_LIAN_BOSS_NUM) return;

		if (!m_param.m_first_flag.IsBitSet(it->first))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GIVE_SECRET_KEY_PARAM_NOT_ADV);
			return;
		}
	}

	const RAShanHaiZhengFengOtherCfg & other_cfg = config->GetOtherCfg();
	const RAShanHaiZhengFengRewardCfg * reward_cfg = config->GetRewardCfg(other_cfg.all_pass_reward_group_id);
	if (NULL == reward_cfg)
	{
		return;
	}

	if (!reward_cfg->reward_list.empty())
	{
		bool is_put_succ = false;
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)reward_cfg->reward_list.size(), &reward_cfg->reward_list[0]))
		{
			if (m_role_module_mgr->GetKnapsack()->PutList((short)reward_cfg->reward_list.size(), &reward_cfg->reward_list[0], PUT_REASON_SHAN_HAI_ZHENG_FENG_REWARD))
			{
				is_put_succ = true;
			}
		}
		if (!is_put_succ)
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->reward_list[0], (int)reward_cfg->reward_list.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG);
		}
		this->OnSendResultNotice(Protocol::SCRAResultNotice::RAND_ACTIVITY_RESULT_NOTICE_TYPE_GET_REWARD, &reward_cfg->reward_list);
	}

	m_param.is_fetch_all_pass_reward_flag = 1;
	this->SendAllInfo();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_burning_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], open_day[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(),
		m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type, m_param.open_day);
}

bool RoleActivityShanHaiZhengFeng::IsLevelUnLock(int activity_time)
{
	if(activity_time <= 0 || activity_time > MAX_RA_SUMMER_LI_LIAN_BOSS_NUM) return false;

	if (activity_time > 1)
	{
		if (!m_param.m_first_flag.IsBitSet(activity_time - 1))		//判断是否通关上一个首领
		{
			return false;
		}
	}
	
	return true;
}
