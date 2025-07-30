#include "rolehundredgodboss.hpp"
#include "gamelog.h"

#include "protocol/msgother.h"
#include "servercommon/errornum.h"

#include "item/knapsack.h"
#include "task/taskrecorder.h"
#include "task/taskpool.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/partner/partner.hpp"
#include "other/partner/partnerconfig.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/hundredgodbossconfig.hpp"


RoleHundredGodBoss::RoleHundredGodBoss()
{
}

RoleHundredGodBoss::~RoleHundredGodBoss()
{
}

void RoleHundredGodBoss::Init(RoleModuleManager * module_mgr)
{
	m_role_module_mgr = module_mgr;
}

void RoleHundredGodBoss::Update(unsigned int now_second)
{
	return ;
	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	//超时则检测是否有已完成的,已完成则补发
	if (0 == common_data.is_finish_hundred_god_boss_flag && 0 != common_data.hundred_god_boss_end_timestamp && now_second > common_data.hundred_god_boss_end_timestamp)
	{
		this->TimeOver();
	}
}

void RoleHundredGodBoss::OnRoleLogin()
{
	return ;
	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	//老玩家处理,老玩家如果已经完成配置的前置任务字段的后置任务.则已完成,如果获取不到配置也算完成
	if (0 == common_data.is_old_user_hundred_god_boss_flag)
	{
		const HundredGodBossOtherCfg & other_cfg = LOGIC_CONFIG->GetHundredGodBossConfig()->GetOtherCfg();
		const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(other_cfg.pre_task);
		if (NULL != task_cfg)
		{
			if (m_role_module_mgr->GetTaskRecorder()->HaveFinished(task_cfg->nex_task_id))
			{
				common_data.is_finish_hundred_god_boss_flag = 1;
			}
		}
		else
		{
			common_data.is_finish_hundred_god_boss_flag = 1;
		}

		common_data.is_old_user_hundred_god_boss_flag = 1;
	}

	this->SendAllInfo();
}

void RoleHundredGodBoss::OnFinishBounty()
{
	return ;
	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	if (0 != common_data.is_finish_hundred_god_boss_flag)
	{
		return;
	}
	common_data.hundred_god_boss_fin_bounty_count += 1;
	this->SendAllInfo();
}

void RoleHundredGodBoss::OnStart()
{
	return ;
	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	if (0 != common_data.is_finish_hundred_god_boss_flag || 0 != common_data.hundred_god_boss_end_timestamp)
	{
		return;
	}
	const HundredGodBossOtherCfg & other_cfg = LOGIC_CONFIG->GetHundredGodBossConfig()->GetOtherCfg();
	if (!m_role_module_mgr->GetTaskRecorder()->HaveFinished(other_cfg.pre_task))
	{
		return;
	}

	common_data.hundred_god_boss_end_timestamp = (unsigned int)(EngineAdapter::Instance().Time() + other_cfg.during_time * SECOND_PER_HOUR);

	this->SendAllInfo();
	gamelog::g_log_role_hundred_god_boss.printf(LL_INFO, "%s user[%d, %s]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName());
}

void RoleHundredGodBoss::OnFetchTargetReward(int target_id)
{
	return ;
	const HundredGodBossTargetCfg * target_cfg = LOGIC_CONFIG->GetHundredGodBossConfig()->GetTargetCfg(target_id);
	if (NULL == target_cfg || target_id <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	if (0 != common_data.is_finish_hundred_god_boss_flag)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_HUNDRED_GOD_BOSS_FINISH_NOT_FETCH);
		return;
	}
	int target_index = target_id - 1;
	if (common_data.fetch_hundred_god_boss_flag & (1 << target_index))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_BOSS_CHAPTER_REWARD_CLAIMED);
		return;
	}
	
	if (EngineAdapter::Instance().Time() > common_data.hundred_god_boss_end_timestamp)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_HUNDRED_GOD_BOSS_END);
		return;
	}

	if (!this->IsFinishCond(target_cfg->cond_type, target_cfg->param))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_HUNDRED_GOD_BOSS_TARGET_NOT_FINISH);
		return;
	}
	if (!m_role_module_mgr->GetKnapsack()->CheckForPut2(target_cfg->reward))
	{
		return;
	}
	if (!m_role_module_mgr->GetKnapsack()->Put(target_cfg->reward, PUT_REASON_ROLE_HUNDRED_GOD_BOSS))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	common_data.fetch_hundred_god_boss_flag |= (1 << target_index);
	this->SendAllInfo();
	gamelog::g_log_role_hundred_god_boss.printf(LL_INFO, "%s user[%d, %s] target_id[%d]", __FUNCTION__, m_role_module_mgr->GetUid(), 
		m_role_module_mgr->GetRoleName(), target_id);
}

void RoleHundredGodBoss::OnFecthFinishReward()
{
	return ;
	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	if (0 != common_data.is_finish_hundred_god_boss_flag)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_HUNDRED_GOD_BOSS_FINISH_NOT_FETCH);
		return;
	}

	if (EngineAdapter::Instance().Time() > common_data.hundred_god_boss_end_timestamp)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_HUNDRED_GOD_BOSS_END);
		return;
	}

	if (!this->IsAllFinish())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ACTIVE_SYSTEM_NOT_ENOUGH);
		return;
	}

	if (!m_role_module_mgr->GetPartner()->ActivePartnerByOther(LOGIC_CONFIG->GetHundredGodBossConfig()->GetOtherCfg().partner_id, __FUNCTION__))
	{
		//激活失败则给发激活道具
		const PartnerBasicCfg * partner_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(LOGIC_CONFIG->GetHundredGodBossConfig()->GetOtherCfg().partner_id);
		if (NULL == partner_cfg)
		{
			return;
		}
		bool is_put_succ = false;
		ItemConfigData reward(partner_cfg->unlock_item_norex, true, 1);
		if (m_role_module_mgr->GetKnapsack()->CheckForPut(reward.item_id, 1))
		{
			if (m_role_module_mgr->GetKnapsack()->Put(reward, PUT_REASON_ROLE_HUNDRED_GOD_BOSS))
			{
				is_put_succ = true;
			}
		}
		if (!is_put_succ)
		{
			if (!m_role_module_mgr->GetKnapsack()->SendMail(&reward, 1, SNED_MAIL_TYPE_DEFAULT, true))
			{
				return;
			}
		}
	}
	//领取成功则把其它没领的奖励也发放
	std::vector<ItemConfigData> reissue_reward_list;
	for (int i = 0; i < (int)(sizeof(common_data.fetch_hundred_god_boss_flag) * 8); ++i)
	{
		if (common_data.fetch_hundred_god_boss_flag & (1 << i))
		{
			continue;
		}
		const HundredGodBossTargetCfg * target_cfg = LOGIC_CONFIG->GetHundredGodBossConfig()->GetTargetCfg(i + 1);
		if (NULL == target_cfg)
		{
			break;
		}
		if (!this->IsFinishCond(target_cfg->cond_type, target_cfg->param))
		{
			continue;
		}
		reissue_reward_list.push_back(target_cfg->reward);
		common_data.fetch_hundred_god_boss_flag |= (1 << i);
		gamelog::g_log_role_hundred_god_boss.printf(LL_INFO, "%s user[%d, %s] target_id[%d]", __FUNCTION__, m_role_module_mgr->GetUid(),
			m_role_module_mgr->GetRoleName(), i + 1);
	}
	if (!reissue_reward_list.empty())
	{
		m_role_module_mgr->GetKnapsack()->SendMail(reissue_reward_list, SEND_MAIL_TYPE_HUNDRED_GOD_BOSS_REWARD_REISSUE);
	}

	common_data.is_finish_hundred_god_boss_flag = 1;
	this->SendAllInfo();
	gamelog::g_log_role_hundred_god_boss.printf(LL_INFO, "%s user[%d, %s]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName());
}

void RoleHundredGodBoss::SendAllInfo()
{
	return ;
	Protocol::SCHundredGodBossInfo info;
	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();
	info.reserve_ch = 0;
	info.is_finish_hundred_god_boss_flag = common_data.is_finish_hundred_god_boss_flag;
	info.fetch_hundred_god_boss_flag = common_data.fetch_hundred_god_boss_flag;
	info.hundred_god_boss_fin_bounty_count = common_data.hundred_god_boss_fin_bounty_count;
	info.reserve_sh = 0;
	info.hundred_god_boss_end_timestamp = common_data.hundred_god_boss_end_timestamp;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

bool RoleHundredGodBoss::IsAllFinish()
{
	std::vector<HundredGodBossTargetCfg> target_cfg_list;
	LOGIC_CONFIG->GetHundredGodBossConfig()->GetAllTargetCfg(target_cfg_list);
	if (target_cfg_list.empty())
	{
		return true;
	}
	
	for (int i = 0; i < (int)target_cfg_list.size(); ++i)
	{
		if (!this->IsFinishCond(target_cfg_list[i].cond_type, target_cfg_list[i].param))
		{
			return false;
		}
	}

	return true;
}

bool RoleHundredGodBoss::IsFinishCond(int cond_type, int param)
{
	switch (cond_type)
	{
		case HUNDRED_GOD_BOSS_COND_TYPE_LEVEL:
		{
			if (m_role_module_mgr->GetRole()->GetLevel() < param)
			{
				return false;
			}
		}
		break;
		case HUNDRED_GOD_BOSS_COND_TYPE_TASK:
		{
			if (!m_role_module_mgr->GetTaskRecorder()->HaveFinished(param))
			{
				return false;
			}
		}
		break;
		case HUNDRED_GOD_BOSS_COND_TYPE_BOUNTY_TASK:
		{
			if (m_role_module_mgr->GetCommonData().hundred_god_boss_fin_bounty_count < param)
			{
				return false;
			}
		}
		break;
		default:
		return false;
	}

	return true;
}

void RoleHundredGodBoss::TimeOver()
{
	CommonDataParam & common_data = m_role_module_mgr->GetCommonData();

	bool is_all_finish = true;
	std::vector<ItemConfigData> reward_list;
	for (int i = 0; i < (int)(sizeof(common_data.fetch_hundred_god_boss_flag) * 8); ++i)
	{
		if (common_data.fetch_hundred_god_boss_flag & (1 << i))
		{
			continue;
		}
		const HundredGodBossTargetCfg * target_cfg = LOGIC_CONFIG->GetHundredGodBossConfig()->GetTargetCfg(i + 1);
		if (NULL == target_cfg)
		{
			break;
		}
		if (!this->IsFinishCond(target_cfg->cond_type, target_cfg->param))
		{
			is_all_finish = false;
			continue;
		}
		reward_list.push_back(target_cfg->reward);
		common_data.fetch_hundred_god_boss_flag |= (1 << i);
	}
	if (is_all_finish)
	{
		const PartnerBasicCfg * partner_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(LOGIC_CONFIG->GetHundredGodBossConfig()->GetOtherCfg().partner_id);
		if (NULL != partner_cfg)
		{
			reward_list.push_back(ItemConfigData(partner_cfg->unlock_item_norex, true, 1));
		}
	}
	if (!reward_list.empty())
	{
		m_role_module_mgr->GetKnapsack()->SendMail(reward_list, SEND_MAIL_TYPE_HUNDRED_GOD_BOSS_TIME_OVER);
	}
	common_data.is_finish_hundred_god_boss_flag = 1;
	this->SendAllInfo();
}
