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
#include "protocol/randactivity/msgzuheheli.hpp"
#include "config/randactivityconfig/impl/randactivityzuheheliconfig.hpp"
#include "roleactivityzuheheli.hpp"

RoleActivityZuHeHeLi::RoleActivityZuHeHeLi(int type) : RoleActivity(type)
{
}

RoleActivityZuHeHeLi::~RoleActivityZuHeHeLi()
{
}

void RoleActivityZuHeHeLi::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityZuHeHeLi::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityZuHeHeLi::OnLoginSendInfo()
{
	this->SendInfo();
}

void RoleActivityZuHeHeLi::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.buy_times); ++i)
	{
		m_param.buy_times[i].ra_day_buy_times = 0;
	}

	this->SetRoleDataChange();
	this->SendInfo();
}

void RoleActivityZuHeHeLi::CheckInitRandActivity(int rand_act_type, bool is_force)
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

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());


		this->SetRoleDataChange();
	}
}

void RoleActivityZuHeHeLi::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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
				this->BuyItemByGold(raor->param_1); //// 组合贺礼 param1 能一次买多个 ，按位(bit),配置的type
			}
			break;
		case 2: // 组合贺礼 param1 能一次买多个 ，按位(bit),配置的type
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
		case 3: // 取消上一次的订单
			{
				this->Cancel();
			}
			break;
	}
}

bool RoleActivityZuHeHeLi::OnBuyItem(unsigned int buy_flag, int chong_zhi_gold, time_t buy_time, int ARG_OUT & err_num)
{
	std::set<int> buy_seq;
	int buy_num = 0;
	for (int i = 0; i < (int)sizeof(buy_flag) * 8; ++i)
	{
		if (buy_flag & (1 << i))
		{
			buy_seq.insert(i);
			buy_num += 1;
		}
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		err_num = -__LINE__;
		return false;
	}

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		err_num = -__LINE__;
		return false;
	}

	const RandActivityZuHeHeLiConfig * config = static_cast<const RandActivityZuHeHeLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		err_num = -__LINE__;
		return false;
	}

	int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDayByTime(ramgr, m_role_activity_type, buy_time);

	std::vector<ItemConfigData> rewards;
	int need_gold = 0;
	int phase = -1;
	for (std::set<int>::iterator iter = buy_seq.begin(); iter != buy_seq.end(); ++iter)
	{
		int type = *iter;
		const RAZuHeHeLiItemCfg * buy_cfg = config->GetRAZuHeHeLiItemCfg(ramgr, type);
		if (buy_cfg == NULL)
		{
			err_num = -__LINE__;
			return false;
		}

		if (!(buy_cfg->min_nuy_day <= open_day && open_day <= buy_cfg->max_buy_day))
		{
			err_num = -__LINE__;
			return false;
		}

		if (type < 0 || type >= RA_ZU_HE_HE_LI_MAX_ITEM)
		{
			err_num = -__LINE__;
			return false;
		}

		if (phase != -1 && phase != buy_cfg->phase)
		{
			err_num = -__LINE__;
			return false;
		}
		phase = buy_cfg->phase;

		if (!buy_cfg->is_chong_zhi)
		{
			err_num = -__LINE__;
			return false;
		}

		if (buy_cfg->limit_type == 1)
		{
			if (m_param.buy_times[type].ra_buy_times >= buy_cfg->limit_buy_times)
			{
				err_num = -__LINE__;
				return false;
			}
		}

		if (buy_cfg->limit_type == 2)
		{
			if (m_param.buy_times[type].ra_day_buy_times >= buy_cfg->limit_buy_times_day)
			{
				err_num = -__LINE__;
				return false;
			}
		}

		need_gold += buy_cfg->need_chong_zhi_gold;
		rewards.push_back(buy_cfg->reward);
	}

	if (rewards.empty())
	{
		err_num = -__LINE__;
		return false;
	}

	need_gold = (int)(need_gold * config->GetDiscount(phase, buy_num));
	if (chong_zhi_gold < need_gold - ZU_HE_HE_LI_WU_CHA || chong_zhi_gold > need_gold + ZU_HE_HE_LI_WU_CHA)			//上面有折扣是double，允许误差。客户端是人民币元取整
	{
		err_num = -__LINE__;
		return false;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)rewards.size(), &rewards[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_ZU_HE_HE_LI);
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

	//前面奖励已经发了，后面不能返回false了

	for (std::set<int>::iterator iter = buy_seq.begin(); iter != buy_seq.end(); ++iter)
	{
		int type = *iter;

		if (type < 0 || type >= RA_ZU_HE_HE_LI_MAX_ITEM)
		{
			continue;
		}

		m_param.buy_times[type].ra_buy_times += 1;
		m_param.buy_times[type].ra_day_buy_times += 1;
	}

	this->SendInfo();
	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);
	return true;
}

int RoleActivityZuHeHeLi::OnBuyGiftItem(int buy_seq1, int buy_seq2, ARG_OUT int * price)
{
	std::set<int> buy_seq;
	buy_seq.insert(buy_seq1);
	buy_seq.insert(buy_seq2);

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return 1;
	}

	const RandActivityZuHeHeLiConfig * config = static_cast<const RandActivityZuHeHeLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}

	std::vector<ItemConfigData> rewards;
	std::set<int> buy_set;
	int need_gold = 0;
	int phase = -1;
	for (std::set<int>::iterator iter = buy_seq.begin(); iter != buy_seq.end(); ++iter)
	{
		int seq = *iter;
		const RAZuHeHeLiItemCfg * buy_cfg = config->GetRAZuHeHeLiItemCfgBySeq(ramgr, seq);
		if (NULL == buy_cfg)
		{
			continue;
		}

		if (phase != -1 && phase != buy_cfg->phase)
		{
			return 1;
		}
		phase = buy_cfg->phase;

		if (!buy_cfg->is_chong_zhi)
		{
			return 1;
		}

		if (buy_cfg->limit_type == 1)
		{
			if (m_param.buy_times[buy_cfg->type].ra_buy_times >= buy_cfg->limit_buy_times)
			{
				return 2;
			}
		}

		if (buy_cfg->limit_type == 2)
		{
			if (m_param.buy_times[buy_cfg->type].ra_day_buy_times >= buy_cfg->limit_buy_times_day)
			{
				return 2;
			}
		}
		need_gold += buy_cfg->need_chong_zhi_gold;
		rewards.push_back(buy_cfg->reward);
		buy_set.insert(buy_cfg->type);
	}
	need_gold = (int)(need_gold * config->GetDiscount(phase, (int)buy_seq.size()));
	if (NULL != price)
	{
		*price = need_gold;
		return 0;
	}
	
	if (rewards.empty())
	{
		return 1;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)rewards.size(), &rewards[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_ZU_HE_HE_LI);
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

	//前面奖励已经发了，后面不能返回false了

	for (std::set<int>::iterator iter = buy_set.begin(); iter != buy_set.end(); ++iter)
	{
		int type = *iter;

		if (type < 0 || type >= RA_ZU_HE_HE_LI_MAX_ITEM)
		{
			continue;
		}

		m_param.buy_times[type].ra_buy_times += 1;
		m_param.buy_times[type].ra_day_buy_times += 1;
	}

	this->SendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);
	return 0;
}

int RoleActivityZuHeHeLi::BuyItemByGold(int buy_flag)
{
	time_t buy_time = EngineAdapter::Instance().Time();

	std::set<int> buy_seq;
	int buy_num = 0;
	for (int i = 0; i < (int)sizeof(buy_flag) * 8; ++i)
	{
		if (buy_flag & (1 << i))
		{
			buy_seq.insert(i);
			buy_num += 1;
		}
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return -__LINE__;
	}

	const RandActivityZuHeHeLiConfig * config = static_cast<const RandActivityZuHeHeLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDayByTime(ramgr, m_role_activity_type, buy_time);

	std::vector<ItemConfigData> rewards;
	int need_gold = 0;
	int phase = -1;
	for (std::set<int>::iterator iter = buy_seq.begin(); iter != buy_seq.end(); ++iter)
	{
		int type = *iter;
		const RAZuHeHeLiItemCfg * buy_cfg = config->GetRAZuHeHeLiItemCfg(ramgr, type);
		if (buy_cfg == NULL)
		{
			return -__LINE__;
		}

		if (!(buy_cfg->min_nuy_day <= open_day && open_day <= buy_cfg->max_buy_day))
		{
			return -__LINE__;
		}

		if (type < 0 || type >= RA_ZU_HE_HE_LI_MAX_ITEM)
		{
			return -__LINE__;
		}

		if (phase != -1 && phase != buy_cfg->phase)
		{
			return -__LINE__;
		}
		phase = buy_cfg->phase;

		if (buy_cfg->is_chong_zhi)
		{
			return -__LINE__;
		}

		if (buy_cfg->limit_type == 1)
		{
			if (m_param.buy_times[type].ra_buy_times >= buy_cfg->limit_buy_times)
			{
				return -__LINE__;
			}
		}

		if (buy_cfg->limit_type == 2)
		{
			if (m_param.buy_times[type].ra_day_buy_times >= buy_cfg->limit_buy_times_day)
			{
				return -__LINE__;
			}
		}

		need_gold += buy_cfg->need_chong_zhi_gold;
		rewards.push_back(buy_cfg->reward);
	}

	if (rewards.empty())
	{
		return -__LINE__;
	}

	need_gold = (int)(need_gold * config->GetDiscount(phase, buy_num));
	if (!m_role_module_mgr->GetMoney()->UseGold(need_gold, __FUNCTION__))
	{
		return -__LINE__;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)rewards.size(), &rewards[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_ZU_HE_HE_LI);
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

	//前面奖励已经发了，后面不能返回false了

	for (std::set<int>::iterator iter = buy_seq.begin(); iter != buy_seq.end(); ++iter)
	{
		int type = *iter;

		if (type < 0 || type >= RA_ZU_HE_HE_LI_MAX_ITEM)
		{
			continue;
		}

		m_param.buy_times[type].ra_buy_times += 1;
		m_param.buy_times[type].ra_day_buy_times += 1;
	}

	this->SendInfo();
	this->SetRoleDataChange();

	return 0;
}

int RoleActivityZuHeHeLi::OnBuyItemCheck(unsigned int buy_flag, long long ARG_OUT & unique_key)
{
	unique_key = 0;

	std::set<int> buy_seq;
	int buy_num = 0;
	time_t buy_time = EngineAdapter::Instance().Time();
	for (int i = 0; i < (int)sizeof(buy_flag) * 8; ++i)
	{
		if (buy_flag & (1 << i))
		{
			buy_seq.insert(i);
			buy_num += 1;
		}
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return -__LINE__;
	}

	const RandActivityZuHeHeLiConfig * config = static_cast<const RandActivityZuHeHeLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDayByTime(ramgr, m_role_activity_type, buy_time);

	std::vector<ItemConfigData> rewards;
	int need_gold = 0;
	int phase = -1;
	for (std::set<int>::iterator iter = buy_seq.begin(); iter != buy_seq.end(); ++iter)
	{
		int type = *iter;
		const RAZuHeHeLiItemCfg * buy_cfg = config->GetRAZuHeHeLiItemCfg(ramgr, type);
		if (buy_cfg == NULL)
		{
			return -__LINE__;
		}

		if (!(buy_cfg->min_nuy_day <= open_day && open_day <= buy_cfg->max_buy_day))
		{
			return -__LINE__;
		}

		if (type < 0 || type >= RA_ZU_HE_HE_LI_MAX_ITEM)
		{
			return -__LINE__;
		}

		if (phase != -1 && phase != buy_cfg->phase)
		{
			return -__LINE__;
		}
		phase = buy_cfg->phase;

		if (!buy_cfg->is_chong_zhi)
		{
			return -__LINE__;
		}

		if (buy_cfg->limit_type == 1)
		{
			if (m_param.buy_times[type].ra_buy_times >= buy_cfg->limit_buy_times)
			{
				return -__LINE__;
			}
		}

		if (buy_cfg->limit_type == 2)
		{
			if (m_param.buy_times[type].ra_day_buy_times >= buy_cfg->limit_buy_times_day)
			{
				return -__LINE__;
			}
		}

		need_gold += buy_cfg->need_chong_zhi_gold;
		rewards.push_back(buy_cfg->reward);
	}

	need_gold = (int)(need_gold * config->GetDiscount(phase, buy_num));

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
	buy_item->need_gold = need_gold;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	buy_item->param1 = buy_flag;
	::ItemConfigDataVecToBuyItem(rewards, buy_item->rewards_item, &buy_item->rewards_num);
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_zu_he_he_li, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_zu_he_he_li.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());
	
	return 0;
}

bool RoleActivityZuHeHeLi::OnBuyItem2(int chong_zhi_gold, long long unique_id)
{
	int err_num = this->OnBuyItem2Helper(chong_zhi_gold, unique_id);
	if (err_num)
	{
		gamelog::g_log_zu_he_he_li.printf(LL_INFO, "%s line:%d |  role[%d %s] arg_gold:%d arg_unique_id:%lld err_ret:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
						  chong_zhi_gold, unique_id, err_num);
	}

	return err_num == 0;
}

int RoleActivityZuHeHeLi::Cancel()
{
	return 0;
}

void RoleActivityZuHeHeLi::SendInfo()
{
	Protocol::SCRAZuHeHeLiInfo protocol;

	for (int i = 0; i < RA_ZU_HE_HE_LI_MAX_ITEM; ++i)
	{
		protocol.ra_buy_times[i] = m_param.buy_times[i].ra_buy_times;
		protocol.ra_day_buy_times[i] = m_param.buy_times[i].ra_day_buy_times;
	}

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

int RoleActivityZuHeHeLi::OnBuyItem2Helper(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_zu_he_he_li.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%ud %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
					  m_param.ra_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	unsigned int buy_flag = buy_item->param1;
	if (chong_zhi_gold < buy_item->need_gold - ZU_HE_HE_LI_WU_CHA || chong_zhi_gold > buy_item->need_gold + ZU_HE_HE_LI_WU_CHA)		//上面有折扣是double，允许误差。客户端是人民币元取整
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
		m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_ZU_HE_HE_LI);
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
		std::set<int> buy_seq;
		int buy_num = 0;
		time_t buy_time = EngineAdapter::Instance().Time();
		for (int i = 0; i < (int)sizeof(buy_flag) * 8; ++i)
		{
			if (buy_flag & (1 << i))
			{
				buy_seq.insert(i);
				buy_num += 1;
			}
		}

		for (std::set<int>::iterator iter = buy_seq.begin(); iter != buy_seq.end(); ++iter)
		{
			int type = *iter;

			if (type < 0 || type >= RA_ZU_HE_HE_LI_MAX_ITEM)
			{
				continue;
			}

			m_param.buy_times[type].ra_buy_times += 1;
			m_param.buy_times[type].ra_day_buy_times += 1;

			gamelog::g_log_zu_he_he_li.printf(LL_INFO, "%s line:%d | role[%d %s] type:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), type);
		}

		this->SendInfo();
		this->SetRoleDataChange();
	}

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_zu_he_he_li, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);
	return 0;
}