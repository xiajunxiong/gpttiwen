#include "roleactivitysuperoffshop.hpp"
#include "gamelog.h"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "item/money.h"
#include "item/knapsack.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/uniqueidgenerator.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "protocol/randactivity/msgracoolsummer.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitysuperoffshopconfig.hpp"

RoleActivitySuperOffShop::RoleActivitySuperOffShop(int type)
	:RoleActivity(type)
{
}

RoleActivitySuperOffShop::~RoleActivitySuperOffShop()
{
}

void RoleActivitySuperOffShop::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RASuperOffShopParam));
}

void RoleActivitySuperOffShop::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RASuperOffShopParam));
}

void RoleActivitySuperOffShop::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP);
	this->SendInfo(true);
}

void RoleActivitySuperOffShop::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP != rand_act_type)
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
		const RandActivitySuperOffShopConfig * config = static_cast<const RandActivitySuperOffShopConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP));
		if (NULL == config)
		{
			return;
		}
		m_param.Reset();
		if (!config->GetDiscItemRandListCfg(ramgr, m_param.phase_rand_disc_item_list, MAX_RA_SUPER_OFF_SHOP_DISC_ITEM_NUM))
		{
			return;
		}
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->SetRoleDataChange();
	}
}

void RoleActivitySuperOffShop::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP);
	memset(m_param.today_normal_buy_times, 0, sizeof(m_param.today_normal_buy_times));
	memset(m_param.today_disc_buy_times, 0, sizeof(m_param.today_disc_buy_times));

	this->SetRoleDataChange();
	this->SendInfo(true);
}

void RoleActivitySuperOffShop::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP);
	switch (raor->opera_type)
	{
		case Protocol::RA_SUPER_OFF_SHOP_REQ_TYPE_INFO:
		{
			this->SendInfo();
		}
		break;
		case Protocol::RA_SUPER_OFF_SHOP_REQ_TYPE_BUY_NORMAL_ITEM:
		{
			this->OnBuyNormalItem(raor->param_1);
		}
		break;
		case Protocol::RA_SUPER_OFF_SHOP_REQ_TYPE_BUY_DISC_ITEM:
		{
			this->OnBuyDiscItem(raor->param_1);
		}
		break;
		case Protocol::RA_SUPER_OFF_SHOP_REQ_TYPE_BUY_CMD_ITEM_CONFIG_CHECK:
		{
			this->OnBuyItemChecek(raor->param_1, (0 != raor->param_2));
		}
		break;
	}
}

bool RoleActivitySuperOffShop::OnCmdBuyItem(long long unique_id, int chong_zhi_gold)
{
	RoleCommonSave * rcs = m_role_module_mgr->GetRoleCommonSaveManager()->GetRoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_SUPER_OFF_SHOP);
	if (rcs == NULL)
	{
		return false;
	}
	const RoleCommonSaveCmdBuyItem * buy_item = rcs->GetTypeData<RoleCommonSaveCmdBuyItem>(0);
	if (buy_item == NULL)
	{
		return false;
	}
	gamelog::g_log_cool_summer.printf(LL_INFO, "%s line:%d | activity_type[%d] role[%d %s] ra_begin_timestamp:%ud %s", __FUNCTION__, __LINE__, RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.ra_begin_timestamp, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id || buy_item->need_gold != chong_zhi_gold)
	{
		return false;
	}

	std::vector<ItemConfigData> rewards = ::BuyItemToItemConfigDataVec(buy_item->rewards_num, buy_item->rewards_item);
	if (rewards.empty())
	{
		return false;
	}
	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)rewards.size(), &rewards[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_SUPER_OFF_SHOP);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&rewards[0], (int)rewards.size(), SEND_MAIL_TYPE_ACTIVITY_BUY_SHOP, true, RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP);
	}
	//如果活动开启时间相等就说明同一次活动里购买，要增加次数。
	if (buy_item->ra_begin_timestamp == m_param.ra_begin_timestamp)
	{
		bool is_send = true;
		if (0 != buy_item->param4)	//普通货架
		{
			bool is_correct = (buy_item->param2 >= 0 && buy_item->param2 < MAX_RA_SUPER_OFF_SHOP_PHASE_NORMAL_ITEM_NUM) ? true : false;
			if (is_correct && EngineAdapter::Instance().DayID() == (unsigned int)buy_item->param3)
			{
				m_param.today_normal_buy_times[buy_item->param2]++;
			}
			if (buy_item->param1 >= 0 && buy_item->param1 < MAX_RA_SUPER_OFF_SHOP_NORMAL_ITEM_NUM)
			{
				m_param.activity_normal_buy_times[buy_item->param1]++;
			}
			else
			{
				is_correct = false;
			}
			if (is_correct)
			{
				this->SendSingleInfo(Protocol::SCRASuperOffShopSingleInfo::NOTICE_TYPE_NORMAL, buy_item->param2, buy_item->param1);
				is_send = false;
			}
			gamelog::g_log_cool_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], index[%d], seq[%d]", __FUNCTION__,
				m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
				RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP, buy_item->param1, buy_item->param2);
		}
		else
		{
			int is_cor = 0;
			if (buy_item->param2 >= 0 && buy_item->param2 < MAX_RA_SUPER_OFF_SHOP_DISC_ITEM_NUM)
			{
				m_param.activity_disc_buy_times[buy_item->param2]++;
				is_cor |= (1 << 0);
			}
			if (buy_item->param1 >= 0 && buy_item->param1 < MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM)
			{
				is_cor |= (1 << 1);
				if (EngineAdapter::Instance().DayID() == (unsigned int)buy_item->param3)
				{
					m_param.today_disc_buy_times[buy_item->param1]++;
				}
			}
			if (is_cor == 3)
			{
				this->SendSingleInfo(Protocol::SCRASuperOffShopSingleInfo::NOTICE_TYPE_DISC, buy_item->param1, buy_item->param2);
				is_send = false;
			}
			
			gamelog::g_log_cool_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], index[%d], seq[%d] type[%d]", __FUNCTION__,
				m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
				RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP, buy_item->param1, buy_item->param2, (is_cor & (1 << 0)) ? m_param.phase_rand_disc_item_list[buy_item->param2] : 0);
		}
		if (is_send)
		{
			this->SendInfo();
		}
		this->SetRoleDataChange();
	}
	rcs->RemoveData(0);
	m_role_module_mgr->GetRole()->Save();
	return true;
}

void RoleActivitySuperOffShop::SendInfo(bool is_recal_phase)
{
	Protocol::SCRASuperOffShopInfo info;
	info.is_need_recal_phase_flag = is_recal_phase;
	memcpy(info.phase_rand_disc_item_list, m_param.phase_rand_disc_item_list, sizeof(info.phase_rand_disc_item_list));
	memcpy(info.today_normal_buy_times, m_param.today_normal_buy_times, sizeof(info.today_normal_buy_times));
	memcpy(info.today_disc_buy_times, m_param.today_disc_buy_times, sizeof(info.today_disc_buy_times));
	memcpy(info.activity_normal_buy_times, m_param.activity_normal_buy_times, sizeof(info.activity_normal_buy_times));
	memcpy(info.activity_disc_buy_times, m_param.activity_disc_buy_times, sizeof(info.activity_disc_buy_times));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

//索引由外部调用判断
void RoleActivitySuperOffShop::SendSingleInfo(int notice_type, int param1, int param2)
{
	Protocol::SCRASuperOffShopSingleInfo info;
	switch (notice_type)
	{
		case Protocol::SCRASuperOffShopSingleInfo::NOTICE_TYPE_NORMAL:	
		{
			info.today_buy_times = m_param.today_normal_buy_times[param1];
			info.activity_buy_times = m_param.activity_normal_buy_times[param2];
		}
		break;
		case Protocol::SCRASuperOffShopSingleInfo::NOTICE_TYPE_DISC:
		{
			info.today_buy_times = m_param.today_disc_buy_times[param1];
			info.activity_buy_times = m_param.activity_disc_buy_times[param2];
		}
		break;
	default:
		return;
	}
	info.notice_type = notice_type;
	info.param1 = param1;
	info.param2 = param2;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivitySuperOffShop::OnBuyNormalItem(int seq)
{
	if (seq < 0 || seq >= MAX_RA_SUPER_OFF_SHOP_PHASE_NORMAL_ITEM_NUM)
	{
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivitySuperOffShopConfig * config = static_cast<const RandActivitySuperOffShopConfig*>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	const RASuperOffShopShopCfg *buy_item_cfg = config->GetShopCfg(ramgr, seq);
	if (NULL == buy_item_cfg || RandActivitySuperOffShopConfig::PRICE_TYPE_MONEY == buy_item_cfg->price_type)
	{
		return;
	}
	short & buy_times_today = m_param.today_normal_buy_times[buy_item_cfg->seq];
	short & buy_times_activity = m_param.activity_normal_buy_times[buy_item_cfg->index];
	switch (buy_item_cfg->limit_type)
	{
	case ACTVITY_BUY_LIMIT_TYPE_ALL:
	{
		if (buy_times_activity > 0 && buy_times_activity >= buy_item_cfg->buy_times)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
			return;
		}
	}
	break;
	case ACTVITY_BUY_LIMIT_TYPE_DAY:
	{
		if (buy_times_today > 0 && buy_times_today >= buy_item_cfg->today_times)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
			return;
		}
	}
	break;
	default:
		break;
	}
	
	//检查消耗
	switch (buy_item_cfg->price_type)
	{
	case RandActivitySuperOffShopConfig::PRICE_TYPE_COIN:
	{
		if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(buy_item_cfg->price))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
		m_role_module_mgr->GetMoney()->UseCoinBind(buy_item_cfg->price, __FUNCTION__);
	}
	break;
	case RandActivitySuperOffShopConfig::PRICE_TYPE_GOLD:
	{
		if (!m_role_module_mgr->GetMoney()->GoldMoreThan(buy_item_cfg->price))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}
		m_role_module_mgr->GetMoney()->UseGold(buy_item_cfg->price, __FUNCTION__);
	}
	break;
	case RandActivitySuperOffShopConfig::PRICE_TYPE_MONEY:
	{
		return;
	}
	break;
	default:
		break;
	}

	bool is_put_succ = false;
	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary(1, &buy_item_cfg->reward_item))
	{
		if (m_role_module_mgr->GetKnapsack()->Put(buy_item_cfg->reward_item, PUT_REASON_SUPER_OFF_SHOP))
		{
			is_put_succ = true;
		}
	}
	if (!is_put_succ)
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&buy_item_cfg->reward_item, 1, SEND_MAIL_TYPE_ACTIVITY_BUY_SHOP, true, RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP);
	}
	++buy_times_activity;
	++buy_times_today;
	this->SendSingleInfo(Protocol::SCRASuperOffShopSingleInfo::NOTICE_TYPE_NORMAL, buy_item_cfg->seq, buy_item_cfg->index);
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_cool_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], index[%d], seq[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP, buy_item_cfg->index, seq);
}

void RoleActivitySuperOffShop::OnBuyDiscItem(int seq)
{	
	if (seq < 0 || seq >= MAX_RA_SUPER_OFF_SHOP_DISC_ITEM_NUM || m_param.phase_rand_disc_item_list[seq] <= 0)
	{
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivitySuperOffShopConfig * config = static_cast<const RandActivitySuperOffShopConfig*>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	//购买的不是该阶段的
	if (config->GetPhase(ramgr) != (seq / MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM + 1))
	{
		return;
	}

	const RASuperOffShopDiscCfg * buy_item_cfg = config->GetDiscItemCfgByType(m_param.phase_rand_disc_item_list[seq]);
	if (NULL == buy_item_cfg || RandActivitySuperOffShopConfig::PRICE_TYPE_MONEY == buy_item_cfg->price_type)
	{
		return;
	}
	int index = seq % MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM;
	if(index < 0 || index >= MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM) return;

	short & buy_times_today = m_param.today_disc_buy_times[index];
	short & buy_times_activity = m_param.activity_disc_buy_times[seq];
	switch (buy_item_cfg->limit_type)
	{
	case ACTVITY_BUY_LIMIT_TYPE_ALL:
	{
		if (buy_times_activity > 0 && buy_times_activity >= buy_item_cfg->buy_times)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
			return;
		}
	}
	break;
	case ACTVITY_BUY_LIMIT_TYPE_DAY:
	{
		if (buy_times_today > 0 && buy_times_today >= buy_item_cfg->today_times)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
			return;
		}
	}
	break;
	default:
		break;
	}

	//检查消耗
	switch (buy_item_cfg->price_type)
	{
	case RandActivitySuperOffShopConfig::PRICE_TYPE_COIN:
	{
		if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(buy_item_cfg->price))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
		m_role_module_mgr->GetMoney()->UseCoinBind(buy_item_cfg->price, __FUNCTION__);
	}
	break;
	case RandActivitySuperOffShopConfig::PRICE_TYPE_GOLD:
	{
		if (!m_role_module_mgr->GetMoney()->GoldMoreThan(buy_item_cfg->price))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}
		m_role_module_mgr->GetMoney()->UseGold(buy_item_cfg->price, __FUNCTION__);
	}
	break;
	case RandActivitySuperOffShopConfig::PRICE_TYPE_MONEY:
	{
		return;
	}
	break;
	default:
		break;
	}

	bool is_put_succ = false;
	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary(1, &buy_item_cfg->reward_item))
	{
		if (m_role_module_mgr->GetKnapsack()->Put(buy_item_cfg->reward_item, PUT_REASON_SUPER_OFF_SHOP))
		{
			is_put_succ = true;
		}
	}
	if (!is_put_succ)
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&buy_item_cfg->reward_item, 1, SEND_MAIL_TYPE_ACTIVITY_BUY_SHOP, true, RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP);
	}
	++buy_times_activity;
	++buy_times_today;
	this->SendSingleInfo(Protocol::SCRASuperOffShopSingleInfo::NOTICE_TYPE_DISC, index, seq);
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_cool_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], index[%d], seq[%d] type[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP, index, seq, m_param.phase_rand_disc_item_list[seq]);
}

void RoleActivitySuperOffShop::OnBuyItemChecek(int seq, bool is_normal)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivitySuperOffShopConfig * config = static_cast<const RandActivitySuperOffShopConfig*>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	short buy_times_today = 0;
	short buy_times_activity = 0;
	int limit_type = 0;
	int buy_times_cfg = 0;
	int today_buy_times_cfg = 0;
	int price = 0;
	int index = 0;
	ItemConfigData item;
	if (is_normal)
	{	
		if (seq < 0 || seq >= MAX_RA_SUPER_OFF_SHOP_PHASE_NORMAL_ITEM_NUM)
		{
			return;
		}
		
		const RASuperOffShopShopCfg *buy_item_cfg = config->GetShopCfg(ramgr, seq);
		if (NULL == buy_item_cfg || RandActivitySuperOffShopConfig::PRICE_TYPE_MONEY != buy_item_cfg->price_type)
		{
			return;
		}
		buy_times_today = m_param.today_normal_buy_times[buy_item_cfg->seq];
		buy_times_activity = m_param.activity_normal_buy_times[buy_item_cfg->index];
		limit_type = buy_item_cfg->limit_type;
		buy_times_cfg = buy_item_cfg->buy_times;
		today_buy_times_cfg = buy_item_cfg->today_times;
		price = buy_item_cfg->price;
		index = buy_item_cfg->index;
		seq = buy_item_cfg->seq;
		item = buy_item_cfg->reward_item;
	}
	else
	{
		if (seq < 0 || seq >= MAX_RA_SUPER_OFF_SHOP_DISC_ITEM_NUM || m_param.phase_rand_disc_item_list[seq] <= 0)
		{
			return;
		}
		//购买的不是该阶段的
		if (config->GetPhase(ramgr) != (seq / MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM + 1))
		{
			return;
		}

		const RASuperOffShopDiscCfg * buy_item_cfg = config->GetDiscItemCfgByType(m_param.phase_rand_disc_item_list[seq]);
		if (NULL == buy_item_cfg || RandActivitySuperOffShopConfig::PRICE_TYPE_MONEY != buy_item_cfg->price_type)
		{
			return;
		}

		index = seq % MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM;
		if (index < 0 || index >= MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM) return;

		buy_times_today = m_param.today_disc_buy_times[index];
		buy_times_activity = m_param.activity_disc_buy_times[seq];
		limit_type = buy_item_cfg->limit_type;
		buy_times_cfg = buy_item_cfg->buy_times;
		today_buy_times_cfg = buy_item_cfg->today_times;
		price = buy_item_cfg->price;
		item = buy_item_cfg->reward_item;
	}
	switch (limit_type)
	{
	case ACTVITY_BUY_LIMIT_TYPE_ALL:
	{
		if (buy_times_activity > 0 && buy_times_activity >= buy_times_cfg)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
			return;
		}
	}
	break;
	case ACTVITY_BUY_LIMIT_TYPE_DAY:
	{
		if (buy_times_today > 0 && buy_times_today >= today_buy_times_cfg)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
			return;
		}
	}
	break;
	default:
		break;
	}
	
	RoleCommonSave * rcs = m_role_module_mgr->GetRoleCommonSaveManager()->GetRoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_SUPER_OFF_SHOP);
	if (rcs == NULL)
	{
		return;
	}
	RoleCommonSaveCmdBuyItem * buy_item = rcs->GetTypeData<RoleCommonSaveCmdBuyItem>(0);
	if (buy_item == NULL)
	{
		return;
	}
	time_t now = EngineAdapter::Instance().Time();
	if (now < buy_item->create_time + CMD_BUY_ITEM_WAIT_TIME)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_has_order, buy_item->create_time + CMD_BUY_ITEM_WAIT_TIME - (unsigned int)now);
		World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(m_role_module_mgr->GetRole(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));

		return;
	}

	if (buy_item->create_time != 0)
	{
		std::string log_str1 = ::GetString(*buy_item);
		gamelog::g_log_cool_summer.printf(LL_INFO, "%s line:%d | activity_type[%d] role[%d %s] %s", __FUNCTION__, __LINE__, RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str1.c_str());
	}
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&buy_item->unique_id))
	{
		return;
	}
	buy_item->cfg_vesion = config->GetConfigVer();
	buy_item->need_gold = price;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->param1 = index;
	buy_item->param2 = seq;
	buy_item->param3 = EngineAdapter::Instance().DayID();
	buy_item->param4 = is_normal ? 1 : 0;
	buy_item->rewards_item[0].item_id = item.item_id;
	buy_item->rewards_item[0].num = item.num;
	buy_item->rewards_item[0].is_bind = item.is_bind;
	buy_item->rewards_num = 1;
	rcs->SetDataChange(0);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_cool_summer.printf(LL_INFO, "%s line:%d | activity_type[%d] role[%d %s] %s", __FUNCTION__, __LINE__, RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	Protocol::SCCmdBuyItemCheckRet info;
	info.buy_type = BUY_TIME_TYPE_SUPER_OFF_SHOP;
	info.cfg_ver = buy_item->cfg_vesion;
	info.unique_id = buy_item->unique_id;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}
