#include "roleactivityluckybag.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityluckybagconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/funopen/funopen.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/uniqueidgenerator.hpp"

#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "protocol/randactivity/msgraluckybag.hpp"

#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/serverconfig/crossconfig.hpp"


RoleActivityLuckyBag::RoleActivityLuckyBag(int type)
	:RoleActivity(type)
{

}

RoleActivityLuckyBag::~RoleActivityLuckyBag()
{

}

void RoleActivityLuckyBag::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy( &m_param , p.base_data , sizeof(RALuckyBagParam));
}

void RoleActivityLuckyBag::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RALuckyBagParam));
}

void RoleActivityLuckyBag::OnLogin()
{
	this->CheckBatchBuyReward();
}

void RoleActivityLuckyBag::OnLoginSendInfo()
{
	this->RandActivityLuckyBagSendInfo();
}

void RoleActivityLuckyBag::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.ra_lucky_bag_buy_flag = 0;
	m_param.ra_lucky_bag_is_fetch_reward = 0;

	this->SetRoleDataChange();
	this->CheckBatchBuyReward();
	this->RandActivityLuckyBagSendInfo();	
}

void RoleActivityLuckyBag::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_LUCKY_BAG != rand_act_type)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_lucky_bag_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_lucky_bag_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		this->SetRoleDataChange();
	}
}

void RoleActivityLuckyBag::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
{
	switch (raor->opera_type)
	{
		case 0:
			{
				this->RandActivityLuckyBagSendInfo();
			}
			break;
		case 1:
			{
				this->OnRandActivityLuckyBagFetchDayReward();
			}
			break;
		case 2:
			{
				long long unique_id = 0;
				int ret_num = this->OnBuyItemCheck(raor->param_1, unique_id);			//购买福袋 配置seq

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
		case 3:
			{
				long long unique_id = 0;
				int ret_num = this->OnBuyItemCheck2(raor->param_1, unique_id);			//购买福袋 配置seq

				if (ret_num == 0)
				{
					Protocol::SCCmdBuyItemCheckRet protocol;
					protocol.unique_id = unique_id;
					protocol.cfg_ver = raor->cfg_ver;
					protocol.buy_type = BATCH_BUY_TYPE;

					m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
				}
			}
			break;
	}
}

int RoleActivityLuckyBag::OnBuyGiftItem(int seq, ARG_OUT int * price)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return 1;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_LUCKY_BAG))
	{
		return 1;
	}
	const RandActivityLuckyBagConfig * config = static_cast<const RandActivityLuckyBagConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LUCKY_BAG));
	if (NULL == config)
	{
		return 1;
	}
	const RandActivityLuckyBagBuyCfg * cfg = config->GetRandActivityLuckyBagBuyCfg(seq);
	if (cfg == NULL)
	{
		return 1;
	}
	if (m_param.ra_lucky_bag_buy_flag & (1 << seq))
	{
		return 2;
	}
	if (NULL != price)
	{
		*price = cfg->need_chong_zhi_gold;
		return 0;
	}

	if (!cfg->reward.empty())
	{
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)cfg->reward.size(), &cfg->reward[0]))
		{
			m_role_module_mgr->GetKnapsack()->PutList((short)cfg->reward.size(), &cfg->reward[0], PUT_REASON_RA_LUCKY_BAG);
		}
		else
		{
			static MailContentParam contentparam; contentparam.Reset();
			FillMailContentParam(contentparam, cfg->reward);

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chong_zhi_buy_item_reward_title);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chong_zhi_buy_item_reward_content, m_role_activity_type);
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
			}
		}
	}
	m_param.ra_lucky_bag_buy_flag |= (1 << seq);

	this->SetRoleDataChange();
	this->RandActivityLuckyBagSendInfo();
	this->OnParticipateRA(__FUNCTION__);
	return 0;
}

int RoleActivityLuckyBag::OnBuyGiftItem2(int seq, int ARG_OUT * price)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return 1;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_LUCKY_BAG))
	{
		return 1;
	}
	const RandActivityLuckyBagConfig * config = static_cast<const RandActivityLuckyBagConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LUCKY_BAG));
	if (NULL == config)
	{
		return 1;
	}
	const RandActivityLuckyBagBatchBuyCfg * cfg = config->GetRandActivityLuckyBagBatchBuyCfg(seq);
	if (cfg == NULL)
	{
		return 1;
	}

	if (m_param.ra_lucky_bag_buy_flag != 0)   //已经买过其他一项，不能进行批量购买操作
	{
		return 2;
	}

	if (NULL != price)
	{
		*price = cfg->need_chong_zhi_gold;
		return 0;
	}
	unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
	time_t zero_time = ::GetZeroTime(now);
	if (now > m_param.batch_buy_info.end_time)
	{
		if (!cfg->batch_buy_reward.empty())
		{
			if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)cfg->batch_buy_reward.size(), &cfg->batch_buy_reward[0]))
			{
				m_role_module_mgr->GetKnapsack()->PutList((short)cfg->batch_buy_reward.size(), &cfg->batch_buy_reward[0], PUT_REASON_RA_LUCKY_BAG);
			}
			else
			{
				static MailContentParam contentparam; contentparam.Reset();
				FillMailContentParam(contentparam, cfg->batch_buy_reward);

				int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chong_zhi_buy_item_reward_title);
				int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chong_zhi_buy_item_reward_content, m_role_activity_type);
				if (lensub > 0 && length > 0)
				{
					MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
				}
			}
		}
		m_param.batch_buy_info.end_time = zero_time + cfg->add_day * SECOND_PER_DAY;
		m_param.batch_buy_info.last_send_reward_time = zero_time;
	}
	else
	{
		m_param.batch_buy_info.end_time += (cfg->add_day + 1) * SECOND_PER_DAY;
	}

	this->SetRoleDataChange();
	this->RandActivityLuckyBagSendInfo();
	this->OnParticipateRA(__FUNCTION__);
	return 0;
}

bool RoleActivityLuckyBag::OnRandActivityLuckyBagBuy(int seq, int chong_zhi_gold)
{
	const RandActivityLuckyBagConfig * config = static_cast<const RandActivityLuckyBagConfig * >
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LUCKY_BAG));
	if (NULL == config)
	{
		return false;
	}

	const RandActivityLuckyBagBuyCfg * cfg = config->GetRandActivityLuckyBagBuyCfg(seq);
	if (cfg == NULL)
	{
		return false;
	}

	if (cfg->need_chong_zhi_gold != chong_zhi_gold)
	{
		return false;
	}

	if (m_param.ra_lucky_bag_buy_flag & (1 << seq))
	{
		return false;
	}

	if (!cfg->reward.empty())
	{
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)cfg->reward.size(), &cfg->reward[0]))
		{
			m_role_module_mgr->GetKnapsack()->PutList((short)cfg->reward.size(), &cfg->reward[0], PUT_REASON_RA_LUCKY_BAG);
		}
		else
		{
			static MailContentParam contentparam; contentparam.Reset();
			FillMailContentParam(contentparam, cfg->reward);

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chong_zhi_buy_item_reward_title);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chong_zhi_buy_item_reward_content, m_role_activity_type);
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
			}
		}
	}

	//m_role_module_mgr->GetMoney()->AddOtherCoin(cfg->add_luck_coin, MONEY_TYPE_RESERVE, __FUNCTION__);

	m_param.ra_lucky_bag_buy_flag |= (1 << seq);

	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);
	return true;
}

int RoleActivityLuckyBag::OnBuyItemCheck(int seq, long long ARG_OUT & unique_key)
{
	unique_key = 0;

	if (seq < 0 || seq >= sizeof(m_param.ra_lucky_bag_buy_flag) * 8)
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityLuckyBagConfig * config = static_cast<const RandActivityLuckyBagConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RandActivityLuckyBagBuyCfg * cfg = config->GetRandActivityLuckyBagBuyCfg(seq);
	if (cfg == NULL)
	{
		return -__LINE__;
	}

	time_t now = EngineAdapter::Instance().Time();
	if (now < m_param.batch_buy_info.end_time)
	{
		return -__LINE__;
	}

	if (m_param.ra_lucky_bag_buy_flag & (1 << seq))
	{
		return -__LINE__;
	}

	if (cfg->reward.empty())
	{
		return -__LINE__;
	}

	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		unique_key = 0;
		return -__LINE__;
	}

	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetNewCmdBuyRecord<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	buy_item->cfg_vesion = config->GetConfigVer();
	buy_item->need_gold = cfg->need_chong_zhi_gold;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_lucky_bag_begin_timestamp;
	buy_item->unique_id = unique_key;
	buy_item->param1 = seq;
	::ItemConfigDataVecToBuyItem(cfg->reward, buy_item->rewards_item, &buy_item->rewards_num);
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_lucky_bag, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_lucky_bag.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityLuckyBag::OnBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_lucky_bag.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
					   m_param.ra_lucky_bag_begin_timestamp, ::GetString(*buy_item).c_str());

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

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)rewards.size(), &rewards[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_RA_LUCKY_BAG);
	}
	else
	{
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chong_zhi_buy_item_reward_title);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chong_zhi_buy_item_reward_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
	
	//如果时间相等就说明同一次活动里购买，要增加次数。
	if (buy_item->ra_begin_timestamp == m_param.ra_lucky_bag_begin_timestamp)
	{
		int seq = buy_item->param1;
		m_param.ra_lucky_bag_buy_flag |= (1 << seq);

		this->RandActivityLuckyBagSendInfo();
		this->SetRoleDataChange();
	}

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_lucky_bag, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	return 0;
}

int RoleActivityLuckyBag::OnBuyItemCheck2(int seq, long long ARG_OUT & unique_key)
{
	unique_key = 0;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityLuckyBagConfig * config = static_cast<const RandActivityLuckyBagConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RandActivityLuckyBagBatchBuyCfg * cfg = config->GetRandActivityLuckyBagBatchBuyCfg(seq);
	if (cfg == NULL)
	{
		return -__LINE__;
	}

	if (m_param.ra_lucky_bag_buy_flag != 0)   //已经买过其他一项，不能进行批量购买操作
	{
		return -__LINE__;
	}

	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		unique_key = 0;
		return -__LINE__;
	}

	time_t now = EngineAdapter::Instance().Time();
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetNewCmdBuyRecord<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), BATCH_COMMON_SAVE_TYPE);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	buy_item->cfg_vesion = config->GetConfigVer();
	buy_item->need_gold = cfg->need_chong_zhi_gold;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_lucky_bag_begin_timestamp;
	buy_item->unique_id = unique_key;
	buy_item->param1 = cfg->add_day;
	::ItemConfigDataVecToBuyItem(cfg->batch_buy_reward, buy_item->rewards_item, &buy_item->rewards_num);
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), BATCH_COMMON_SAVE_TYPE, gamelog::g_log_lucky_bag, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_lucky_bag.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityLuckyBag::OnBuyItem2(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), BATCH_COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_lucky_bag.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
					m_param.ra_lucky_bag_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return  -__LINE__;
	}

	int add_day = buy_item->param1;
	time_t now = EngineAdapter::Instance().Time();
	time_t zero_time = ::GetZeroTime(now);
	if (now > m_param.batch_buy_info.end_time)
	{
		std::vector<ItemConfigData> rewards = ::BuyItemToItemConfigDataVec(buy_item->rewards_num, buy_item->rewards_item);
		if (rewards.empty())
		{
			return  -__LINE__;
		}

		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)rewards.size(), &rewards[0]))
		{
			m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_RA_LUCKY_BAG);
		}
		else
		{
			static MailContentParam contentparam; contentparam.Reset();
			FillMailContentParam(contentparam, rewards);

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chong_zhi_buy_item_reward_title);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chong_zhi_buy_item_reward_content, m_role_activity_type);
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
			}
		}

		m_param.batch_buy_info.end_time = zero_time + add_day * SECOND_PER_DAY;
		m_param.batch_buy_info.last_send_reward_time = zero_time;
	}
	else
	{
		m_param.batch_buy_info.end_time += (add_day + 1) * SECOND_PER_DAY;
	}

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), BATCH_COMMON_SAVE_TYPE, unique_id, gamelog::g_log_lucky_bag, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->SetRoleDataChange();
	this->RandActivityLuckyBagSendInfo();

	return 0;
}

void RoleActivityLuckyBag::RandActivityLuckyBagSendInfo()
{
	Protocol::SCRALuckyBagInfo msg;

	msg.ra_lucky_bag_buy_flag = m_param.ra_lucky_bag_buy_flag;
	msg.ra_lucky_bag_is_fetch_reward = m_param.ra_lucky_bag_is_fetch_reward;
	msg.batch_buy_end_time = m_param.batch_buy_info.end_time;
	msg.batch_buy_last_time = m_param.batch_buy_info.last_send_reward_time;
	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

void RoleActivityLuckyBag::OnRandActivityLuckyBagFetchDayReward()
{
	if (m_param.ra_lucky_bag_is_fetch_reward != 0)
	{
		return;
	}

	const RandActivityLuckyBagConfig * config = static_cast<const RandActivityLuckyBagConfig * >
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LUCKY_BAG));
	if (NULL == config) 
	{
		return;
	}

	const RandActivityLuckyBagOtherCfg & other_cfg = config->GetRandActivityLuckyBagOtherCfg();

	if (other_cfg.day_reward.empty())
	{
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->PutList((short)other_cfg.day_reward.size(), &other_cfg.day_reward[0], PUT_REASON_RA_LUCKY_BAG))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}
	m_param.ra_lucky_bag_is_fetch_reward += 1;

	this->SetRoleDataChange();
	this->RandActivityLuckyBagSendInfo();

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityLuckyBag::CheckBatchBuyReward()
{
	if (m_param.batch_buy_info.last_send_reward_time <= 0 || m_param.batch_buy_info.end_time <= 0)
	{
		return;
	}

	unsigned int now = (unsigned int)EngineAdapter::Instance().Time();	
	unsigned int min_time = std::min(now, m_param.batch_buy_info.end_time);
	if (min_time > 0)
	{
		min_time -= 1;
	}

	int times = (::GetZeroTime(min_time) - ::GetZeroTime(m_param.batch_buy_info.last_send_reward_time)) / SECOND_PER_DAY;

	if (times <= 0)
	{
		return;
	}

	const RandActivityLuckyBagConfig * config = static_cast<const RandActivityLuckyBagConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const RandActivityLuckyBagBatchBuyCfg * cfg = config->GetRandActivityLuckyBagBatchBuyCfg(0);
	if (cfg == NULL)
	{
		return;
	}

	for (int i = 0; i < times && i < 30; ++i)
	{
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, cfg->batch_buy_reward);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_lucky_bag_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_lucky_bag_content);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	m_param.batch_buy_info.last_send_reward_time = min_time;
	this->SetRoleDataChange();
}

