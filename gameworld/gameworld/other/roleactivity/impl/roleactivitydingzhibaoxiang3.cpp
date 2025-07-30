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
#include "protocol/randactivity/msgradingzhibaoxiang3.hpp"
#include "config/randactivityconfig/impl/randactivitydingzhibaoxiang3config.hpp"
#include "roleactivitydingzhibaoxiang3.hpp"


RoleActivityDingZhiBaoXiang3::RoleActivityDingZhiBaoXiang3(int type) :RoleActivity(type)
{
}

RoleActivityDingZhiBaoXiang3::~RoleActivityDingZhiBaoXiang3()
{
}

void RoleActivityDingZhiBaoXiang3::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityDingZhiBaoXiang3::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityDingZhiBaoXiang3::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleActivityDingZhiBaoXiang3::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.ra_ding_zhi_bao_xiang_day_buy_times); ++i)
	{
		m_param.ra_ding_zhi_bao_xiang_day_buy_times[i] = 0;
	}

	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityDingZhiBaoXiang3::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
	{
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity * activity = ramgr->ForceGetRandActivity(rand_act_type);
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_ding_zhi_bao_xiang_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_ding_zhi_bao_xiang_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->OnCheckInitBonus();

		this->SetRoleDataChange();
	}
}

void RoleActivityDingZhiBaoXiang3::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
				int ret_num = this->OnBuyItemCheck(raor->param_1, raor->param_2, unique_id);	//购买定制宝箱	 param1 配置seq，param3 选择的宝箱按位来

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
				this->ReceiveFreeReward();
			}
			break;
	}
}

bool RoleActivityDingZhiBaoXiang3::OnBuyItemOld(int seq, int param1, int chong_zhi_gold)
{
	if (seq < 0 || seq >= RA_DING_ZHI_BAO_XIANG_3_MAX_ITEM)
	{
		return false;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return false;
	}

	const RandActivityDingZhiBaoXiang3Config * config = static_cast<const RandActivityDingZhiBaoXiang3Config *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return false;
	}

	const RADingZhiBaoXiang3ItemCfg * buy_cfg = config->GetRADingZhiBaoXiangItemCfg(ramgr, seq);
	if (buy_cfg == NULL)
	{
		return false;
	}

	if (buy_cfg->need_chong_zhi_gold != chong_zhi_gold)
	{
		return false;
	}

	std::set<int> select_seq;
	for (int i = 0; i < (int)sizeof(param1) * 8; ++i)
	{
		if (param1 & (1 << i))
		{
			select_seq.insert(i);
		}
	}

	if (select_seq.size() != buy_cfg->reward_select_num)
	{
		return false;
	}

	const RADingZhiBaoXiang3RewardGroup * reward_group_cfg = config->GetRADingZhiBaoXiangRewardGroup(buy_cfg->reward_group_id);
	if (reward_group_cfg == NULL)
	{
		return false;
	}

	std::vector<ItemConfigData> rewards;
	for (std::set<int>::iterator iter = select_seq.begin(); iter != select_seq.end(); ++iter)
	{
		int index = *iter;
		if (index < 0 || index >= (int)reward_group_cfg->rewards.size())
		{
			return false;
		}

		rewards.push_back(reward_group_cfg->rewards[index]);
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
	{
		if (m_param.ra_ding_zhi_bao_xiang_buy_times[seq] >= buy_cfg->limit_buy_times)
		{
			return false;
		}
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
	{
		if (m_param.ra_ding_zhi_bao_xiang_day_buy_times[seq] >= buy_cfg->limit_buy_times_day)
		{
			return false;
		}
	}

	if (rewards.empty())
	{
		return false;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)rewards.size(), &rewards[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_DING_ZHI_BAO_XIANG);
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

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
	{
		m_param.ra_ding_zhi_bao_xiang_buy_times[seq] +=1;
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
	{
		m_param.ra_ding_zhi_bao_xiang_day_buy_times[seq] += 1;
	}

	this->SendInfo();

	this->SetRoleDataChange();

	return true;
}

int RoleActivityDingZhiBaoXiang3::OnBuyGiftItem(int seq, ARG_OUT int * price)
{
	if (seq < 0 || seq >= RA_DING_ZHI_BAO_XIANG_3_MAX_ITEM)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}
	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	const RandActivityDingZhiBaoXiang3Config * config = static_cast<const RandActivityDingZhiBaoXiang3Config *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	const RADingZhiBaoXiang3ItemCfg * buy_cfg = config->GetRADingZhiBaoXiangItemCfg(ramgr, seq);
	if (buy_cfg == NULL)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}
	if (NULL != price)
	{
		*price = buy_cfg->need_chong_zhi_gold;
		return 0;
	}
	const RADingZhiBaoXiang3RewardGroup * reward_group_cfg = config->GetRADingZhiBaoXiangRewardGroup(buy_cfg->reward_group_id);
	if (reward_group_cfg == NULL)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
	{
		if (m_param.ra_ding_zhi_bao_xiang_buy_times[seq] >= buy_cfg->limit_buy_times)
		{
			return BUY_GIFT_ITEM_RET_TYPE_2;
		}
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
	{
		if (m_param.ra_ding_zhi_bao_xiang_day_buy_times[seq] >= buy_cfg->limit_buy_times_day)
		{
			return BUY_GIFT_ITEM_RET_TYPE_2;
		}
	}

	if (reward_group_cfg->rewards.empty())
	{
		return 1;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)reward_group_cfg->rewards.size(), &reward_group_cfg->rewards[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)reward_group_cfg->rewards.size(), &reward_group_cfg->rewards[0], PUT_REASON_DING_ZHI_BAO_XIANG);
	}
	else
	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, reward_group_cfg->rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chong_zhi_buy_item_reward_title);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chong_zhi_buy_item_reward_content, m_role_activity_type);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
	{
		m_param.ra_ding_zhi_bao_xiang_buy_times[seq] += 1;
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
	{
		m_param.ra_ding_zhi_bao_xiang_day_buy_times[seq] += 1;
	}

	this->SendInfo();
	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

int RoleActivityDingZhiBaoXiang3::OnBuyItemCheck(int seq, int param1, long long ARG_OUT & unique_key)
{
	unique_key = 0;

	if (seq < 0 || seq >= RA_DING_ZHI_BAO_XIANG_3_MAX_ITEM)
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityDingZhiBaoXiang3Config * config = static_cast<const RandActivityDingZhiBaoXiang3Config *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RADingZhiBaoXiang3ItemCfg * buy_cfg = config->GetRADingZhiBaoXiangItemCfg(ramgr, seq);
	if (buy_cfg == NULL)
	{
		return -__LINE__;
	}

	std::set<int> select_seq;
	for (int i = 0; i < (int)sizeof(param1) * 8; ++i)
	{
		if (param1 & (1 << i))
		{
			select_seq.insert(i);
		}
	}

	if (select_seq.size() != buy_cfg->reward_select_num)
	{
		return -__LINE__;
	}

	const RADingZhiBaoXiang3RewardGroup * reward_group_cfg = config->GetRADingZhiBaoXiangRewardGroup(buy_cfg->reward_group_id);
	if (reward_group_cfg == NULL)
	{
		return -__LINE__;
	}

	std::vector<ItemConfigData> rewards;
	for (std::set<int>::iterator iter = select_seq.begin(); iter != select_seq.end(); ++iter)
	{
		int index = *iter;
		if (index < 0 || index >= (int)reward_group_cfg->rewards.size())
		{
			return -__LINE__;
		}

		rewards.push_back(reward_group_cfg->rewards[index]);
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
	{
		if (m_param.ra_ding_zhi_bao_xiang_buy_times[seq] >= buy_cfg->limit_buy_times)
		{
			return -__LINE__;
		}
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
	{
		if (m_param.ra_ding_zhi_bao_xiang_day_buy_times[seq] >= buy_cfg->limit_buy_times_day)
		{
			return -__LINE__;
		}
	}

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
	buy_item->need_gold = buy_cfg->need_chong_zhi_gold;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_ding_zhi_bao_xiang_begin_timestamp;
	buy_item->unique_id = unique_key;
	buy_item->param1 = seq;
	buy_item->param2 = param1;
	buy_item->param3 = buy_cfg->limit_type;
	::ItemConfigDataVecToBuyItem(rewards, buy_item->rewards_item, &buy_item->rewards_num);
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_ding_zhi_bao_xiang_3, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str3 = ::GetString(*buy_item);
	gamelog::g_log_ding_zhi_bao_xiang_3.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str3.c_str());

	return 0;
}

int RoleActivityDingZhiBaoXiang3::OnBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_ding_zhi_bao_xiang_3.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
					      m_param.ra_ding_zhi_bao_xiang_begin_timestamp, ::GetString(*buy_item).c_str());

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
		m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_DING_ZHI_BAO_XIANG);
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

	int seq = buy_item->param1;
	int param = buy_item->param2;
	int limit_type = buy_item->param3;


	//如果时间相等就说明同一次活动里购买，要增加次数。
	if (buy_item->ra_begin_timestamp == m_param.ra_ding_zhi_bao_xiang_begin_timestamp)
	{
		if (0 <= seq && seq < RA_DING_ZHI_BAO_XIANG_3_MAX_ITEM)
		{
			if (limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
			{
				m_param.ra_ding_zhi_bao_xiang_buy_times[seq] += 1;
			}

			if (limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
			{
				m_param.ra_ding_zhi_bao_xiang_day_buy_times[seq] += 1;
			}
		}

		this->SendInfo();
		this->SetRoleDataChange();
	}

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_ding_zhi_bao_xiang_3, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

void RoleActivityDingZhiBaoXiang3::SendInfo()
{
	Protocol::SCRADingZhiBaoXiang3Info protocol;
	for (int i = 0; i < RA_DING_ZHI_BAO_XIANG_3_MAX_ITEM; ++i)
	{
		protocol.ra_ding_zhi_bao_xiang_day_buy_times[i] = m_param.ra_ding_zhi_bao_xiang_day_buy_times[i];
		protocol.ra_ding_zhi_bao_xiang_buy_times[i] = m_param.ra_ding_zhi_bao_xiang_buy_times[i];
	}
	protocol.has_free_reward = m_param.has_free_reward;
	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

void RoleActivityDingZhiBaoXiang3::ReceiveFreeReward()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}

	if (m_param.has_free_reward != 0) return;

	const RandActivityDingZhiBaoXiang3Config * config = static_cast<const RandActivityDingZhiBaoXiang3Config *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	const RADingZhiBaoXiang3OtherCfg & other_cfg = config->GetOtherCfg();

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(other_cfg.free_reward))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->PutVec(other_cfg.free_reward, PUT_REASON_DING_ZHI_BAO_XIANG))
	{
		return;
	}

	m_param.has_free_reward = 1;

	std::string gamelog_str;
	for (int i = 0; i < (int)other_cfg.free_reward.size(); ++i)
	{
		gamelog_str = STRING_SPRINTF("{item[%d, %d]} ", other_cfg.free_reward[i].item_id, other_cfg.free_reward[i].num);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "::%s user[%d, %s] get_reward[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), gamelog_str.c_str());

	this->SendInfo();
	this->SetRoleDataChange();
}
