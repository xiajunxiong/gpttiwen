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
#include "protocol/randactivity/msgraduanyangfuli.hpp"
#include "config/randactivityconfig/impl/randactivityduanyangfuliconfig.hpp"
#include "roleactivityduanyangfuli.hpp"


RoleActivityDuanYangFuLi::RoleActivityDuanYangFuLi(int type) :RoleActivity(type)
{
}

RoleActivityDuanYangFuLi::~RoleActivityDuanYangFuLi()
{
}

void RoleActivityDuanYangFuLi::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityDuanYangFuLi::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityDuanYangFuLi::OnLoginSendInfo()
{
	this->SendInfo();
}


void RoleActivityDuanYangFuLi::CheckInitRandActivity(int rand_act_type, bool is_force)
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

void RoleActivityDuanYangFuLi::OnRAClose()
{
	this->GiveUnFetchReward();
}

void RoleActivityDuanYangFuLi::OnLoginCheck()
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

void RoleActivityDuanYangFuLi::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	switch (raor->opera_type)
	{
		case 0:  //请求下发
			{
				this->SendInfo();
			}
			break;
		case 1:	// p1 选的奖励reward_seq ,p2 是否购买所有
			{
				long long unique_id = 0;
				int ret_num = this->OnBuyItemCheck(raor->param_1, raor->param_2, unique_id);

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
		case 2:	//领取奖励 p1 选的奖励reward_seq
			{
				this->FetchReward(raor->param_1);		
			}
			break;
	}
}

int RoleActivityDuanYangFuLi::FetchReward(int seq)
{
	if (seq < 0 || seq >= ARRAY_LENGTH(m_param.item_list))
	{
		return -__LINE__;
	}

	RADuanYangFuLiItem & item = m_param.item_list[seq];
	if (item.is_buy == 0)
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityDuanYangFuLiConfig * config = static_cast<const RandActivityDuanYangFuLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	const RandActivityDuanYangFiLiCfg * cfg = config->GetRandActivityDuanYangFiLiCfg(ramgr, seq);
	if (!cfg)
	{
		return -__LINE__;
	}

	if (item.fetch_reward_times >= cfg->can_fetch_times)
	{
		return -__LINE__;
	}

	int act_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_DUAN_YANG_FU_LI);
	if (cfg->day > act_open_day)	
	{
		return -__LINE__;
	}

	//到活动开启天数后，每天能领取一次。之前没领取一次给领取完
	int real_can_fetch_times = std::min(cfg->can_fetch_times, act_open_day - cfg->day + 1);
	for (int i = item.fetch_reward_times; i < real_can_fetch_times; ++i)
	{
		const std::vector<ItemConfigData> & rewards = cfg->rewards;
		if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(rewards))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_HAS_NO_SPACING);
			break;
		}

		m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_RA_FU_LI);
		item.fetch_reward_times += 1;

		gamelog::g_duan_yang_fu_li.printf(LL_INFO, "%s line:%d |  role[%d %s] seq:%d times:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
			seq, item.fetch_reward_times);
	}

	this->SendInfo();
	this->SetRoleDataChange();

	return 0;
}

int RoleActivityDuanYangFuLi::OnBuyItemCheck(int seq, bool is_buy_all, long long ARG_OUT & unique_key)
{
	unique_key = 0;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	const RandActivityDuanYangFuLiConfig * config = static_cast<const RandActivityDuanYangFuLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	int need_chong_zhi_gold = 0;
	if (is_buy_all)
	{
		for (int i = 0; i < ARRAY_LENGTH(m_param.item_list); ++i)
		{
			if (m_param.item_list[i].is_buy != 0)
			{
				return -__LINE__;		//如果有一个购买了，就不能一键购买全部
			}
		}

		const RandActivityDuanYangFiLiOtherCfg & other_cfg = config->GetOtherCfg();
		need_chong_zhi_gold = other_cfg.all_buy_need_chong_zhi_gold;
	}
	else
	{
		if (seq < 0 || seq >= ARRAY_LENGTH(m_param.item_list))
		{
			return -__LINE__;
		}

		RADuanYangFuLiItem & item = m_param.item_list[seq];
		if (item.is_buy != 0)
		{
			return -__LINE__;
		}

		const RandActivityDuanYangFiLiCfg * cfg = config->GetRandActivityDuanYangFiLiCfg(ramgr, seq);
		if (!cfg)
		{
			return -__LINE__;
		}

		need_chong_zhi_gold = cfg->need_chong_zhi_gold;
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
	buy_item->need_gold = need_chong_zhi_gold;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->unique_id = unique_key;
	buy_item->param1 = seq;
	buy_item->param2 = is_buy_all;
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_duan_yang_fu_li, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_duan_yang_fu_li.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityDuanYangFuLi::OnBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_duan_yang_fu_li.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return  -__LINE__;
	}

	int seq = buy_item->param1;
	int is_buy_all = buy_item->param2;

	int need_chong_zhi_gold = 0;
	if (is_buy_all)
	{
		for (int i = 0; i < ARRAY_LENGTH(m_param.item_list); ++i)
		{
			m_param.item_list[i].is_buy = 1;
		}
	}
	else
	{
		if (seq < 0 || seq >= ARRAY_LENGTH(m_param.item_list))
		{
			return -__LINE__;
		}

		RADuanYangFuLiItem & item = m_param.item_list[seq];
		item.is_buy = 1;
	}

	this->SetRoleDataChange();
	this->SendInfo();

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_duan_yang_fu_li, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

int RoleActivityDuanYangFuLi::OnBuyGiftItem(int seq, bool is_buy_all, ARG_OUT int * price)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}

	const RandActivityDuanYangFuLiConfig * config = static_cast<const RandActivityDuanYangFuLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return 1;
	}

	int need_chong_zhi_gold = 0;
	if (is_buy_all)
	{
		for (int i = 0; i < ARRAY_LENGTH(m_param.item_list); ++i)
		{
			if (m_param.item_list[i].is_buy != 0)
			{
				return 2;		//如果有一个购买了，就不能一键购买全部
			}
		}

		const RandActivityDuanYangFiLiOtherCfg & other_cfg = config->GetOtherCfg();
		need_chong_zhi_gold = other_cfg.all_buy_need_chong_zhi_gold;
	}
	else
	{
		if (seq < 0 || seq >= ARRAY_LENGTH(m_param.item_list))
		{
			return 1;
		}

		RADuanYangFuLiItem & item = m_param.item_list[seq];
		if (item.is_buy != 0)
		{
			return 2;
		}

		const RandActivityDuanYangFiLiCfg * cfg = config->GetRandActivityDuanYangFiLiCfg(ramgr, seq);
		if (!cfg)
		{
			return 1;
		}

		need_chong_zhi_gold = cfg->need_chong_zhi_gold;
	}

	if (NULL != price)
	{
		*price = need_chong_zhi_gold;
		return 0;
	}

	if (is_buy_all)
	{
		for (int i = 0; i < ARRAY_LENGTH(m_param.item_list); ++i)
		{
			m_param.item_list[i].is_buy = 1;
		}
	}
	else
	{
		RADuanYangFuLiItem & item = m_param.item_list[seq];
		item.is_buy = 1;
	}

	this->SetRoleDataChange();
	this->SendInfo();

	this->OnParticipateRA(__FUNCTION__);
	return 0;
}

void RoleActivityDuanYangFuLi::SendInfo()
{
	Protocol::SCRADuanYangFuLiInfo protocol;
	SameArrayTypeCopy(protocol.item_list, m_param.item_list);

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

void RoleActivityDuanYangFuLi::GiveUnFetchReward()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityDuanYangFuLiConfig * config = static_cast<const RandActivityDuanYangFuLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	const RandActivityDuanYangFuLiSectionCfg * cfg = config->GetRandActivityDuanYangFiLiSectionCfg(ramgr, m_param.ra_begin_timestamp);
	if (!cfg)
	{
		return;
	}

	for (std::map<int, RandActivityDuanYangFiLiCfg> ::const_iterator iter = cfg->item_list.begin(); iter != cfg->item_list.end(); ++iter)
	{
		const RandActivityDuanYangFiLiCfg & curr = iter->second;
		int seq = curr.seq;

		if (seq < 0 || seq >= ARRAY_LENGTH(m_param.item_list))
		{
			continue;
		}

		RADuanYangFuLiItem & item = m_param.item_list[seq];
		if (item.is_buy == 0)
		{
			continue;
		}

		item.is_buy = 0;

		while (item.fetch_reward_times < curr.can_fetch_times)
		{
			// 发邮件
			static MailContentParam contentparam; contentparam.Reset();
			FillMailContentParam(contentparam, curr.rewards);

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_unfetch_reward_title, m_role_activity_type);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_unfetch_reward_content, m_role_activity_type);
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
			}

			item.fetch_reward_times += 1;

			gamelog::g_duan_yang_fu_li.printf(LL_INFO, "%s line:%d |  role[%d %s] seq:%d times:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
				seq, item.fetch_reward_times);
		}		
	}

	this->SetRoleDataChange();
}

