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
#include "protocol/randactivity/msgrazhenpinhangpu.hpp"
#include "config/randactivityconfig/impl/randactivityzhenpinshangpuconfig.hpp"
#include "roleactivityzhenpinshangpu.hpp"

RoleActivityZhenPinShangPu::RoleActivityZhenPinShangPu(int type) :RoleActivity(type)
{
}

RoleActivityZhenPinShangPu::~RoleActivityZhenPinShangPu()
{
}

void RoleActivityZhenPinShangPu::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(m_param));
}

void RoleActivityZhenPinShangPu::GetInitParam(RoleActivityRoleData * p)
{
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(p->base_data));
	memcpy(p->base_data, &m_param, sizeof(m_param));
}

void RoleActivityZhenPinShangPu::OnLoginSendInfo()
{
	this->SendInfo();
	this->SendOpenDayInfo();
}

void RoleActivityZhenPinShangPu::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.ra_day_buy_times); ++i)
	{
		m_param.ra_day_buy_times[i].Reset();
	}

	this->SendInfo();
	this->SendOpenDayInfo();
	this->SetRoleDataChange();
}

void RoleActivityZhenPinShangPu::CheckInitRandActivity(int rand_act_type, bool is_force)
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

void RoleActivityZhenPinShangPu::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	switch (raor->opera_type)
	{
		case 0:
			{
				this->SendInfo();
			}
			break;
		case 1:
			{
				long long unique_id = 0;
				int ret_num = this->OnBuyItemCheck(raor->param_1, raor->param_2, unique_id); // param1 配置seq ,  param2要选择的物品二进制

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

int RoleActivityZhenPinShangPu::GetDayBuyTimes(int type)
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.ra_day_buy_times); ++i)
	{
		if (type == m_param.ra_day_buy_times[i].type)
		{
			return m_param.ra_day_buy_times[i].buy_times;
		}
	}

	return 0;
}

int RoleActivityZhenPinShangPu::GetBuyTimes(int type)
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.ra_buy_times); ++i)
	{
		if (type == m_param.ra_buy_times[i].type)
		{
			return m_param.ra_buy_times[i].buy_times;
		}
	}

	return 0;
}

void RoleActivityZhenPinShangPu::AddDayBuyTimes(int type)
{
	int free_index = -1;
	for (int i = 0; i < ARRAY_LENGTH(m_param.ra_day_buy_times); ++i)
	{
		if (type == m_param.ra_day_buy_times[i].type)
		{
			m_param.ra_day_buy_times[i].buy_times += 1;
			return;
		}

		if (free_index == -1 && 0 == m_param.ra_day_buy_times[i].type && m_param.ra_day_buy_times[i].buy_times == 0)
		{
			free_index = i;
		}
	}

	if (free_index != -1)
	{
		m_param.ra_day_buy_times[free_index].type = type;
		m_param.ra_day_buy_times[free_index].buy_times = 1;
	}
}

void RoleActivityZhenPinShangPu::AddBuyTimes(int type)
{
	int free_index = -1;
	for (int i = 0; i < ARRAY_LENGTH(m_param.ra_buy_times); ++i)
	{
		if (type == m_param.ra_buy_times[i].type)
		{
			m_param.ra_buy_times[i].buy_times += 1;
			return;
		}

		if (free_index == -1 && 0 == m_param.ra_buy_times[i].type && m_param.ra_buy_times[i].buy_times == 0)
		{
			free_index = i;
		}
	}

	if (free_index != -1)
	{
		m_param.ra_buy_times[free_index].type = type;
		m_param.ra_buy_times[free_index].buy_times = 1;
	}
}

int RoleActivityZhenPinShangPu::OnBuyItemCheck(int seq, int select_flag, long long ARG_OUT & unique_key)
{
	if (seq < 0 || seq >= RA_ZHEN_PIN_SHANG_PU_MAX_ITEM)
	{
		return -__LINE__;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return -__LINE__;
	}

	unique_key = 0;

	const RandActivityZhenPinShangPuConfig * config = static_cast<const RandActivityZhenPinShangPuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return -__LINE__;
	}

	time_t buy_time = EngineAdapter::Instance().Time();
	int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDayByTime(ramgr, m_role_activity_type, buy_time);

	const RAZhenPinShangPuItemCfg * buy_cfg = config->GetRAZhenPinShangPuItemCfg(ramgr, seq);
	if (buy_cfg == NULL)
	{
		return -__LINE__;
	}

	if (!(buy_cfg->min_buy_day <= open_day && open_day <= buy_cfg->max_buy_day))
	{
		return -__LINE__;
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
	{
		if (this->GetBuyTimes(buy_cfg->type) >= buy_cfg->limit_buy_times)
		{
			return -__LINE__;
		}
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
	{
		if (this->GetDayBuyTimes(buy_cfg->type) >= buy_cfg->limit_buy_times_day)
		{
			return -__LINE__;
		}
	}

	std::vector<ItemConfigData> rewards;
	bool b = config->GetRewards(buy_cfg->reward_group_id, buy_cfg->select_reward_num, select_flag, rewards);
	if (!b)
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
	buy_item->param1 = buy_cfg->type;
	buy_item->param2 = buy_cfg->limit_type;
	buy_item->param3 = buy_cfg->add_gold;
	::ItemConfigDataVecToBuyItem(rewards, buy_item->rewards_item, &buy_item->rewards_num);
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, gamelog::g_log_zhen_pin_shang_pu, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_zhen_pin_shang_pu.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return 0;
}

int RoleActivityZhenPinShangPu::OnBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_zhen_pin_shang_pu.printf(LL_INFO, "%s line:%d | role[%d %s] ra_begin_timestamp:%u %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
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

	int type = buy_item->param1;
	int limit_type = buy_item->param2;
	int add_gold = buy_item->param3;


	m_role_module_mgr->GetMoney()->AddGold(add_gold, __FUNCTION__);

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(rewards))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_ZHEN_PIN_SHANG_PU);
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
		if (limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
		{
			this->AddBuyTimes(type);
		}

		if (limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
		{
			this->AddDayBuyTimes(type);
		}

		this->SendInfo();
		this->SetRoleDataChange();
	}

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), COMMON_SAVE_TYPE, unique_id, gamelog::g_log_zhen_pin_shang_pu, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

int RoleActivityZhenPinShangPu::OnBuyGiftItem(int seq, int select_flag, ARG_OUT int * price)
{
	if (seq < 0 || seq >= RA_ZHEN_PIN_SHANG_PU_MAX_ITEM)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	const RandActivityZhenPinShangPuConfig * config = static_cast<const RandActivityZhenPinShangPuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	time_t buy_time = EngineAdapter::Instance().Time();
	int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDayByTime(ramgr, m_role_activity_type, buy_time);

	const RAZhenPinShangPuItemCfg * buy_cfg = config->GetRAZhenPinShangPuItemCfg(ramgr, seq);
	if (buy_cfg == NULL)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	if (!(buy_cfg->min_buy_day <= open_day && open_day <= buy_cfg->max_buy_day))
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_ALL)
	{
		if (this->GetBuyTimes(buy_cfg->type) >= buy_cfg->limit_buy_times)
		{
			return BUY_GIFT_ITEM_RET_TYPE_2;
		}
	}

	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
	{
		if (this->GetDayBuyTimes(buy_cfg->type) >= buy_cfg->limit_buy_times_day)
		{
			return BUY_GIFT_ITEM_RET_TYPE_2;
		}
	}

	std::vector<ItemConfigData> rewards;
	bool b = config->GetRewards(buy_cfg->reward_group_id, buy_cfg->select_reward_num, select_flag, rewards);
	if (!b)
	{
		return BUY_GIFT_ITEM_RET_TYPE_1;
	}

	if (NULL != price)
	{
		*price = buy_cfg->need_chong_zhi_gold;
		return 0;
	}

	m_role_module_mgr->GetMoney()->AddGold(buy_cfg->add_gold, __FUNCTION__);

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(rewards))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_ZHEN_PIN_SHANG_PU);
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
		this->AddBuyTimes(buy_cfg->type);
	}
	if (buy_cfg->limit_type == ACTVITY_BUY_LIMIT_TYPE_DAY)
	{
		this->AddDayBuyTimes(buy_cfg->type);
	}

	this->SendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	return 0;
}

void RoleActivityZhenPinShangPu::SendInfo()
{
	Protocol::SCRAZhenPinShangPu protocol;
	for (int i = 0; i < RA_ZHEN_PIN_SHANG_PU_MAX_ITEM; ++i)
	{
		protocol.buy_times[i] = m_param.ra_buy_times[i];
		protocol.day_buy_times[i] = m_param.ra_day_buy_times[i];
	}

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

void RoleActivityZhenPinShangPu::SendOpenDayInfo()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityZhenPinShangPuConfig * config = static_cast<const RandActivityZhenPinShangPuConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	Protocol::SCRAZhenPinShangPuOpenDay protocol;
	protocol.day = config->GetOpenDay(ramgr);
	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}


