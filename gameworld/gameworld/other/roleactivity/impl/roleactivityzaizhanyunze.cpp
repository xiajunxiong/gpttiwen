#include "roleactivityzaizhanyunze.hpp"
#include "config/randactivityconfig/impl/razaizhanyunzeconfig.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/route/mailroute.hpp"
#include "item/knapsack.h"
#include "battle/battle_manager_local.hpp"

#include "servercommon/string/gameworldstr.h"
#include "servercommon/errornum.h"
#include "gamelog.h"

RoleActivityZaiZhanYunZe::RoleActivityZaiZhanYunZe(int type)
	: RoleActivity(type)
{
}

RoleActivityZaiZhanYunZe::~RoleActivityZaiZhanYunZe()
{
}

void RoleActivityZaiZhanYunZe::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAZaiZhanYunZeParam));
}

void RoleActivityZaiZhanYunZe::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAZaiZhanYunZeParam));
}

void RoleActivityZaiZhanYunZe::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleActivityZaiZhanYunZe::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

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

void RoleActivityZaiZhanYunZe::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}
	this->CheckInitRandActivity(m_role_activity_type);

	switch (raor->opera_type)
	{
	case Protocol::RA_ZAI_ZHAN_YUN_ZE_REQ_TYPE_BOSS_BATTLE:
		{
			this->BossBattle(raor->param_1);
		}
		break;
	default:
		return;
	}
}

void RoleActivityZaiZhanYunZe::BossBattle(int boss_index)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	Role * role = m_role_module_mgr->GetRole();
	if (NULL == role)
	{
		return;
	}

	//不可组队
	if (role->InTeam())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_IN_TEAM);
		return;
	}

	//配置
	const RAZaiZhanYunZeConfig * config = static_cast<const RAZaiZhanYunZeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}
	const RAZaiZhanYunZe::OtherCfg & other_cfg = config->GetOtherCfg();

	const RAZaiZhanYunZe::BossCfg * boss_cfg = config->GetBossCfg(boss_index);
	if (NULL == boss_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	//活动开启天数不足
	int act_open_day = config->GetActOpenDay(ramgr, m_role_activity_type);
	if (boss_cfg->act_open_day > act_open_day)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HAVE_NOT_REACH_ACTIVE_CONDITION);
		return;
	}

	//检查门票
	ItemID cost_item = other_cfg.battle_cost_item_id;
	int cost_num = boss_cfg->battle_cost_item_num;
	if (!m_role_module_mgr->GetKnapsack()->HasItem(cost_item, cost_num, false))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_OTHER_MONEY_NOT_ENOUGH);
		return;
	}

	//战斗
	if (BattleManagerLocal::GetInstance().StartBattleModeFightReq(m_role_module_mgr->GetRole(), BATTLE_MODE_ZAI_ZHAN_YUN_ZE,
		boss_cfg->monster_group_id, boss_index, 0, false))
	{
		//消耗门票
		m_role_module_mgr->GetKnapsack()->ConsumeItem(cost_item, cost_num, __FUNCTION__);
	}

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityZaiZhanYunZe::OnBarrleFinish(int boss_index)
{
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

	//配置
	const RAZaiZhanYunZeConfig * config = static_cast<const RAZaiZhanYunZeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const RAZaiZhanYunZe::BossCfg * boss_cfg = config->GetBossCfg(boss_index);
	if (NULL == boss_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	//记录
	m_param.act_boss_info_[boss_index].pass_time++;
	this->SetRoleDataChange();
	this->SendInfo();
	//奖励总存
	std::vector<ItemConfigData> temp_reward_vec;
	
	if (1 == m_param.act_boss_info_[boss_index].pass_time)
	{//放入首通奖励
		const RAZaiZhanYunZe::RewardVec * first_reward_cfg
			= config->GetDefRewardGroupCfg(ramgr, boss_cfg->first_pass_id);
		if (NULL != first_reward_cfg)
		{
			temp_reward_vec.insert(temp_reward_vec.end(), first_reward_cfg->begin(), first_reward_cfg->end());
		}
	}

	{//放入通关次数奖励
		RAZaiZhanYunZe::BossCfg::PassNumIDMap::const_iterator pass_num_id_cit
			= boss_cfg->pass_num_id_map.find(m_param.act_boss_info_[boss_index].pass_time);
		if (boss_cfg->pass_num_id_map.end() != pass_num_id_cit)
		{
			const RAZaiZhanYunZe::RewardVec * pass_num_reward_cfg
				= config->GetDefRewardGroupCfg(ramgr, pass_num_id_cit->second);
			if (NULL != pass_num_reward_cfg)
			{
				temp_reward_vec.insert(temp_reward_vec.end(), pass_num_reward_cfg->begin(), pass_num_reward_cfg->end());
			}
		}
	}

	{//放入通关随机奖励
		const RAZaiZhanYunZe::RewardVec * pass_reward_cfg
			= config->GetRandRewardGroupCfg(ramgr, boss_cfg->pass_rand_id);
		if (NULL != pass_reward_cfg)
		{
			temp_reward_vec.insert(temp_reward_vec.end(), pass_reward_cfg->begin(), pass_reward_cfg->end());
		}
	}

	//奖励发放
	if (!temp_reward_vec.empty())
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)temp_reward_vec.size(), &temp_reward_vec[0], false))
		{
			//发邮件
			m_role_module_mgr->GetKnapsack()->SendMail(&temp_reward_vec[0], (short)temp_reward_vec.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, m_role_activity_type);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->PutList((short)temp_reward_vec.size(), &temp_reward_vec[0], PUT_REASON_RA_ZAI_ZHAN_YUN_ZE);
		}

		//奖励通知
		m_role_module_mgr->GetKnapsack()->NotifyGetItemList((short)temp_reward_vec.size(), &temp_reward_vec[0], GET_REASON_ZAI_ZHAN_YUN_ZE);
	}

	//日志
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], boss_index[%d], pase_num[%d]",
		__FUNCTION__,m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		boss_index, m_param.act_boss_info_[boss_index].pass_time);
}

void RoleActivityZaiZhanYunZe::SendInfo()
{
	Protocol::SCRAZaiZhanYunZeBase info;
	info.boss_info_count = RAZaiZhanYunZe::BOSS_MAX;
	for (int i = 0; i < RAZaiZhanYunZe::BOSS_MAX; i++)
	{
		info.boss_pass_num[i] = m_param.act_boss_info_[i].pass_time;
	}

	m_role_module_mgr->NetSend(&info, sizeof(info));

}
