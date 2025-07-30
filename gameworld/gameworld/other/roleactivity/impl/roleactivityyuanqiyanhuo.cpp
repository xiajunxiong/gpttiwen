#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/funopen/funopen.hpp"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/uniqueidgenerator.hpp"

#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "protocol/randactivity/msgrayuanqiyanhuo.hpp"
#include "config/randactivityconfig/impl/randactivityyuanqiyanhuoconfig.hpp"
#include "roleactivityyuanqiyanhuo.hpp"


RoleActivityYuanQiYanHuo::RoleActivityYuanQiYanHuo(int type) :RoleActivity(type)
{
}

RoleActivityYuanQiYanHuo::~RoleActivityYuanQiYanHuo()
{
}

void RoleActivityYuanQiYanHuo::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityYuanQiYanHuo::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityYuanQiYanHuo::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleActivityYuanQiYanHuo::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{	
	m_param.is_fetch_free_reward = 0;
	this->SendInfo();
	this->SetRoleDataChange();
}

void RoleActivityYuanQiYanHuo::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
	{
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity * activity = ramgr->ForceGetRandActivity(rand_act_type);
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(ramgr->GetRandActivityBeginTime(m_role_activity_type));
		this->OnCheckInitBonus();

		this->SetRoleDataChange();
	}
}

void RoleActivityYuanQiYanHuo::OnRAClose()
{
	this->GiveUnFetchReward();
}

void RoleActivityYuanQiYanHuo::OnLoginCheck()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity * activity = ramgr->ForceGetRandActivity(this->GetRoleActivityType());
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(this->GetRoleActivityType(), m_param.ra_begin_timestamp);
	if (in_activity_time)
	{
		return;
	}

	this->GiveUnFetchReward();
}

void RoleActivityYuanQiYanHuo::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	switch (raor->opera_type)
	{
		case 0://请求下发
			{
				this->SendInfo();
			}
			break;
		case 1:
			{
				long long unique_id = 0;
				int ret_num = this->OnBuyItemCheck(unique_id);	//购买定制宝箱	 param1 配置seq，param2 选择的宝箱按位来

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
		case 2:
			{
				this->FetchReward(raor->param_1);		//领取奖励 p1 是哪天的
			}
			break;
		case 3:
			{
				this->FetchFreeReward();		//领取免费奖励 
			}
			break;
		case 4:
			{
				this->OpenYanHuo();		//打开烟火物品 
			}
			break;
	}
}

int RoleActivityYuanQiYanHuo::FetchReward(int seq)
{
	if (!m_param.is_buy)
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityYuanQiYanHuoConfig * config = static_cast<const RandActivityYuanQiYanHuoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RandActivityYuanQiYanHuoItemCfg * cfg = config->GetRandActivityYuanQiYuanHuoItemCfg(ramgr, seq);
	if (!cfg)
	{
		return -__LINE__;
	}

	if (cfg->seq < 0 || cfg->seq >= BitCount(m_param.get_reward_flag))
	{
		return -__LINE__;
	}

	if (IsSetBit(m_param.get_reward_flag, cfg->seq))
	{
		return -__LINE__;
	}

	int act_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_YUAN_QI_YAN_HUO);
	int need_day = cfg->seq + 1;
	if (need_day > act_open_day)
	{
		return -__LINE__;
	}

	const std::vector<ItemConfigData> & rewards = cfg->rewards;
	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)rewards.size(), &rewards[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_RA_YUAN_QI_YAN_HUO);
	}
	else
	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_act_full_bag_title, m_role_activity_type);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_act_full_bag_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	SetBit(m_param.get_reward_flag, cfg->seq);
	this->SendInfo();
	this->SetRoleDataChange();


	gamelog::g_yuan_qi_yan_huo.printf(LL_INFO, "%s line:%d |  role[%d %s]  get_reward_flag:%ud seq:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.get_reward_flag, cfg->seq);

	return 0;
}

int RoleActivityYuanQiYanHuo::FetchFreeReward()
{
	if (m_param.is_fetch_free_reward)
	{
		ret_line;
	}

	const RandActivityYuanQiYanHuoConfig * config = static_cast<const RandActivityYuanQiYanHuoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RandActivityYuanQiYanHuoOtherCfg & other_cfg = config->GetOtherCfg();

	if (!m_role_module_mgr->GetKnapsack()->PutVec(other_cfg.free_rewards, PUT_REASON_RA_YUAN_QI_YAN_HUO))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HAS_NO_SPACING);
		return -__LINE__;
	}

	m_param.is_fetch_free_reward = 1;
	this->SendInfo();
	this->SetRoleDataChange();

	gamelog::g_yuan_qi_yan_huo.printf(LL_INFO, "%s line:%d |  role[%d %s]  is_fetch_free_reward:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.is_fetch_free_reward);

	return 0;
}

int RoleActivityYuanQiYanHuo::OpenYanHuo()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		ret_line;
	}

	const RandActivityYuanQiYanHuoConfig * config = static_cast<const RandActivityYuanQiYanHuoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		ret_line;
	}

	const RandActivityYuanQiYanHuoOtherCfg & other_cfg = config->GetOtherCfg();

	const tm * now_tm = EngineAdapter::Instance().LocalTime();

	int now_hour_min = now_tm->tm_hour * 100 + now_tm->tm_min;
	if (now_hour_min < other_cfg.yuan_huo_item_open_begin_time || other_cfg.yuan_huo_item_open_end_time < now_hour_min)
	{		
		ret_line;
	}

	if (!m_role_module_mgr->GetKnapsack()->HasItem(other_cfg.yuan_huo_item_id, 1))
	{
		ret_line;
	}

	const RandActivityYuanQiYanHuoYanHuoRewardItemCfg * cfg = config->GetRandActivityYuanQiYanHuoYanHuoRewardItemCfg(ramgr);
	if (!cfg)
	{
		ret_line;
	}

	if (!m_role_module_mgr->GetKnapsack()->PutVec(cfg->rewards, PUT_REASON_RA_YUAN_QI_YAN_HUO_OPEN_ITEM))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HAS_NO_SPACING);
		return -__LINE__;
	}

	m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.yuan_huo_item_id, 1, __FUNCTION__);
	m_role_module_mgr->GetKnapsack()->NotifyGetItemListVec(cfg->rewards, GET_REASON_YUAN_QI_YAN_HUO_OPEN);

	return 0;
}

void RoleActivityYuanQiYanHuo::GiveUnFetchReward()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!m_param.is_buy)
	{
		return;
	}

	const RandActivityYuanQiYanHuoConfig * config = static_cast<const RandActivityYuanQiYanHuoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const RandActivityYuanQiYanHuoSectionCfg * cfg = config->GetRandActivityYuanQiYuanHuoSectionCfg(ramgr, m_param.ra_begin_timestamp);
	if (!cfg)
	{
		return;
	}

	for (std::map<int, RandActivityYuanQiYanHuoItemCfg> ::const_iterator iter = cfg->item_list.begin(); iter != cfg->item_list.end(); ++iter)
	{
		const RandActivityYuanQiYanHuoItemCfg & curr = iter->second;
		if (IsSetBit(m_param.get_reward_flag, curr.seq))
		{
			continue;
		}

		SetBit(m_param.get_reward_flag, curr.seq);

		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, curr.rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_unfetch_reward_title, m_role_activity_type);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_unfetch_reward_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	m_param.is_buy = 0;
	this->SetRoleDataChange();
}

int RoleActivityYuanQiYanHuo::OnBuyItemCheck(long long ARG_OUT & unique_key)
{
	unique_key = 0;

	if (m_param.is_buy)
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityYuanQiYanHuoConfig * config = static_cast<const RandActivityYuanQiYanHuoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RandActivityYuanQiYanHuoOtherCfg & buy_cfg = config->GetOtherCfg();

	const std::vector<ItemConfigData> & rewards = buy_cfg.buy_rewards;
	if (rewards.empty())
	{
		return -__LINE__;
	}

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
	buy_item->need_gold = buy_cfg.need_chong_zhi_gold;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	::ItemConfigDataVecToBuyItem(rewards, buy_item->rewards_item, &buy_item->rewards_num);
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_yuan_qi_yan_huo, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_yuan_qi_yan_huo.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityYuanQiYanHuo::OnBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_yuan_qi_yan_huo.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, ::GetString(*buy_item).c_str());

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
		m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_RA_JIN_YI_TIAN_XIANG);
	}
	else
	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chong_zhi_buy_item_reward_title);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chong_zhi_buy_item_reward_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	m_param.is_buy = 1;
	this->SetRoleDataChange();
	this->SendInfo();

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_yuan_qi_yan_huo, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

int RoleActivityYuanQiYanHuo::OnBuyGiftItem(ARG_OUT int * price)
{
	if (m_param.is_buy)
	{
		return 2;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}

	const RandActivityYuanQiYanHuoConfig * config = static_cast<const RandActivityYuanQiYanHuoConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}
	const RandActivityYuanQiYanHuoOtherCfg & buy_cfg = config->GetOtherCfg();

	if (NULL != price)
	{
		*price = buy_cfg.need_chong_zhi_gold;
		return 0;
	}

	bool is_put_succ = false;
	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)buy_cfg.buy_rewards.size(), &buy_cfg.buy_rewards[0]))
	{
		if (m_role_module_mgr->GetKnapsack()->PutVec(buy_cfg.buy_rewards, PUT_REASON_RA_JIN_YI_TIAN_XIANG))
		{
			is_put_succ = true;
		}
	}
	if (!is_put_succ)
	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, buy_cfg.buy_rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chong_zhi_buy_item_reward_title);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chong_zhi_buy_item_reward_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	m_param.is_buy = 1;
	this->SetRoleDataChange();
	this->SendInfo();
	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

void RoleActivityYuanQiYanHuo::SendInfo()
{
	Protocol::SCRAYuanQiYanHuo protocol;
	protocol.is_buy = m_param.is_buy;
	protocol.is_fetch_free_reward = m_param.is_fetch_free_reward;
	protocol.get_reward_flag = m_param.get_reward_flag;

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}
