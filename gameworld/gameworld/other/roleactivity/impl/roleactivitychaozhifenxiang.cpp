#include "roleactivitychaozhifenxiang.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitychaozhifenxiangconfig.hpp"

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

RoleActivityChaoZhiFenXiang::RoleActivityChaoZhiFenXiang(int type) :RoleActivity(type)
{
}

RoleActivityChaoZhiFenXiang::~RoleActivityChaoZhiFenXiang()
{
}

void RoleActivityChaoZhiFenXiang::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAChaoZhiFenXiangParam));
}

void RoleActivityChaoZhiFenXiang::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAChaoZhiFenXiangParam));
}

void RoleActivityChaoZhiFenXiang::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG))
		return;

	this->RASendInfo();
}

void RoleActivityChaoZhiFenXiang::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG))
	{
		return;
	}

	this->OnRewardPatch();

	const RandActivityChaoZhiFenXiangConfig * config = static_cast<const RandActivityChaoZhiFenXiangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG));
	if (NULL == config)
		return;

	m_param.task_fetch_mark = 0;
	memset(m_param.task_comple, 0, sizeof(m_param.task_comple));
	
	if (m_param.task_comple[RA_CHAOZHI_FENXIANG_TASK_TYPE_LOGIN] <= 0)
	{
		this->AddTaskComplete(RA_CHAOZHI_FENXIANG_TASK_TYPE_LOGIN);
	}

	m_param.zhigou_mark &= (~(1 << 1));
	if (config->GetOtherCfg().buy_type == 1) m_param.zhigou_mark &= (~(1 << 0));

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityChaoZhiFenXiang::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG != rand_act_type)
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
		this->AddTaskComplete(RA_CHAOZHI_FENXIANG_TASK_TYPE_LOGIN);

		this->SetRoleDataChange();
	}
}

void RoleActivityChaoZhiFenXiang::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG);

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
			this->OnBuy();
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
	case 4:
		{
			this->OnFetchZhiGouReward();
		}
		break;
	}
}

void RoleActivityChaoZhiFenXiang::OnRAClose()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (0 == (m_param.zhigou_mark & (1 << 0)))
		return;

	const RandActivityChaoZhiFenXiangConfig * config = static_cast<const RandActivityChaoZhiFenXiangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG));
	if (NULL == config)
		return;

	if (0 == (m_param.zhigou_mark & (1 << 1)))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&config->GetOtherCfg().reward_item, 1, SEND_MAIL_TYPE_ACTIVITY_END, false, RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG);
		m_param.zhigou_mark |= 1 << 1;

		this->RASendInfo();
	}

	int complete_num = 0;
	const std::vector<RAChaoZhiFenXiangTaskCfg> * task_list = config->GetTaskCfg(ramgr, -1);
	if (NULL == task_list)
		return;

	for (std::vector<RAChaoZhiFenXiangTaskCfg>::const_iterator it = task_list->begin(); it != task_list->end(); it++)
	{
		int task_type = it->type;
		if (m_param.task_comple[task_type] >= it->parameter)
			complete_num += 1;
	}

	int REWARD_NUM = config->GetRewardNum(ramgr);
	for (int i = 0; i < REWARD_NUM; i++)
	{
		if (0 != (m_param.task_fetch_mark & (1 << i)))
			continue;

		const RAChaoZhiFenXiangRewardCfg * cfg = config->GetRewardCfg(ramgr, i);
		if (NULL == cfg)
			continue;

		if (complete_num < cfg->task)
			continue;

		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward_item[0], (short)cfg->reward_item.size(), SEND_MAIL_TYPE_ACTIVITY_END, false, RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG);

		m_param.task_fetch_mark |= 1 << i;
	}
}

int RoleActivityChaoZhiFenXiang::OnBuyGiftItem(ARG_OUT int * price)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}

	const RandActivityChaoZhiFenXiangConfig * config = static_cast<const RandActivityChaoZhiFenXiangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG));
	if (NULL == config)
	{
		return 1;
	}
	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return 1;
	}

	if (config->GetOtherCfg().price_type != 3)
	{
		return 1;
	}
	if (NULL != price)
	{
		*price = config->GetOtherCfg().price;
		return 0;
	}
	if (0 != (m_param.zhigou_mark & (1 << 0)))
	{
		return 2;
	}

	m_param.zhigou_mark |= 1 << 0;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type);

	return 0;
}

int RoleActivityChaoZhiFenXiang::OnBuyItemCheck(long long ARG_OUT & unique_key)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityChaoZhiFenXiangConfig * config = static_cast<const RandActivityChaoZhiFenXiangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG));
	if (NULL == config)
	{
		return -__LINE__;
	}

	if (config->GetOtherCfg().price_type != 3)
	{
		return -__LINE__;
	}

	if (0 != (m_param.zhigou_mark & (1 << 0)))
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
	buy_item->need_gold = config->GetOtherCfg().price;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_role_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_role_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityChaoZhiFenXiang::OnCmdBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_role_activity.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return -__LINE__;
	}

	m_param.zhigou_mark |= 1 << 0;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG);

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_role_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	return 0;
}

void RoleActivityChaoZhiFenXiang::AddTaskComplete(int task_type, int add_times)
{
	if (task_type < 0 || task_type >= RA_CHAOZHI_FENXIANG_TASK_TYPE_MAX)
		return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG))
		return;

	m_param.task_comple[task_type] += add_times;

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityChaoZhiFenXiang::RASendInfo()
{
	Protocol::SCRAChaoZhiFenXiangInfo info;
	info.task_fetch_mark = m_param.task_fetch_mark;
	info.zhigou_mark = m_param.zhigou_mark;
	memcpy(info.task_comple, m_param.task_comple, sizeof(info.task_comple));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityChaoZhiFenXiang::OnFetchReward(int task_seq)
{
	if (0 == (m_param.zhigou_mark & (1 << 0)))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_ZHIGOU_NOT_BUY);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityChaoZhiFenXiangConfig * config = static_cast<const RandActivityChaoZhiFenXiangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (0 != (m_param.task_fetch_mark & (1 << task_seq)))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	int complete_num = 0;
	const std::vector<RAChaoZhiFenXiangTaskCfg> * task_list = config->GetTaskCfg(ramgr);
	if (NULL == task_list)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	for (std::vector<RAChaoZhiFenXiangTaskCfg>::const_iterator it = task_list->begin(); it != task_list->end(); it++)
	{
		int task_type = it->type;
		if (m_param.task_comple[task_type] >= it->parameter)
			complete_num += 1;
	}

	const RAChaoZhiFenXiangRewardCfg * cfg = config->GetRewardCfg(ramgr, task_seq);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (complete_num < cfg->task)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ANECDOTE_TASK_NOT_FINISH);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)cfg->reward_item.size(), &cfg->reward_item[0], false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward_item[0], (short)cfg->reward_item.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->PutVec(cfg->reward_item, PUT_REASON_CHAOZHI_FENXIANG);
	}

	m_param.task_fetch_mark |= 1 << task_seq;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	// log
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] task_seq[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG,
		task_seq);
}

void RoleActivityChaoZhiFenXiang::OnBuy()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityChaoZhiFenXiangConfig * config = static_cast<const RandActivityChaoZhiFenXiangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (config->GetOtherCfg().price_type == 3)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (0 != (m_param.zhigou_mark & (1 << 0)))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_COW_REPORT_GOOD_NEWS_BUY_TIMES_LIMIT);
		return;
	}

	if (config->GetOtherCfg().price_type == 0)
	{
		if (!m_role_module_mgr->GetMoney()->UseGold(config->GetOtherCfg().price, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}
	}
	else
	{
		if (!m_role_module_mgr->GetMoney()->UseOtherCoin(config->GetOtherCfg().price, MONEY_TYPE_SILVER_COIN, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_SILVER_COIN_NOT_ENOUGH);
			return;
		}
	}

	m_param.zhigou_mark |= 1 << 0;

	this->RASendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	// log
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] price_type[%d], price[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG, 
		config->GetOtherCfg().price_type, config->GetOtherCfg().price);
}

void RoleActivityChaoZhiFenXiang::OnFetchZhiGouReward()
{
	if (0 == (m_param.zhigou_mark & (1 << 0)))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_NOT_ELIGIBLE);
		return;
	}

	if (0 != (m_param.zhigou_mark & (1 << 1)))
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

	const RandActivityChaoZhiFenXiangConfig * config = static_cast<const RandActivityChaoZhiFenXiangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut2(config->GetOtherCfg().reward_item, false, false))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&config->GetOtherCfg().reward_item, 1, SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->Put(config->GetOtherCfg().reward_item, PUT_REASON_CHAOZHI_FENXIANG);
	}

	m_param.zhigou_mark |= 1 << 1;

	this->RASendInfo();
	this->SetRoleDataChange();

	// log
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] ", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);
}

void RoleActivityChaoZhiFenXiang::OnRewardPatch()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG))
		return;

	if (0 == (m_param.zhigou_mark & (1 << 0)))
		return;

	const RandActivityChaoZhiFenXiangConfig * config = static_cast<const RandActivityChaoZhiFenXiangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG));
	if (NULL == config)
		return;

	if (0 == (m_param.zhigou_mark & (1 << 1)))
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&config->GetOtherCfg().reward_item, 1, SEND_MAIL_TYPE_ACTIVITY_END, false, RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG);
		m_param.zhigou_mark |= 1 << 1;

		this->RASendInfo();
	}

	int complete_num = 0;
	const std::vector<RAChaoZhiFenXiangTaskCfg> * task_list = config->GetTaskCfg(ramgr, -1);
	if (NULL == task_list)
		return;

	for (std::vector<RAChaoZhiFenXiangTaskCfg>::const_iterator it = task_list->begin(); it != task_list->end(); it++)
	{
		int task_type = it->type;
		if (m_param.task_comple[task_type] >= it->parameter)
			complete_num += 1;
	}

	int REWARD_NUM = config->GetRewardNum(ramgr);
	for (int i = 0; i < REWARD_NUM; i++)
	{	
		if (0 != (m_param.task_fetch_mark & (1 << i)))
			continue;

		const RAChaoZhiFenXiangRewardCfg * cfg = config->GetRewardCfg(ramgr, i);
		if (NULL == cfg)
			continue;

		if (complete_num < cfg->task)
			continue;

		m_role_module_mgr->GetKnapsack()->SendMail(&cfg->reward_item[0], (short)cfg->reward_item.size(), SEND_MAIL_TYPE_ACTIVITY_END, false, RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG);

		m_param.task_fetch_mark |= 1 << i;
	}
}
