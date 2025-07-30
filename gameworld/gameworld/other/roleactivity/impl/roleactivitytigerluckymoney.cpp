#include "roleactivitytigerluckymoney.hpp"
#include "config/randactivityconfig/impl/randactivitytigerluckymoneyconfig.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "servercommon/errornum.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "gamelog.h"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/uniqueidgenerator.hpp"

RoleActivityTigerLuckyMoney::RoleActivityTigerLuckyMoney(int type)
	:RoleActivity(type)
{

}

RoleActivityTigerLuckyMoney::~RoleActivityTigerLuckyMoney()
{

}

void RoleActivityTigerLuckyMoney::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityTigerLuckyMoney::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityTigerLuckyMoney::Update(time_t now_second)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY))
	{
		return;
	}

	if (!m_param.task_status.IsBitSet(RATLM::RATLM_TASK_TYPE_ONLINE_TIME))
	{
		if (m_param.ra_online_time_timestamp == 0 || m_param.ra_online_time_timestamp < now_second)
		{
			m_param.ra_online_time_timestamp = EngineAdapter::Instance().Time() + 60;
		}
	}

	if (!m_param.task_status.IsBitSet(RATLM::RATLM_TASK_TYPE_ONLINE_TIME) && now_second == m_param.ra_online_time_timestamp)
	{
		m_param.ra_online_time_timestamp += 60;
		this->AddTaskParam(RATLM::RATLM_TASK_TYPE_ONLINE_TIME);
	}

}

void RoleActivityTigerLuckyMoney::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY))
	{
		return;
	}

	this->RASendInfo();
}

void RoleActivityTigerLuckyMoney::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY))
	{
		return;
	}

	const RandActivityTigerLuckyMoneyConfig * config = static_cast<const RandActivityTigerLuckyMoneyConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY));
	if (NULL == config)	return;

	m_param.role_level = m_role_module_mgr->GetRole()->GetLevel();
	m_param.DayReset();

	time_t cur_time = EngineAdapter::Instance().Time();
	time_t activity_end_time = ramgr->GetRandActivityEndTime(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY);

	if (cur_time + 86400 >= activity_end_time)			// 如果是最后一天，刷新领取
	{
		m_param.is_can_receive = 1;
	}

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityTigerLuckyMoney::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY != rand_act_type)
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

		const RandActivityTigerLuckyMoneyConfig * config = static_cast<const RandActivityTigerLuckyMoneyConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY));
		if (NULL == config)	return;

		const RATLMRewardCfg * reward_cfg = config->GetRewardCfg(ramgr);
		if (NULL == reward_cfg) return;

		m_param.lucky_money_num = reward_cfg->start_num;
		m_param.role_level = m_role_module_mgr->GetRole()->GetLevel();

		this->RASendInfo();
		this->SetRoleDataChange();
	}
}

void RoleActivityTigerLuckyMoney::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();			// 发协议
		}
		break;
	case 1:
		{
			this->ReceiveTaskReward(raor->param_1);	// 领取任务奖励,p1-任务seq
		}
		break;
	case 2:
		{
			this->ReceiveLuckyMoney(); // 领取压岁钱
		}
		break;
	default:
		break;
	}
}

void RoleActivityTigerLuckyMoney::AddTaskParam(int task_type, int task_param)
{
	if (task_type < 0 || task_type >= RATLM::RATLM_TASK_TYPE_MAX) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY)) return;

	const RandActivityTigerLuckyMoneyConfig * config = static_cast<const RandActivityTigerLuckyMoneyConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY));
	if (NULL == config)	return;

	const std::vector<RATLMBasisCfg> * basis_cfg = config->GetBasisCfg(ramgr, m_param.role_level);
	if (NULL == basis_cfg) return;

	std::vector<RATLMBasisCfg>::const_iterator it = basis_cfg->begin();
	for (; it != basis_cfg->end(); ++it)
	{
		if (it->task_type == task_type)
		{
			if (m_param.task_status.IsBitSet(task_type))
			{
				return;
			}

			m_param.task_extent[task_type] += task_param;

			if (m_param.task_extent[task_type] >= it->task_parameter)
			{
				m_param.task_extent[task_type] = it->task_parameter;

				m_param.task_status.SetBit(task_type);

				gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] activity_type[%d] finish_task_type[%d]", __FUNCTION__,
					m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY, task_type);
			}
			break;
		}
	}

	if (it == basis_cfg->end()) return;

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityTigerLuckyMoney::LevelUpToFirstTask(int cur_level)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY))
	{
		return;
	}

	const RandActivityTigerLuckyMoneyConfig * config = static_cast<const RandActivityTigerLuckyMoneyConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY));
	if (NULL == config)	return;

	int level = config->GetStartLevel();
	if (level == 0) return;

	if (level == cur_level)
	{
		m_param.role_level = m_role_module_mgr->GetRole()->GetLevel();

		this->RASendInfo();
		this->SetRoleDataChange();
	}
}

void RoleActivityTigerLuckyMoney::RASendInfo()
{
	Protocol::SCRATigerLuckyMoneyInfo msg;
	msg.is_can_receive = m_param.is_can_receive;
	msg.lucky_money_num = m_param.lucky_money_num;
	msg.role_level = m_param.role_level;
	msg.task_status = m_param.task_status;
	msg.task_reward_status = m_param.task_reward_status;

	memset(msg.task_extent, 0, sizeof(msg.task_extent));
	memcpy(msg.task_extent,m_param.task_extent,sizeof(msg.task_extent));

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

void RoleActivityTigerLuckyMoney::ReceiveTaskReward(int seq)
{
	if (seq < 0 || seq >= RATLM::RATLM_TASK_TYPE_MAX) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityTigerLuckyMoneyConfig * config = static_cast<const RandActivityTigerLuckyMoneyConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY));
	if (NULL == config)	return;

	const std::vector<RATLMBasisCfg> * basis_cfg = config->GetBasisCfg(ramgr, m_param.role_level);
	if (NULL == basis_cfg) return;

	std::vector<ItemConfigData> reward_list;
	std::vector<RATLMBasisCfg>::const_iterator it = basis_cfg->begin();
	for (; it != basis_cfg->end(); ++it)
	{
		if (it->seq == seq)
		{
			if (!it->reward_item.empty())
			{
				reward_list = it->reward_item;
			}
			break;
		}
	}
	if (it == basis_cfg->end()) return;

	if (!m_param.task_status.IsBitSet(it->task_type))
	{
		return;
	}

	if (m_param.task_reward_status.IsBitSet(it->task_type))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	if (!reward_list.empty())
	{
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(reward_list))
		{
			m_role_module_mgr->GetKnapsack()->PutVec(reward_list, PUT_REASON_RA_TIGER_LUCKY_MONEY);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&reward_list[0], (int)reward_list.size(), SEND_MAIL_TYPE_TIGER_LUCKY_MONEY);
		}
	}

	m_param.task_reward_status.SetBit(it->task_type);

	const RATLMRewardCfg * reward_cfg = config->GetRewardCfg(ramgr);
	if (reward_cfg == NULL) return;

	m_param.lucky_money_num += reward_cfg->add_num;

	time_t cur_time = EngineAdapter::Instance().Time();
	time_t activity_end_time = ramgr->GetRandActivityEndTime(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY);

	if (cur_time + 86400 >= activity_end_time)			// 如果是最后一天，刷新领取
	{
		m_param.is_can_receive = 1;
	}

	std::string gamelog_str;
	for (int i = 0; i < (int)reward_list.size(); ++i)
	{
		gamelog_str = STRING_SPRINTF("{item[%d, %d]} ", reward_list[i].item_id, reward_list[i].num);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] finish_task_type[%d] get_reward[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), it->task_type, gamelog_str.c_str());

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityTigerLuckyMoney::ReceiveLuckyMoney()
{
	if (m_param.is_can_receive == 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PLEASE_SPECIFIED_TIME_RECEIVE);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityTigerLuckyMoneyConfig * config = static_cast<const RandActivityTigerLuckyMoneyConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY));
	if (NULL == config)	return;

	const RATLMRewardCfg * reward_cfg = config->GetRewardCfg(ramgr);
	if (NULL == reward_cfg) return;

	std::string gamelog_str;

	if (reward_cfg->reward_type == RATLM::LUCKY_MONEY_TYPE_GOLD)
	{
		if (!m_role_module_mgr->GetMoney()->AddGold(m_param.lucky_money_num,__FUNCTION__))
		{
			return;
		}

		gamelog_str = STRING_SPRINTF("add_gold[%d]", m_param.lucky_money_num);
	}
	else if(reward_cfg->reward_type == RATLM::LUCKY_MONEY_TYPE_JADE)
	{
		if (!m_role_module_mgr->GetMoney()->AddOtherCoin(m_param.lucky_money_num, MONEY_TYPE_SILVER_COIN, __FUNCTION__))
		{
			return;
		}

		gamelog_str = STRING_SPRINTF("add_jade[%d]", m_param.lucky_money_num);
	}
	else if(reward_cfg->reward_type == RATLM::LUCKY_MONEY_TYPE_ITEM)
	{
		if (ITEMPOOL->GetItem(reward_cfg->item_id) == NULL) return;

		if (!m_role_module_mgr->GetKnapsack()->CheckForPut(reward_cfg->item_id, m_param.lucky_money_num, false))
		{
			return;
		}
		m_role_module_mgr->GetKnapsack()->Put(ItemConfigData(reward_cfg->item_id,true,m_param.lucky_money_num), SEND_MAIL_TYPE_TIGER_LUCKY_MONEY);

		gamelog_str = STRING_SPRINTF("add_item[%d-%d]", reward_cfg->item_id, m_param.lucky_money_num);
	}
	else
	{
		return;
	}

	m_param.is_can_receive = 0;
	m_param.lucky_money_num = 0;

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] get_reward[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), gamelog_str.c_str());

	this->RASendInfo();
	this->SetRoleDataChange();
}
