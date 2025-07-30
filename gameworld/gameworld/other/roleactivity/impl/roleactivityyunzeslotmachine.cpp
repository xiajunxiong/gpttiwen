#include "roleactivityyunzeslotmachine.hpp"
#include "config/randactivityconfig/impl/randactivityyunzeslotmachineconfig.hpp"
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

#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"


RoleActivityYunZeSlotMachine::RoleActivityYunZeSlotMachine(int type)
	:RoleActivity(type)
{

}

RoleActivityYunZeSlotMachine::~RoleActivityYunZeSlotMachine()
{

}

void RoleActivityYunZeSlotMachine::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityYunZeSlotMachine::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityYunZeSlotMachine::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE != rand_act_type)
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

void RoleActivityYunZeSlotMachine::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE))
	{
		return;
	}

	const RandActivityYunZeSlotMachineConfig * config = static_cast<const RandActivityYunZeSlotMachineConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAYZSlotMachineOtherCfg & other_cfg = config->GetOtherCfg();

	if (other_cfg.limit_type == YUN_ZE_SLOT_MACHINE_BUY_TYPE_TOTAL)
	{
		return;
	}

	if (other_cfg.limit_type == YUN_ZE_SLOT_MACHINE_BUY_TYPE_TODAY)
	{
		m_param.buy_times = 0;
	}

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityYunZeSlotMachine::Update(time_t now_second)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE))
		return;

	const RandActivityYunZeSlotMachineConfig * config = static_cast<const RandActivityYunZeSlotMachineConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	ItemID item = config->GetOtherCfg().reward_item.item_id;

	int item_num = m_role_module_mgr->GetKnapsack()->Count(item);
	if (0 == item_num) return;

	if (m_param.surplus_key != item_num)
	{
		m_param.surplus_key = item_num;
	}

}

void RoleActivityYunZeSlotMachine::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE))
	{
		return;
	}
	this->RASendInfo();
}

int RoleActivityYunZeSlotMachine::OnBuyGiftItem(ARG_OUT int * price)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}

	const RandActivityYunZeSlotMachineConfig * config = static_cast<const RandActivityYunZeSlotMachineConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}
	if (YUN_ZE_SLOT_MACHINE_MONEY_TYPE_RMB != config->GetOtherCfg().price_type)
	{
		return 1;
	}
	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return 1;
	}

	if (NULL != price)
	{
		*price = config->GetOtherCfg().price;
		return 0;
	}

	if (YUN_ZE_SLOT_MACHINE_BUY_TYPE_TOTAL == config->GetOtherCfg().limit_type)
	{
		if (m_param.buy_times >= config->GetOtherCfg().total_buy_times)
		{
			return 2;
		}
	}
	else if (YUN_ZE_SLOT_MACHINE_BUY_TYPE_TODAY == config->GetOtherCfg().limit_type)
	{
		if (m_param.buy_times >= config->GetOtherCfg().today_buy_times)
		{
			return 2;
		}
	}

	const ItemConfigData & item = config->GetOtherCfg().reward_item;
	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(item.item_id, item.num, false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&item, 1, SEND_MAIL_TYPE_ACTIVITY_REWARD, true, m_role_activity_type);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->Put(item, PUT_REASON_YUN_ZE_SLOT_MACHINE_BUY_KEY);
	}
	
	m_param.buy_times++;
	m_param.surplus_key += item.num;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d, %s] buy_times[%d] surplus_key[%d] use_RMB[%d]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.buy_times, m_param.surplus_key, config->GetOtherCfg().price);

	return 0;
}

void RoleActivityYunZeSlotMachine::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE);

	switch (raor->opera_type)
	{
	case RA_YUN_ZE_SLOT_MACHINE_TYPE_GET_INFO:
		{
			this->RASendInfo();
		}
		break;
	case RA_YUN_ZE_SLOT_MACHINE_TYPE_LOTTERY:
		{
			this->Lottery();
		}
		break;
	case RA_YUN_ZE_SLOT_MACHINE_TYPE_LOCK_REWARD:
		{
			this->LockRewardPool(raor->param_1);
		}
		break;
	case RA_YUN_ZE_SLOT_MACHINE_TYPE_BUY:
		{
			const RandActivityYunZeSlotMachineConfig * config = static_cast<const RandActivityYunZeSlotMachineConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE));
			if (NULL == config)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return;
			}

			if (config->GetOtherCfg().price_type == YUN_ZE_SLOT_MACHINE_MONEY_TYPE_RMB)
			{
				long long unique_id = 0;
				int ret_num = this->OnBuyItemCheck(unique_id);
				if (ret_num == 0)
				{
					Protocol::SCCmdBuyItemCheckRet protocol;
					protocol.unique_id = unique_id;
					protocol.cfg_ver = raor->cfg_ver;
					protocol.buy_type = BUY_TYPE;
					m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
				}
			}
			else if (config->GetOtherCfg().price_type == YUN_ZE_SLOT_MACHINE_MONEY_TYPE_GOLD || config->GetOtherCfg().price_type == YUN_ZE_SLOT_MACHINE_MONEY_TYPE_JADE)
			{
				this->BuyLotteryItem();
			}
		}
		break;
	default:
		break;
	}

}

void RoleActivityYunZeSlotMachine::RASendInfo()
{
	Protocol::SCRAYunZeSlotMachineInfo ramsg;
	ramsg.buy_times = m_param.buy_times;
	ramsg.surplus_key = m_param.surplus_key;
	memcpy(ramsg.lock_reward, m_param.lock_reward, sizeof(ramsg.lock_reward));

	m_role_module_mgr->NetSend(&ramsg, sizeof(ramsg));
}

int RoleActivityYunZeSlotMachine::RASendRandResultInfo(int rand_result)
{
	if (rand_result <= 0) return -1;

	for (int i = 0; i < YUN_ZE_SLOT_NACHINE_LOCK_REWARD_TYPE; ++i)
	{
		if (0 == m_param.lock_reward[i]) return -2;
	}

	Protocol::SCRAYunZeSlotMachineResultInfo ramsg;
	ramsg.rand_result = rand_result;

	m_role_module_mgr->NetSend(&ramsg, sizeof(ramsg));
	return 0;
}


void RoleActivityYunZeSlotMachine::Lottery()
{
	for (int i = 0; i < YUN_ZE_SLOT_NACHINE_LOCK_REWARD_TYPE; ++i)
	{
		if (m_param.lock_reward[i] == 0)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PLEASE_LOCK_ALL_REWARD);
			return;
		}
	}

	const RandActivityYunZeSlotMachineConfig * config = static_cast<const RandActivityYunZeSlotMachineConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAYZSlotMachineOtherCfg & other_cfg = config->GetOtherCfg();

	if (m_param.surplus_key < other_cfg.item_consume)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_LACK_ITEM);
		return;
	}

	 int rand_reward_pool = config->GetRandRewardByRateCfg();

	 if (0 != this->RASendRandResultInfo(rand_reward_pool))
	 {
		 return;
	 }
	 
	 this->SendReward(rand_reward_pool);
	 this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityYunZeSlotMachine::SendReward(int reward_pool)
{
	if (reward_pool <= YUN_ZE_SLOT_MACHINE_TYPE_BEGIN || reward_pool >= YUN_ZE_SLOT_MACHINE_TYPE_MAX)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityYunZeSlotMachineConfig * config = static_cast<const RandActivityYunZeSlotMachineConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAYZSlotMachineTaskRewardCfg * task_reward_cfg = config->GetTaskRewardCfg(ramgr);
	if (NULL == task_reward_cfg) return;

	std::string gamelog_str;

	std::vector<ItemConfigData> item_list;
	if (reward_pool == YUN_ZE_SLOT_MACHINE_TYPE_COMFORT)
	{
		for (int i = 0; i < (int)task_reward_cfg->reward_list.size(); ++i)
		{
			if (task_reward_cfg->reward_list[i].item_pool == reward_pool)
			{
				item_list.push_back(task_reward_cfg->reward_list[i].item);

				gamelog_str = STRING_SPRINTF("{item[%d, %d]} ", task_reward_cfg->reward_list[i].item.item_id, task_reward_cfg->reward_list[i].item.num);
			}
		}			
	}
	else
	{
		for (int i = reward_pool - 1; i < YUN_ZE_SLOT_NACHINE_LOCK_REWARD_TYPE; ++i)
		{
			for (int j = 0; j < (int)task_reward_cfg->reward_list.size(); ++j)
			{
				if (task_reward_cfg->reward_list[j].item_group == m_param.lock_reward[i])
				{
					item_list.push_back(task_reward_cfg->reward_list[j].item);

					gamelog_str += STRING_SPRINTF("{item[%d, %d]} ", task_reward_cfg->reward_list[i].item.item_id, task_reward_cfg->reward_list[i].item.num);
				}
			}
		}
	}

	if (item_list.empty())
	{
		return;
	}

	const ItemConfigData & item = config->GetOtherCfg().reward_item;
	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(item.item_id, config->GetOtherCfg().item_consume, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)item_list.size(), &item_list[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)item_list.size(), &item_list[0], PUT_REASON_YUN_ZE_SLOT_MACHINE, false);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&item_list[0], (int)item_list.size(), SEND_MAIL_TYPE_YUN_ZE_SLOT_MACHINE, true);
	}

	if (!item_list.empty())
	{
		m_role_module_mgr->GetKnapsack()->NotifyGetItemList((short)item_list.size(), &item_list[0], GET_REASON_YUNZE_SLOT_MACHINE);
	}

	m_param.surplus_key -= config->GetOtherCfg().item_consume; 

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityYunZeSlotMachine::%s role[%d, %s] surplus_key[%d] get_reward[reward_list:%d, %s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.surplus_key, (int)item_list.size(), gamelog_str.c_str());
}

void RoleActivityYunZeSlotMachine::LockRewardPool(int reward_id)
{
	if (reward_id <= 0 || reward_id > RA_YZ_SLOT_MACHINE_MAX_REWARD_POOL_NUM)
	{
		return;
	}

	const RandActivityYunZeSlotMachineConfig * config = static_cast<const RandActivityYunZeSlotMachineConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RAYZSlotMachineTaskRewardCfg * task_reward_cfg = config->GetTaskRewardCfg(ramgr);
	if (NULL == task_reward_cfg) return;

	int t_reward_pool = 0;
	for (int i = 0; i < (int)task_reward_cfg->reward_list.size(); ++i)
	{
		if (task_reward_cfg->reward_list[i].item_group == reward_id)
		{
			t_reward_pool = task_reward_cfg->reward_list[i].item_pool;
		}
	}
	if (0 == t_reward_pool)
	{
		return;
	}

	m_param.lock_reward[t_reward_pool - 1] = reward_id;

	this->RASendInfo();
}

void RoleActivityYunZeSlotMachine::BuyLotteryItem()
{
	const RandActivityYunZeSlotMachineConfig * config = static_cast<const RandActivityYunZeSlotMachineConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (YUN_ZE_SLOT_MACHINE_BUY_TYPE_TOTAL == config->GetOtherCfg().limit_type)
	{
		if (m_param.buy_times >= config->GetOtherCfg().total_buy_times)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_SHOP_BUY_LIMIT);
			return;
		}
	}
	else if(YUN_ZE_SLOT_MACHINE_BUY_TYPE_TODAY == config->GetOtherCfg().limit_type)
	{
		if (m_param.buy_times >= config->GetOtherCfg().today_buy_times)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_SHOP_BUY_LIMIT);
			return;
		}
	}

	const ItemConfigData & item = config->GetOtherCfg().reward_item;

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(item.item_id, item.num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	int money_type = 0;

	bool is_use_success = false;
	if (config->GetOtherCfg().price_type == YUN_ZE_SLOT_MACHINE_MONEY_TYPE_GOLD)
	{
		if (m_role_module_mgr->GetMoney()->GoldMoreThan(config->GetOtherCfg().price))
		{
			m_role_module_mgr->GetMoney()->UseGold(config->GetOtherCfg().price, __FUNCTION__);
			is_use_success = true;
			money_type = MONEY_TYPE_GOLD;
		}
		else
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}
	}
	else if (config->GetOtherCfg().price_type == YUN_ZE_SLOT_MACHINE_MONEY_TYPE_JADE)
	{
		if (m_role_module_mgr->GetMoney()->OtherCoinMoreThan(config->GetOtherCfg().price, MONEY_TYPE_SILVER_COIN))
		{
			m_role_module_mgr->GetMoney()->UseOtherCoin(config->GetOtherCfg().price, MONEY_TYPE_SILVER_COIN, __FUNCTION__);
			is_use_success = true;
			money_type = MONEY_TYPE_SILVER_COIN;
		}
		else
		{
			m_role_module_mgr->NoticeNum(errornum::EN_SILVER_COIN_NOT_ENOUGH);
			return;
		}
	}

	if (!is_use_success)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->Put(item, PUT_REASON_YUN_ZE_SLOT_MACHINE_BUY_KEY))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	m_param.buy_times++;
	m_param.surplus_key += item.num;

	this->RASendInfo();
	this->SetRoleDataChange();

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityYunZeSlotMachine::%s role[%d, %s] buy_times[%d] surplus_key[%d] use_money[type:%d, %d]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.buy_times, m_param.surplus_key, money_type, config->GetOtherCfg().price);
}

int RoleActivityYunZeSlotMachine::OnBuyItemCheck(long long ARG_OUT & unique_key)
{
	const RandActivityYunZeSlotMachineConfig * config = static_cast<const RandActivityYunZeSlotMachineConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return -__LINE__;
	}

	unique_key = 0;
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		unique_key = 0;
		return -__LINE__;
	}

	time_t now = EngineAdapter::Instance().Time();
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetNewCmdBuyRecord<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	buy_item->cfg_vesion = config->GetConfigVer();
	buy_item->need_gold = config->GetOtherCfg().price;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityYunZeSlotMachine::OnCmdBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return -__LINE__;
	}

	const RandActivityYunZeSlotMachineConfig * config = static_cast<const RandActivityYunZeSlotMachineConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return -__LINE__;
	}

	// 限购
	if (YUN_ZE_SLOT_MACHINE_BUY_TYPE_TOTAL == config->GetOtherCfg().limit_type)
	{
		if (m_param.buy_times >= config->GetOtherCfg().total_buy_times)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_SHOP_BUY_LIMIT);
			return -__LINE__;
		}
	}
	else if (YUN_ZE_SLOT_MACHINE_BUY_TYPE_TODAY == config->GetOtherCfg().limit_type)
	{
		if (m_param.buy_times >= config->GetOtherCfg().today_buy_times)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_SHOP_BUY_LIMIT);
			return -__LINE__;
		}
	}

	const ItemConfigData & item = config->GetOtherCfg().reward_item;

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(item.item_id, item.num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return -__LINE__;
	}

	if (!m_role_module_mgr->GetKnapsack()->Put(item, PUT_REASON_YUN_ZE_SLOT_MACHINE_BUY_KEY))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return -__LINE__;
	}
	
	m_param.buy_times++;
	m_param.surplus_key += item.num;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityYunZeSlotMachine::%s role[%d, %s] buy_times[%d] surplus_key[%d] use_RMB[%d]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), m_param.buy_times, m_param.surplus_key, config->GetOtherCfg().price);

	return 0;
}
