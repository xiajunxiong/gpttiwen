#include "roleactivitywanlingzhuli.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitywanlingzhuliconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "servercommon/uniqueidgenerator.hpp"

RoleActivityWanLingZhuLi::RoleActivityWanLingZhuLi(int type)
	:RoleActivity(type)
{
}

RoleActivityWanLingZhuLi::~RoleActivityWanLingZhuLi()
{
}

void RoleActivityWanLingZhuLi::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAWanLingZhuLiParam));
}

void RoleActivityWanLingZhuLi::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAWanLingZhuLiParam));
}

void RoleActivityWanLingZhuLi::OnLoginSendInfo()
{
	this->RASendInfo();
}

void RoleActivityWanLingZhuLi::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}

	m_param.day_reward_mark = 0;
	if(m_param.is_add_day == 0) m_param.login_day += 1;
	m_param.is_add_day = 0;

	this->RASendInfo();
	this->SetRoleDataChange();
}

void RoleActivityWanLingZhuLi::CheckInitRandActivity(int rand_act_type, bool is_force)
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
		m_param.login_day = 1;
		m_param.is_add_day = 1;

		this->SetRoleDataChange();
	}
}

void RoleActivityWanLingZhuLi::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(raor->rand_activity_type))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}
	this->CheckInitRandActivity(raor->rand_activity_type);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();
		}
		break;
	case 1:
		{
			this->OnFetchDayReward();
		}
		break;
	case 2:
		{
			this->OnFetchReward(raor->param_1, raor->param_2 != 0);
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

void RoleActivityWanLingZhuLi::OnRAClose()
{
	this->OnRewardBuFa();
}

void RoleActivityWanLingZhuLi::OnLoginCheck()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		this->OnRewardBuFa();
	}
}

int RoleActivityWanLingZhuLi::OnBuyItemCheck(long long ARG_OUT & unique_key)
{
	if (m_param.is_buy_jiagou != 0)
	{
		return -__LINE__;
	}

	const RandActivityWanLingZhuLiConfig * config = static_cast<const RandActivityWanLingZhuLiConfig *>
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

int RoleActivityWanLingZhuLi::OnCmdBuyItem(int chong_zhi_gold, long long unique_id)
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

	m_param.is_buy_jiagou = 1;

	this->RASendInfo();
	this->SetRoleDataChange();

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

int RoleActivityWanLingZhuLi::OnBuyGiftItem(ARG_OUT int * price)
{
	if (m_param.is_buy_jiagou != 0)
	{
		return 2;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}
	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return 1;
	}

	const RandActivityWanLingZhuLiConfig * config = static_cast<const RandActivityWanLingZhuLiConfig *>
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

	m_param.is_buy_jiagou = 1;

	this->RASendInfo();
	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);


	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type);

	return 0;
}

void RoleActivityWanLingZhuLi::RASendInfo()
{
	Protocol::SCRAWanLingZhuLiInfo info;
	info.login_day = m_param.login_day;
	info.fetch_reward_mark = m_param.fetch_reward_mark;
	info.day_reward_mark = m_param.day_reward_mark;
	info.is_buy_jiagou = m_param.is_buy_jiagou;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityWanLingZhuLi::OnFetchDayReward()
{
	if (m_param.day_reward_mark != 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
		return;

	const RandActivityWanLingZhuLiConfig * config = static_cast<const RandActivityWanLingZhuLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
		return;

	if (m_role_module_mgr->GetKnapsack()->CheckForPut2(config->GetOtherCfg()->reward_item, false))
	{
		m_role_module_mgr->GetKnapsack()->Put(config->GetOtherCfg()->reward_item, PUT_REASON_RA_WANLING_ZHULI);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&config->GetOtherCfg()->reward_item, 1, SEND_MAIL_TYPE_TIGER_ACTIVITY, true, m_role_activity_type);
	}

	m_param.day_reward_mark = 1;

	this->RASendInfo();
	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] day_reward_mark[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		m_param.day_reward_mark);
}

void RoleActivityWanLingZhuLi::OnFetchReward(int day_index, bool is_zhongji)
{
	if (m_param.is_buy_jiagou == 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_ZHIGOU_NOT_BUY);
		return;
	}

	if (IsSetBit(m_param.fetch_reward_mark, day_index))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

// 	if ((!is_zhongji && day_index >= m_param.login_day) || day_index > m_param.login_day)
// 	{
// 		m_role_module_mgr->NoticeNum(errornum::EN_RA_LOGIN_DAY_NOT);
// 		return;
// 	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
		return;

	const RandActivityWanLingZhuLiConfig * config = static_cast<const RandActivityWanLingZhuLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
		return;

	int act_real_open_day = config->GetActOpenDay(ramgr, m_role_activity_type);
	if ((!is_zhongji && day_index >= act_real_open_day) || day_index > act_real_open_day)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_LOGIN_DAY_NOT);
		return;
	}

	const RAWanLingZhuLiRewardCfg * cfg = config->GetRewardCfg(ramgr, day_index);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutList2(cfg->reward, false))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(cfg->reward, PUT_REASON_RA_WANLING_ZHULI);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(cfg->reward, SEND_MAIL_TYPE_TIGER_ACTIVITY, true, m_role_activity_type);
	}

	SetBit(m_param.fetch_reward_mark, day_index);

	this->RASendInfo();
	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] index[%d], fetch_reward_mark[%d] day_num[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		day_index, m_param.fetch_reward_mark, m_param.login_day);
}

void RoleActivityWanLingZhuLi::OnRewardBuFa()
{
	if (m_param.is_buy_jiagou == 0)
		return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
		return;

	const RandActivityWanLingZhuLiConfig * config = static_cast<const RandActivityWanLingZhuLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
		return;

	bool is_send_reward = false;
	int max_num = config->GetRewardNum(ramgr);
	for (int i = 0; i < max_num; i++)
	{
		if (IsSetBit(m_param.fetch_reward_mark, i))
			continue;

		const RAWanLingZhuLiRewardCfg * cfg = config->GetRewardCfg(ramgr, i);
		if (NULL == cfg)
			continue;

		m_role_module_mgr->GetKnapsack()->SendMail(cfg->reward, SEND_MAIL_TYPE_ACTIVITY_END, false, m_role_activity_type);

		SetBit(m_param.fetch_reward_mark, i);

		is_send_reward = true;
		gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] index[%d] fetch_reward_mark[%d]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
			i, m_param.fetch_reward_mark);
	}

	if (!is_send_reward) return;

	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);
}
