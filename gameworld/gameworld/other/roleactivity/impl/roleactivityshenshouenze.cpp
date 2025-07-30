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
#include "protocol/randactivity/msgrashenshouenze.hpp"
#include "config/randactivityconfig/impl/randactivityshenshouenzeconfig.hpp"
#include "roleactivityshenshouenze.hpp"

RoleActivityShenShouEnZe::RoleActivityShenShouEnZe(int type) :RoleActivity(type)
{
}

RoleActivityShenShouEnZe::~RoleActivityShenShouEnZe()
{
}

void RoleActivityShenShouEnZe::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityShenShouEnZe::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityShenShouEnZe::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleActivityShenShouEnZe::CheckInitRandActivity(int rand_act_type, bool is_force)
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

		this->SetRoleDataChange();
	}
}

void RoleActivityShenShouEnZe::OnRAClose()
{
	this->GiveUnFetchReward();
}

void RoleActivityShenShouEnZe::OnLoginCheck()
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

void RoleActivityShenShouEnZe::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.ra_day_buy_times); ++i)
	{
		m_param.ra_day_buy_times[i] = 0;
	}

	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityShenShouEnZe::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	switch (raor->opera_type)
	{
		case 0:  //请求下发
			{
				this->SendInfo();
			}
			break;
		case 1:	//购买 p1 奖励reward_seq 直购
			{
				long long unique_id = 0;
				int ret_num = this->OnBuyItemCheck(raor->param_1, unique_id);

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
		case 2:	//购买 p1 奖励reward_seq 元宝，灵玉
			{
				this->OnBuyItemByMoneyType(raor->param_1);
			}
			break;
		case 3:	//领取设置奖励
			{
				this->FetchTimesReward();
			}
			break;
		case 4:	//设置自选  p1 选的奖励reward_seq
			{
				this->SetReward(raor->param_1);
			}
			break;
		case 5:	//抽奖 p1 次数
			{
				this->DrwardReward(raor->param_1);
			}
			break;
		case 6:	//抽奖记录
			{
				this->SendRecordInfo();
			}
			break;
		case 7:	//重置选择的奖励
			{
				this->ClearSetReward();
			}
			break;
	}
}

int RoleActivityShenShouEnZe::FetchTimesReward()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		ret_line;
	}

	const RandActivityShenShouEnZeConfig * config = static_cast<const RandActivityShenShouEnZeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		ret_line;
	}


	const RandActivityShenShouEnZeSelectRewardCfg * select_cfg = config->GetRandActivityShenShouEnZeSelectRewardCfg(ramgr, m_param.select_reward_seq);
	if (!select_cfg)
	{
		ret_line;
	}

	if (m_param.draw_times < select_cfg->need_draw_times)
	{
		ret_line;
	}


	const std::vector<ItemConfigData> & rewards = select_cfg->rewards;
	if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(rewards))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HAS_NO_SPACING);
		return -__LINE__;
	}

	m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_RA_SHEN_SHOU_EN_ZE_FETCH);
	this->AddRecord(rewards, select_cfg->is_broadcast);

	gamelog::g_log_ra_shen_shou_en_ze.printf(LL_INFO, "%s line:%d | role[%d %s] reward_seq:%d draw_times:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		select_cfg->seq, m_param.draw_times);

	m_param.is_select = 0;
	m_param.select_reward_seq = 0;
	m_param.draw_times = 0;

	this->SendInfo();
	this->SetRoleDataChange();

	return 0;
}

int RoleActivityShenShouEnZe::SetReward(int reward_seq)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		ret_line;
	}

	const RandActivityShenShouEnZeConfig * config = static_cast<const RandActivityShenShouEnZeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		ret_line;
	}

	if (m_param.is_select != 0)
	{
		const RandActivityShenShouEnZeSelectRewardCfg * select_cfg = config->GetRandActivityShenShouEnZeSelectRewardCfg(ramgr, m_param.select_reward_seq);
		if (select_cfg)
		{
			ret_line;
		}
	}

	const RandActivityShenShouEnZeSelectRewardCfg * select_cfg_2 = config->GetRandActivityShenShouEnZeSelectRewardCfg(ramgr, reward_seq);
	if (!select_cfg_2)
	{
		ret_line;
	}

	m_param.is_select = 1;
	m_param.select_reward_seq = reward_seq;
	m_param.draw_times = 0;

	this->SendInfo();
	this->SetRoleDataChange();

	return 0;
}

int RoleActivityShenShouEnZe::ClearSetReward()
{
	gamelog::g_log_ra_shen_shou_en_ze.printf(LL_INFO, "%s line:%d | role[%d %s] is_select:%d select_reward_seq:%d draw_times:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.is_select, m_param.select_reward_seq, m_param.draw_times);

	m_param.is_select = 0;
	m_param.select_reward_seq = 0;
	m_param.draw_times = 0;

	this->SendInfo();
	this->SetRoleDataChange();

	return 0;
}

int RoleActivityShenShouEnZe::DrwardReward(int times)
{
	if (times > 10)
	{
		ret_line;
	}

	if (m_param.is_select == 0)
	{
		ret_line;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		ret_line;
	}

	const RandActivityShenShouEnZeConfig * config = static_cast<const RandActivityShenShouEnZeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		ret_line;
	}

	const RandActivityShenShouEnZeSelectRewardCfg * select_cfg = config->GetRandActivityShenShouEnZeSelectRewardCfg(ramgr, m_param.select_reward_seq);
	if (!select_cfg)
	{
		ret_line;
	}

	if (m_param.draw_times + times > select_cfg->need_draw_times)
	{
		ret_line;
	}

	const RandActivityShenShouEnZeOtherCfg & other_cfg = config->GetOtherCfg();
	int draw_need_item_num = other_cfg.draw_need_item_num * times;
	if (!m_role_module_mgr->GetKnapsack()->HasItem(other_cfg.draw_need_item_id, draw_need_item_num))
	{
		ret_line;
	}

	const RandActivityShenShouEnZeSectionCfg * section_cfg = config->GetRandActivityShenShouEnZeSectionCfg(ramgr);
	if (!section_cfg)
	{
		ret_line;
	}

	std::vector<const RandActivityShenShouEnZeCfg *> draw_rewards_list;
	for (int curr_times = 0; curr_times < times; ++curr_times)
	{
		int r = RandomNum(section_cfg->rate_count);

		for (int i = 0; i < (int)section_cfg->item_list.size(); ++i)
		{
			const RandActivityShenShouEnZeCfg & curr = section_cfg->item_list[i];
			if (r < curr.rate)
			{
				draw_rewards_list.push_back(&curr);
				break;
			}

			r -= curr.rate;
		}
	}

	if ((int)draw_rewards_list.size() != times)
	{
		ret_line;
	}

	m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.draw_need_item_id, draw_need_item_num, __FUNCTION__);

	int log_draw_times = m_param.draw_times;
	m_param.draw_times += times;

	Protocol::SCRAShenShouEnZeDrawResult draw_ret;
	draw_ret.draw_times = m_param.draw_times;
	for (int i = 0; i < (int)draw_rewards_list.size(); ++i)
	{
		const RandActivityShenShouEnZeCfg * curr = draw_rewards_list[i];

		if (!curr)
		{
			continue;
		}

		int is_mail = 0;
		const std::vector<ItemConfigData> & rewards = curr->rewards;
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(rewards))
		{
			m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_RA_SHEN_SHOU_EN_ZE);
		}
		else
		{
			is_mail = 1;

			m_role_module_mgr->NoticeNum(errornum::EN_KNASACK_NO_SPACE_SEND_MAIL);
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

		this->AddRecord(rewards, curr->is_broadcast);

		if (i < ARRAY_LENGTH(draw_ret.reward_seq_list))
		{
			draw_ret.reward_seq_list[i].is_mail = is_mail;
			draw_ret.reward_seq_list[i].reward_seq= curr->seq;
		}

		gamelog::g_log_ra_shen_shou_en_ze.printf(LL_INFO, "%s line:%d | role[%d %s] select_seq:%d reward_seq:%d draw_times:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
			m_param.select_reward_seq, curr->seq, ++log_draw_times);
	}


	m_role_module_mgr->NetSend(&draw_ret, sizeof(draw_ret));

	return 0;
}

int RoleActivityShenShouEnZe::OnBuyItemCheck(int seq, long long ARG_OUT & unique_key)
{
	unique_key = 0;

	if (seq < 0 || seq >= SHEN_SHOU_EN_ZE_MAX_BUY_ITEM)
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		ret_line;
	}

	const RandActivityShenShouEnZeConfig * config = static_cast<const RandActivityShenShouEnZeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		ret_line;
	}

	const RandActivityShenShouEnZeBuyItemCfg * buy_cfg = config->GetRandActivityShenShouEnZeBuyItemCfg(ramgr, seq);
	if (!buy_cfg)
	{
		ret_line;
	}

	if (!buy_cfg->is_need_chong_zhi)
	{
		ret_line;
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
	{
		if (m_param.ra_buy_times[seq] >= buy_cfg->limit_buy_times)
		{
			return -__LINE__;
		}
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
	{
		if (m_param.ra_day_buy_times[seq] >= buy_cfg->limit_buy_times_day)
		{
			return -__LINE__;
		}
	}

	if (buy_cfg->rewards.empty())
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
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	buy_item->param1 = seq;
	buy_item->param2 = buy_cfg->limit_type;
	::ItemConfigDataVecToBuyItem(buy_cfg->rewards, buy_item->rewards_item, &buy_item->rewards_num);
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_ra_shen_shou_en_ze, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_ra_shen_shou_en_ze.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityShenShouEnZe::OnBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_ra_shen_shou_en_ze.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
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

	int seq = buy_item->param1;
	int limit_type = buy_item->param2;


	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(rewards))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_RA_SHEN_SHOU_EN_ZE_BUY);
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

	//如果时间相等就说明同一次活动里购买，要增加次数。
	if (buy_item->ra_begin_timestamp == m_param.ra_begin_timestamp)
	{
		if (0 <= seq && seq < SHEN_SHOU_EN_ZE_MAX_BUY_ITEM)
		{
			if (limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
			{
				m_param.ra_buy_times[seq] += 1;
			}

			if (limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
			{
				m_param.ra_day_buy_times[seq] += 1;
			}
		}

		this->SendInfo();
		this->SetRoleDataChange();
	}

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_ra_shen_shou_en_ze, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);
	return 0;
}

int RoleActivityShenShouEnZe::OnBuyItemByMoneyType(int seq)
{
	if (seq < 0 || seq >= SHEN_SHOU_EN_ZE_MAX_BUY_ITEM)
	{
		ret_line;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		ret_line;
	}

	const RandActivityShenShouEnZeConfig * config = static_cast<const RandActivityShenShouEnZeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		ret_line;
	}

	const RandActivityShenShouEnZeBuyItemCfg * buy_cfg = config->GetRandActivityShenShouEnZeBuyItemCfg(ramgr, seq);
	if (!buy_cfg)
	{
		ret_line;
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
	{
		if (m_param.ra_buy_times[seq] >= buy_cfg->limit_buy_times)
		{
			ret_line;
		}
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
	{
		if (m_param.ra_day_buy_times[seq] >= buy_cfg->limit_buy_times_day)
		{
			ret_line;
		}
	}

	if (buy_cfg->rewards.empty())
	{
		ret_line;
	}

	if (buy_cfg->is_need_chong_zhi)
	{
		ret_line;
	}

	if (!m_role_module_mgr->GetMoney()->MoneyTypeMoreThan(buy_cfg->money_type, buy_cfg->money_value))
	{
		ret_line;
	}

	const std::vector<ItemConfigData> & rewards = buy_cfg->rewards;
	if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(rewards))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HAS_NO_SPACING);
		ret_line;
	}

	if (!m_role_module_mgr->GetMoney()->UseMoney(buy_cfg->money_type, buy_cfg->money_value, __FUNCTION__))
	{
		ret_line;
	}

	m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_RA_SHEN_SHOU_EN_ZE_BUY);

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
	{
		m_param.ra_buy_times[seq] += 1;
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
	{
		m_param.ra_day_buy_times[seq] += 1;
	}

	gamelog::g_log_ra_shen_shou_en_ze.printf(LL_INFO, "%s line:%d |  role[%d %s] seq:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		seq);


	this->SendInfo();
	this->SetRoleDataChange();

	return 0;
}

int RoleActivityShenShouEnZe::OnBuyGiftItem(int seq, ARG_OUT int * price)
{
	if (seq < 0 || seq >= SHEN_SHOU_EN_ZE_MAX_BUY_ITEM)
	{
		return 1;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}

	const RandActivityShenShouEnZeConfig * config = static_cast<const RandActivityShenShouEnZeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}

	const RandActivityShenShouEnZeBuyItemCfg * buy_cfg = config->GetRandActivityShenShouEnZeBuyItemCfg(ramgr, seq);
	if (!buy_cfg)
	{
		return 1;
	}

	if (!buy_cfg->is_need_chong_zhi)
	{
		return 1;
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
	{
		if (m_param.ra_buy_times[seq] >= buy_cfg->limit_buy_times)
		{
			return 2;
		}
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
	{
		if (m_param.ra_day_buy_times[seq] >= buy_cfg->limit_buy_times_day)
		{
			return 2;
		}
	}

	if (NULL != price)
	{
		*price = buy_cfg->need_chong_zhi_gold;
		return 0;
	}
	
	bool is_put_succ = false;
	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(buy_cfg->rewards))
	{
		if (m_role_module_mgr->GetKnapsack()->PutVec(buy_cfg->rewards, PUT_REASON_RA_SHEN_SHOU_EN_ZE_BUY))
		{
			is_put_succ = true;
		}
	}

	if (!is_put_succ)
	{
		m_role_module_mgr->GetKnapsack()->SendMail(buy_cfg->rewards, SEND_MAIL_TYPE_ACTIVITY_BUY_BAG_FULL, false, m_role_activity_type);
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
	{
		m_param.ra_buy_times[seq] += 1;
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
	{
		m_param.ra_day_buy_times[seq] += 1;
	}

	this->SendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_ra_shen_shou_en_ze.printf(LL_INFO, "%s role[%d, %s] seq[%d]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
		seq);
	return 0;
}

void RoleActivityShenShouEnZe::SendInfo()
{
	Protocol::SCRAShenShouEnZeInfo protocol;
	SameArrayTypeCopy(protocol.ra_day_buy_times, m_param.ra_day_buy_times);
	SameArrayTypeCopy(protocol.ra_buy_times, m_param.ra_buy_times);
	protocol.is_select = m_param.is_select;					//是否选择了物品
	protocol.select_reward_seq = m_param.select_reward_seq;			//选择物品的seq
	protocol.draw_times = m_param.draw_times;

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

void RoleActivityShenShouEnZe::SendRecordInfo()
{
	Protocol::SCRAShenShouEnZeRecord protocol;
	SameArrayTypeCopy(protocol.record_list, m_param.record_list);

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

void RoleActivityShenShouEnZe::GiveUnFetchReward()
{
}

void RoleActivityShenShouEnZe::AddRecord(const std::vector<ItemConfigData> & rewards, bool is_broadcast)
{
	if (rewards.empty())
	{
		return;
	}

	const ItemConfigData & reward = rewards[0];

	int index = m_param.record_index % ARRAY_LENGTH(m_param.record_list);
	RAShenShouEnZeRecord record;
	record.item_id = reward.item_id;
	record.num = reward.num;
	record.time = (unsigned int)EngineAdapter::Instance().Time();
	record.times = m_param.draw_times;
	m_param.record_list[index] = record;
	m_param.record_index = index + 1;

	if (is_broadcast)
	{		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_randactivity_get_item_broadcast_3, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
			m_role_activity_type, reward.item_id, m_role_activity_type);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
		}
	}
}
