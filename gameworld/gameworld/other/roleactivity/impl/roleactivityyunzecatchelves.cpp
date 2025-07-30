#include "roleactivityyunzecatchelves.hpp"
#include "gamelog.h"
#include "obj/character/role.h"

#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivitychaozhifenxiang.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityyunzecatchelvesconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

RoleActivityYunZeCatchElves::RoleActivityYunZeCatchElves(int type)
	:RoleActivity(type)
{

}

RoleActivityYunZeCatchElves::~RoleActivityYunZeCatchElves()
{

}

void RoleActivityYunZeCatchElves::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityYunZeCatchElves::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityYunZeCatchElves::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_YUN_ZE_CATCH_ELVES != rand_act_type)
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

void RoleActivityYunZeCatchElves::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_CATCH_ELVES))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_YUN_ZE_CATCH_ELVES);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();			//直接返回，无操作
		}
		break;
	case 1:
		{
			this->CatchElves();			// 抓捕
		}
		break;
	case 2:
		{
			this->BatchCatchElves();	// 一键抓捕
		}
		break;
	default:
		break;
	}
}

void RoleActivityYunZeCatchElves::RASendInfo()
{
}

void RoleActivityYunZeCatchElves::CatchElves()
{
	const RandActivityYunZeCatchElvesConfig * config = static_cast<const RandActivityYunZeCatchElvesConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_CATCH_ELVES));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RAYZCatchElvesBasicsCfg * elves_cfg = config->GetRandRewardCfg();
	if (NULL == elves_cfg) return;

	const RAYZCatchElvesRewardCfg * reward_cfg = config->GetRewardByGroup(ramgr, elves_cfg->reward_group);
	if (NULL == reward_cfg) return;

	std::vector<ItemConfigData> reward_list;
	reward_list.insert(reward_list.end(), reward_cfg->reward.begin(), reward_cfg->reward.end());

	bool is_extra_reward = false;
	if (elves_cfg->extra_reward_group != 0)
	{
		const RAYZCatchElvesRewardCfg * extra_reward_cfg = config->GetRewardByGroup(ramgr, elves_cfg->extra_reward_group);
		if (NULL == extra_reward_cfg) return;

		reward_list.insert(reward_list.end(), extra_reward_cfg->reward.begin(), extra_reward_cfg->reward.end());

		is_extra_reward = true;
	}

	if (reward_list.empty()) return;

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(config->GetOtherCfg().consume_item,config->GetOtherCfg().consume_num,__FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)reward_list.size(), &reward_list[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)reward_list.size(), &reward_list[0], PUT_REASON_RA_YUNZE_CATCH_ELVES);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&reward_list[0], (int)reward_list.size(), SEND_MAIL_TYPE_YUNZE_CATCH_ELVES);
	}

	std::string gamelog_str;
	for (int i = 0; i < reward_list.size(); ++i)
	{
		gamelog_str += STRING_SPRINTF("{item[%d, %d]} ", reward_list[i].item_id, reward_list[i].num);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityYunZeCatchElves::%s role[%d, %s] surplus_item[%d] get_reward[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetKnapsack()->Count(config->GetOtherCfg().consume_item), gamelog_str.c_str());

	Protocol::SCRAYunZeCatchElvesGetInfo yunze_cg;
	memset(yunze_cg.catch_elves_num, 0, sizeof(yunze_cg.catch_elves_num));
	yunze_cg.catch_elves_num[0] = is_extra_reward ? 1 : 0;
	yunze_cg.catch_elves_num[elves_cfg->elves_id] += 1;
	m_role_module_mgr->NetSend(&yunze_cg, sizeof(yunze_cg));

	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);

	this->JoinActivity();
}

void RoleActivityYunZeCatchElves::BatchCatchElves()
{
	const RandActivityYunZeCatchElvesConfig * config = static_cast<const RandActivityYunZeCatchElvesConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_CATCH_ELVES));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (config->GetOtherCfg().consume_num > m_role_module_mgr->GetKnapsack()->Count(config->GetOtherCfg().consume_item))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	std::vector<ItemConfigData> reward_list;
	Protocol::SCRAYunZeCatchElvesGetInfo yunze_cg;
	memset(yunze_cg.catch_elves_num, 0, sizeof(yunze_cg.catch_elves_num));
	bool is_extra_reward = false;

	int times = (m_role_module_mgr->GetKnapsack()->Count(config->GetOtherCfg().consume_item) - 
				(m_role_module_mgr->GetKnapsack()->Count(config->GetOtherCfg().consume_item) % config->GetOtherCfg().consume_num)) / config->GetOtherCfg().consume_num;		// 可抽奖次数
	times = times > 99 ? 99 : times;			// 一次性最大99抽

	for (int i = 0; i < times; i++)
	{
		const RAYZCatchElvesBasicsCfg * rand_elves_cfg = config->GetRandRewardCfg();
		if (NULL == rand_elves_cfg) return;

		yunze_cg.catch_elves_num[rand_elves_cfg->elves_id] += 1;
	}

	for (int j = 1; j <= config->GetElvesNumCfg(); ++j)
	{
		const RAYZCatchElvesBasicsCfg * elves_cfg = config->GetBasicsByElvesCfg(j);
		if (NULL == elves_cfg) return;

		const RAYZCatchElvesRewardCfg * reward_cfg = config->GetRewardByGroup(ramgr, elves_cfg->reward_group);
		if (NULL == reward_cfg) return;

		std::vector<ItemConfigData> t_item_list;
		t_item_list = reward_cfg->reward;

		if (yunze_cg.catch_elves_num[j] != 0)
		{
			for (int i = 0; i < (int)t_item_list.size(); ++i)
			{
				t_item_list[i].num = reward_cfg->reward[i].num * yunze_cg.catch_elves_num[elves_cfg->elves_id];
			}

			reward_list.insert(reward_list.end(), t_item_list.begin(), t_item_list.end());

			if (elves_cfg->extra_reward_group != 0)
			{
				const RAYZCatchElvesRewardCfg * extra_reward_cfg = config->GetRewardByGroup(ramgr, elves_cfg->extra_reward_group);
				if (NULL == extra_reward_cfg) return;

				t_item_list = extra_reward_cfg->reward;
				for (int x = 0; x < (int)t_item_list.size(); ++x)
				{
					t_item_list[x].num = extra_reward_cfg->reward[x].num * yunze_cg.catch_elves_num[elves_cfg->elves_id];
				}

				reward_list.insert(reward_list.end(), t_item_list.begin(), t_item_list.end());

				is_extra_reward = true;
			}
		}

	}

	if (reward_list.empty()) return;

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(config->GetOtherCfg().consume_item, config->GetOtherCfg().consume_num * times, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)reward_list.size(), &reward_list[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)reward_list.size(), &reward_list[0], PUT_REASON_RA_YUNZE_CATCH_ELVES);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&reward_list[0], (int)reward_list.size(), SEND_MAIL_TYPE_YUNZE_CATCH_ELVES);
	}

	std::string gamelog_str;
	for (int i = 0; i < reward_list.size(); ++i)
	{
		gamelog_str += STRING_SPRINTF("{item[%d, %d]} ", reward_list[i].item_id, reward_list[i].num);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityYunZeCatchElves::%s role[%d, %s] times[%d] surplus_item[%d] get_reward[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), times, m_role_module_mgr->GetKnapsack()->Count(config->GetOtherCfg().consume_item), gamelog_str.c_str());


	yunze_cg.catch_elves_num[0] = is_extra_reward ? 1 : 0;
	m_role_module_mgr->NetSend(&yunze_cg, sizeof(yunze_cg));

	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	this->JoinActivity(times);
}

void RoleActivityYunZeCatchElves::JoinActivity(int times)
{
	{
		RoleActivityChaoZhiFenXiang * role_activity = static_cast<RoleActivityChaoZhiFenXiang *>
			(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG));
		if (NULL != role_activity)
		{
			role_activity->AddTaskComplete(RA_CHAOZHI_FENXIANG_TASK_TYPE_JINGLING_DAZHUABU, times);
		}
	}
}



