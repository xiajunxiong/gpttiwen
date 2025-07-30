#include "roleactivityyuanqijishibu.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityyuanqijishibuconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"

#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "servercommon/uniqueidgenerator.hpp"

RoleActivityYuanQiJiShiBu::RoleActivityYuanQiJiShiBu(int type) :RoleActivity(type)
{
}

RoleActivityYuanQiJiShiBu::~RoleActivityYuanQiJiShiBu()
{
}

void RoleActivityYuanQiJiShiBu::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAYuanQiJiShiBuParam));
}

void RoleActivityYuanQiJiShiBu::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAYuanQiJiShiBuParam));
}

void RoleActivityYuanQiJiShiBu::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
		return;

	this->RASendInfo();
}

void RoleActivityYuanQiJiShiBu::CheckInitRandActivity(int rand_act_type, bool is_force)
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

		if (m_param.now_day_id != EngineAdapter::Instance().DayID())
		{
			m_param.now_day_id = EngineAdapter::Instance().DayID();
			this->OnCompleteTask(RA_YUAN_QI_JISHIBU_TASK_TYPE_LOGIN);
		}

		this->SetRoleDataChange();
	}
}

void RoleActivityYuanQiJiShiBu::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
	case 0: // 请求信息
		{
			this->RASendInfo();
		}
		break;
	case 1:	// 领取免费奖励
		{
			this->RAFetchReward(raor->param_1);
		}
		break;

	case 2:	// 领取进阶奖励
		{
			this->RAFetchJingJieReward(raor->param_1);
		}
		break;
	case 3:
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

void RoleActivityYuanQiJiShiBu::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
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

	m_param.complete_flag = 0;
	memset(m_param.task_list, 0, sizeof(m_param.task_list));

	if (m_param.now_day_id != now_dayid)
	{
		m_param.now_day_id = now_dayid;
		this->OnCompleteTask(RA_YUAN_QI_JISHIBU_TASK_TYPE_LOGIN);
	}
	else
	{	// 天数ID相等的时候说明今天已经完成登录任务了，但是上面已经重置过，所以把这个任务在加回来，但是不给奖励
		this->OnCompleteTask(RA_YUAN_QI_JISHIBU_TASK_TYPE_LOGIN, 1, false);
	}

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityYuanQiJiShiBu::OnRAClose()
{
	this->OnRewardBuFa();
}

void RoleActivityYuanQiJiShiBu::OnLoginCheck()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		this->OnRewardBuFa();
	}
}

void RoleActivityYuanQiJiShiBu::OnCompleteTask(int task_type, int add_param, bool is_reward)
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

	const RandActivityYuanQiJiShiBuConfig * config = static_cast<const RandActivityYuanQiJiShiBuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
		return;

	const std::vector<RAYuanQiJiShiBuTaskCfg> * task_map = config->GetTaskListCfg(ramgr);
	if (NULL == task_map)
		return;

	bool is_send = false;
	for (unsigned int i = 0; i < task_map->size(); i++)
	{
		if (task_type != task_map->at(i).task_type)
			continue;

		if (IsSetBit(m_param.complete_flag, i))
			continue;

		m_param.task_list[i] += add_param;
		if (m_param.task_list[i] >= task_map->at(i).param)
		{
			m_param.task_list[i] = task_map->at(i).param;

			SetBit(m_param.complete_flag, i);

			if (is_reward)
			{
				m_param.combat += task_map->at(i).combat;

				//log
				gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] task_index[%d] task_param[%d] huozhong[%d]", __FUNCTION__,
					m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
					i, m_param.task_list[i], m_param.combat);
			}
		}

		is_send = true;
	}

	if (is_send)
	{
		this->RASendInfo();
		this->SetRoleDataChange();
	}
}

int RoleActivityYuanQiJiShiBu::OnBuyItemCheck(long long ARG_OUT & unique_key)
{
	if (m_param.is_jinjie_jiangli != 0)
	{
		return -__LINE__;
	}

	const RandActivityYuanQiJiShiBuConfig * config = static_cast<const RandActivityYuanQiJiShiBuConfig *>
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
	buy_item->need_gold = config->GetOtherCfg()->price;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityYuanQiJiShiBu::OnCmdBuyItem(int chong_zhi_gold, long long unique_id)
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

	m_param.is_jinjie_jiangli = 1;

	this->RASendInfo();
	this->SetRoleDataChange();

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

int RoleActivityYuanQiJiShiBu::OnBuyGiftItem(ARG_OUT int * price)
{
	if (m_param.is_jinjie_jiangli != 0)
	{
		return 2;
	}

	const RandActivityYuanQiJiShiBuConfig * config = static_cast<const RandActivityYuanQiJiShiBuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}

	if (NULL != price)
	{
		*price = config->GetOtherCfg()->price;
		return 0;
	}

	m_param.is_jinjie_jiangli = 1;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

void RoleActivityYuanQiJiShiBu::RASendInfo()
{
	Protocol::SCRAYuanQiJiShiBuInfo info;
	info.combat = m_param.combat;
	info.is_jinjie_jiangli = m_param.is_jinjie_jiangli;
	info.quanming_reward_fetch_flag = m_param.quanming_reward_fetch_flag;
	info.jinjie_reward_fetch_flag = m_param.jinjie_reward_fetch_flag;
	info.complete_flag = m_param.complete_flag;

	memcpy(info.task_list, m_param.task_list, sizeof(info.task_list));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityYuanQiJiShiBu::RAFetchReward(int index)
{	
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityYuanQiJiShiBuConfig * config = static_cast<const RandActivityYuanQiJiShiBuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
		return;

	const RAYuanQiJiShiBuRewardCfg * reward_cfg = config->GetRewardCfg(ramgr, index);
	if (NULL == reward_cfg)
		return;

	if (IsSetBit(m_param.quanming_reward_fetch_flag, index))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	if (m_param.combat < reward_cfg->combat)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_QIYUAN_KONGMINGDENG_NOT_ITEM);
		return;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPut2(reward_cfg->reward, false))
	{
		m_role_module_mgr->GetKnapsack()->Put(reward_cfg->reward, PUT_REASON_RA_YUANQI_JISHIBU);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->reward, 1, SEND_MAIL_TYPE_TIGER_ACTIVITY, true, m_role_activity_type);
	}

	SetBit(m_param.quanming_reward_fetch_flag, index);

	this->RASendInfo();
	this->SetRoleDataChange();

	// log
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] index[%d] combat[%d]", 
		__FUNCTION__, 
		m_role_module_mgr->GetRole()->GetUID(), 
		m_role_module_mgr->GetRole()->GetName(), 
		m_role_module_mgr->GetRole()->GetLevel(), 
		m_role_activity_type,
		index,
		m_param.combat);
}

void RoleActivityYuanQiJiShiBu::RAFetchJingJieReward(int index)
{
	if (m_param.is_jinjie_jiangli == 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_YUANQI_JISHIBU_NOT_BUY_JINJIE);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityYuanQiJiShiBuConfig * config = static_cast<const RandActivityYuanQiJiShiBuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
		return;

	const RAYuanQiJiShiBuRewardCfg * reward_cfg = config->GetRewardCfg(ramgr, index);
	if (NULL == reward_cfg)
		return;

	if (IsSetBit(m_param.jinjie_reward_fetch_flag, index))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	if (m_param.combat < reward_cfg->combat)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_QIYUAN_KONGMINGDENG_NOT_ITEM);
		return;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutList2(reward_cfg->reward_2, false))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(reward_cfg->reward_2, PUT_REASON_RA_YUANQI_JISHIBU);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->reward_2[0], (int)reward_cfg->reward_2.size(), SEND_MAIL_TYPE_TIGER_ACTIVITY, true, m_role_activity_type);
	}

	SetBit(m_param.jinjie_reward_fetch_flag, index);

	this->RASendInfo();
	this->SetRoleDataChange();

	// log
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] index[%d] combat[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		index, m_param.combat);
}

void RoleActivityYuanQiJiShiBu::OnRewardBuFa()
{
	if (m_param.combat <= 0)
		return;

	// 要求没买进阶的 免费的都不给补
	if (m_param.is_jinjie_jiangli == 0)
		return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityYuanQiJiShiBuConfig * config = static_cast<const RandActivityYuanQiJiShiBuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
		return;

	const std::vector<RAYuanQiJiShiBuRewardCfg> * map = config->GetRewardMap(ramgr);
	if (NULL == map)
		return;

	for (int i = 0; i < (int)map->size(); i++)
	{
		if(m_param.combat < map->at(i).combat)
			continue;

		if (!IsSetBit(m_param.quanming_reward_fetch_flag, i))
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&map->at(i).reward, 1, SEND_MAIL_TYPE_ACTIVITY_END, false, m_role_activity_type);

			SetBit(m_param.quanming_reward_fetch_flag, i);

			gamelog::g_log_role_activity.printf(LL_INFO, "%s quanming_reward user[%d, %s], level[%d], activity_type[%d] index[%d] combat[%d]", __FUNCTION__,
				m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
				i, m_param.combat);
		}

// 		if (m_param.is_jinjie_jiangli == 0)
// 			continue;

		if (!IsSetBit(m_param.jinjie_reward_fetch_flag, i))
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&map->at(i).reward_2[0], (int)map->at(i).reward_2.size(), SEND_MAIL_TYPE_ACTIVITY_END, false, m_role_activity_type);

			SetBit(m_param.jinjie_reward_fetch_flag, i);

			gamelog::g_log_role_activity.printf(LL_INFO, "%s jinjie_reward user[%d, %s], level[%d], activity_type[%d] index[%d] combat[%d]", __FUNCTION__,
				m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
				i, m_param.combat);
		}
	}

	m_param.combat = 0;

	this->SetRoleDataChange();
}
