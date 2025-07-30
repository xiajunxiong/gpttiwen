#include "roleactivitywanlinggongying.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitywanlinggongyingconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"

#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "servercommon/uniqueidgenerator.hpp"

RoleActivityWanLingGongYing::RoleActivityWanLingGongYing(int type) :RoleActivity(type)
{
}

RoleActivityWanLingGongYing::~RoleActivityWanLingGongYing()
{
}

void RoleActivityWanLingGongYing::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAWanLingGongYingParam));
}

void RoleActivityWanLingGongYing::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAWanLingGongYingParam));
}

void RoleActivityWanLingGongYing::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING))
		return;

	this->RASendInfo();
}

void RoleActivityWanLingGongYing::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING != rand_act_type)
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

void RoleActivityWanLingGongYing::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();
		}
		break;
	case 1:
		{
			this->OnFetchReward(raor->param_1);
		}
		break;
	case 2:
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
		break;
	}
}

void RoleActivityWanLingGongYing::OnRAClose()
{
	if (m_param.ra_buy_reward_timestamp <= 0)
		return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
		return;

	const RandActivityWanLingGongYingConfig * config = static_cast<const RandActivityWanLingGongYingConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING));
	if (NULL == config)
		return;

	for (int i = 0; i < RA_WAN_LING_GONG_YING_TASK_MAX; i++)
	{
		if(m_param.task_list[i].task_flag != 1)
			continue;
		
		const RAWanLingGongYingTaskCfg * cfg = config->GeTaskCfg(ramgr, i);
		if (NULL == cfg)
			continue;

		m_role_module_mgr->GetKnapsack()->SendMail(cfg->reward_item, cfg->reward_count, SEND_MAIL_TYPE_ACTIVITY_END, true, RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING);

		m_param.task_list[i].task_flag = 2;

		this->SetRoleDataChange();
		// log
		gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] task_index[%d], is_complete[%d]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING,
			i, 1);
	}
}

void RoleActivityWanLingGongYing::OnCompleteTask(int activity_type, int time)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
		return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING))
		return;

	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING);

	const RandActivityWanLingGongYingConfig * config = static_cast<const RandActivityWanLingGongYingConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING));
	if (NULL == config)
		return;

	const std::vector<int> * cfg_list = config->GetTaskIndexCfg(ramgr, activity_type);
	if (NULL == cfg_list)
		return;

	bool is_send = false;
	for (unsigned int i = 0; i < cfg_list->size(); i++)
	{
		int task_index = (*cfg_list)[i];
		if(task_index < 0 || task_index >= RA_WAN_LING_GONG_YING_TASK_MAX)
			continue;

		const RAWanLingGongYingTaskCfg * cfg = config->GeTaskCfg(ramgr, task_index);
		if(NULL == cfg)
			continue;

		if(m_param.task_list[task_index].task_flag != 0)
			continue;

		m_param.task_list[task_index].param1 += time;

		if (m_param.task_list[task_index].param1 >= cfg->param1)
		{
			m_param.task_list[task_index].param1 = cfg->param1;
			m_param.task_list[task_index].task_flag = 1;
		}

		is_send = true;
	}

	if (is_send)
	{
		this->RASendInfo();
		this->SetRoleDataChange();
	}
}

int RoleActivityWanLingGongYing::OnBuyGiftItem(ARG_OUT int * price)
{
	if (m_param.ra_buy_reward_timestamp != 0)
	{
		return 2;
	}

	const RandActivityWanLingGongYingConfig * config = static_cast<const RandActivityWanLingGongYingConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return 1;
	}
	if (NULL != price)
	{
		*price = config->GetOtherCfg().buy_gold;
		return 0;
	}

	m_param.ra_buy_reward_timestamp = (unsigned int)EngineAdapter::Instance().Time();

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

int RoleActivityWanLingGongYing::OnBuyItemCheck(long long ARG_OUT & unique_key)
{
	if (m_param.ra_buy_reward_timestamp != 0)
	{
		return -__LINE__;
	}

	const RandActivityWanLingGongYingConfig * config = static_cast<const RandActivityWanLingGongYingConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
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
	buy_item->need_gold = config->GetOtherCfg().buy_gold;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityWanLingGongYing::OnCmdBuyItem(int chong_zhi_gold, long long unique_id)
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

	m_param.ra_buy_reward_timestamp = (unsigned int)EngineAdapter::Instance().Time();

	this->RASendInfo();
	this->SetRoleDataChange();

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

void RoleActivityWanLingGongYing::RASendInfo()
{
	Protocol::SCRAWanLingGongYingInfo info;
	info.ra_buy_reward_timestamp = m_param.ra_buy_reward_timestamp;
	memcpy(info.task_list, m_param.task_list, sizeof(info.task_list));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityWanLingGongYing::OnFetchReward(int task_index)
{
	if (task_index < 0 || task_index >= RA_WAN_LING_GONG_YING_TASK_MAX)
		return;

	if (m_param.task_list[task_index].task_flag == 2)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityWanLingGongYingConfig * config = static_cast<const RandActivityWanLingGongYingConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAWanLingGongYingTaskCfg * cfg = config->GeTaskCfg(ramgr, task_index);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}	

	bool is_complete = false;
	if (m_param.task_list[task_index].task_flag == 1)
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPutList(cfg->reward_count, cfg->reward_item, false))
		{
			m_role_module_mgr->GetKnapsack()->SendMail(cfg->reward_item, cfg->reward_count, SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->PutList(cfg->reward_count, cfg->reward_item, PUT_REASON_RA_WANLING_GONGYING);
		}

		is_complete = true;
	}
	else
	{
		// 没有参与过活动, 不给发奖励
// 		if (m_param.task_list[task_index].param1 <= 0)
// 			return;

		int activity_type = config->GetActivityType(cfg->type);
		if (activity_type < RAND_ACTIVITY_TYPE_BEGIN || activity_type >= RAND_ACTIVITY_TYPE_END)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
// 		unsigned int activity_end_time = ramgr->GetRandActivityEndTime(activity_type);
// 		
// 		// 不是 在子活动结束后才买的资格
// 		if (activity_end_time > m_param.ra_buy_reward_timestamp)
// 			return;

		if (ramgr->IsRandActivityOpen(activity_type))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_OPEN_OPER_FAIL);
			return;
		}

		if (!m_role_module_mgr->GetKnapsack()->CheckForPutList(cfg->unfinish_count, cfg->unfinish_item, false))
		{
			m_role_module_mgr->GetKnapsack()->SendMail(cfg->unfinish_item, cfg->unfinish_count, SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->PutList(cfg->unfinish_count, cfg->unfinish_item, PUT_REASON_RA_WANLING_GONGYING);
		}

		is_complete = false;
	}

	m_param.task_list[task_index].task_flag = 2;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	// log
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] task_index[%d], is_complete[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING,
		task_index, is_complete ? 1:0);
}
