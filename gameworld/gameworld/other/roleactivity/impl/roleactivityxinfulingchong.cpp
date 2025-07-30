#include "roleactivityxinfulingchong.hpp"
#include "gamelog.h"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"

#include "item/money.h"
#include "item/knapsack.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"

#include "protocol/randactivity/msgraxinfulingchong.hpp"

#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/uniqueidgenerator.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityxinfulingchongconfig.hpp"

RoleActivityXinFuLingChong::RoleActivityXinFuLingChong(int type)
	:RoleActivity(type)
{
}

RoleActivityXinFuLingChong::~RoleActivityXinFuLingChong()
{
}

void RoleActivityXinFuLingChong::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAXinFuLingChongParam));
}

void RoleActivityXinFuLingChong::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAXinFuLingChongParam));
}

void RoleActivityXinFuLingChong::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}
	this->CheckInitRandActivity(m_role_activity_type);
	this->SendAllInfo();
}

void RoleActivityXinFuLingChong::CheckInitRandActivity(int rand_act_type, bool is_force)
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
		const RandActivityXinFuLingChongConfig * config = static_cast<const RandActivityXinFuLingChongConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
		if (NULL == config)
		{
			return;
		}
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->SetRoleDataChange();
	}
}

void RoleActivityXinFuLingChong::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
}

void RoleActivityXinFuLingChong::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
		case Protocol::RA_XIN_FU_LING_CHONG_OPERA_TYPE_INFO:
		{
			this->SendAllInfo();
		}
		break;
		case Protocol::RA_XIN_FU_LING_CHONG_OPERA_TYPE_BUY_ITEM:
		{
			this->BuyItem();
		}
		break;
	}
}

int RoleActivityXinFuLingChong::OnCmdBuyItem(long long unique_id, int chong_zhi_gold)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), 
		COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), 
		m_role_module_mgr->GetRole()->GetName(), m_param.ra_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return  -__LINE__;
	}

	std::vector<ItemConfigData> rewards = ::BuyItemToItemConfigDataVec(buy_item->rewards_num, buy_item->rewards_item);
	if (rewards.empty())
	{
		return  -__LINE__;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(rewards))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_XIN_FU_LING_CHONG);
	}
	else
	{
		// 发邮件
		m_role_module_mgr->GetKnapsack()->SendMail(rewards, SEND_MAIL_TYPE_ACTIVITY_REWARD_BAG_FULL, false, m_role_activity_type);
	}

	if (m_param.ra_begin_timestamp == buy_item->ra_begin_timestamp)
	{
		m_param.buy_count += 1;
		this->SetRoleDataChange();
		this->SendAllInfo();
	}
	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, 
		gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);
	return 0;
}

int RoleActivityXinFuLingChong::OnBuyGiftItem(ARG_OUT int * price)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	const RandActivityXinFuLingChongConfig * config = static_cast<const RandActivityXinFuLingChongConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	const RAXinFuLingChongRewardCfg & reward_cfg = config->GetRewardCfg();
	if (m_param.buy_count >= MAX_RA_XIN_FU_LING_CHONG_BUY_COUNT)
	{
		return BUY_GIFT_ITEM_RET_TYPE_2;
	}

	if (NULL != price)
	{
		*price = reward_cfg.price;
		return 0;
	}

	if (!reward_cfg.reward_list.empty())
	{
		bool is_put_succ = false;
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(reward_cfg.reward_list))
		{
			if (m_role_module_mgr->GetKnapsack()->PutVec(reward_cfg.reward_list, PUT_REASON_XIN_FU_LING_CHONG))
			{
				is_put_succ = true;
			}
		}
		if (!is_put_succ)
		{
			m_role_module_mgr->GetKnapsack()->SendMail(reward_cfg.reward_list, SEND_MAIL_TYPE_ACTIVITY_REWARD_BAG_FULL, false, m_role_activity_type);
		}
	}
	else
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}


	m_param.buy_count += 1;
	this->SendAllInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

void RoleActivityXinFuLingChong::SendAllInfo()
{
	Protocol::SCRAXinFuLingChongInfo info;
	info.buy_count = m_param.buy_count;
	
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityXinFuLingChong::BuyItem()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}
	const RandActivityXinFuLingChongConfig * config = static_cast<const RandActivityXinFuLingChongConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}
	if (m_param.buy_count >= MAX_RA_XIN_FU_LING_CHONG_BUY_COUNT)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
		return;
	}

	const RAXinFuLingChongRewardCfg & reward_cfg = config->GetRewardCfg();
	if (reward_cfg.is_cmd_buy)
	{
		this->OnBuyItemCheck(config);
		return;
	}
	
	if (!m_role_module_mgr->GetMoney()->MoneyTypeMoreThan(reward_cfg.price_type, reward_cfg.price))
	{	
		return;
	}
	
	if (!reward_cfg.reward_list.empty())
	{
		if (!m_role_module_mgr->GetMoney()->UseMoneyType(reward_cfg.price_type, reward_cfg.price, __FUNCTION__))
		{
			return;
		}

		bool is_put_succ = false;
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(reward_cfg.reward_list))
		{
			if (m_role_module_mgr->GetKnapsack()->PutVec(reward_cfg.reward_list, PUT_REASON_XIN_FU_LING_CHONG))
			{
				is_put_succ = true;
			}
		}
		if (!is_put_succ)
		{
			m_role_module_mgr->GetKnapsack()->SendMail(reward_cfg.reward_list, SEND_MAIL_TYPE_ACTIVITY_REWARD_BAG_FULL, false, m_role_activity_type);
		}
	}
	else
	{
		return;
	}

	m_param.buy_count += 1;
	this->SendAllInfo();

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s buy_count[%d] price_type[%d] price[%d]", __FUNCTION__, m_param.buy_count, reward_cfg.price_type, reward_cfg.price);
}

void RoleActivityXinFuLingChong::OnBuyItemCheck(const RandActivityXinFuLingChongConfig * ra_config)
{
	if (NULL == ra_config)
	{
		return;
	}
	const RAXinFuLingChongRewardCfg & reward_cfg = ra_config->GetRewardCfg();
	if (!reward_cfg.is_cmd_buy)
	{
		return;
	}

	time_t now = EngineAdapter::Instance().Time();
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetNewCmdBuyRecord<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE);
	if (buy_item == NULL)
	{
		return;
	}
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&buy_item->unique_id))
	{
		return;
	}

	buy_item->cfg_vesion = ra_config->GetConfigVer();
	buy_item->need_gold = reward_cfg.price;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	::ItemConfigDataVecToBuyItem(reward_cfg.reward_list, buy_item->rewards_item, &buy_item->rewards_num);
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_rand_activity, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	Protocol::SCCmdBuyItemCheckRet info;
	info.buy_type = BUY_TYPE;
	info.cfg_ver = buy_item->cfg_vesion;
	info.unique_id = buy_item->unique_id;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}
