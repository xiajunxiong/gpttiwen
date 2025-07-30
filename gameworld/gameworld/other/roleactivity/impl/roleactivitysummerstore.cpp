#include "roleactivitysummerstore.hpp"
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

#include "protocol/randactivity/msgraburningsummer.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitysummerstoreconfig.hpp"

RoleActivitySummerStore::RoleActivitySummerStore(int type)
	:RoleActivity(type)
{
}

RoleActivitySummerStore::~RoleActivitySummerStore()
{
}

void RoleActivitySummerStore::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RASummerStoreParam));
}

void RoleActivitySummerStore::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RASummerStoreParam));
}

void RoleActivitySummerStore::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SUMMER_STORE))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SUMMER_STORE);
	this->SendInfo(true);
}

//void RoleActivitySummerStore::OnRAOpenOnlineSend()
//{
//	this->SendInfo(true);
//}

void RoleActivitySummerStore::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_SUMMER_STORE != rand_act_type)
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
		const RandActivitySummerStoreConfig * config = static_cast<const RandActivitySummerStoreConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SUMMER_STORE));
		if (NULL == config)
		{
			return;
		}
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->SetRoleDataChange();
	}
}

void RoleActivitySummerStore::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SUMMER_STORE))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SUMMER_STORE);
	m_param.is_open_today_flag = 0;
	memset(m_param.today_buy_times, 0, sizeof(m_param.today_buy_times));
	this->SetRoleDataChange();
	this->SendInfo(true);
}

void RoleActivitySummerStore::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SUMMER_STORE))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SUMMER_STORE);
	switch (raor->opera_type)
	{
		case Protocol::RA_SUMMER_STORE_REQ_TYPE_INFO:
		{
			this->SendInfo();
		}
		break;
		case Protocol::RA_SUMMER_STORE_REQ_TYPE_BUY_ITEM:
		{
			this->BuyItem(raor->param_1);
		}
		break;
		case Protocol::RA_SUMMER_STORE_REQ_TYPE_BUY_CMD_ITEM_CONFIG_CHECK:
		{
			this->OnBuyItemChecek(raor->param_1);
		}
		break;
		case Protocol::RA_SUMMER_STORE_REQ_TYPE_OPEN_ACTIVITY:
		{
			this->SetOpenFlag();
		}
		break;
	}
}

bool RoleActivitySummerStore::OnCmdBuyItem(long long unique_id, int chong_zhi_gold)
{
	RoleCommonSave * rcs = m_role_module_mgr->GetRoleCommonSaveManager()->GetRoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_SUMMER_STORE);
	if (rcs == NULL)
	{
		return false;
	}
	const RoleCommonSaveCmdBuyItem * buy_item = rcs->GetTypeData<RoleCommonSaveCmdBuyItem>(0);
	if (buy_item == NULL)
	{
		return false;
	}
	gamelog::g_log_burning_summer.printf(LL_INFO, "%s line:%d | activity_type[%d] role[%d %s] ra_begin_timestamp:%ud %s", __FUNCTION__, __LINE__, RAND_ACTIVITY_TYPE_SUMMER_STORE, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
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
		m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_SUMMER_STORE);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&rewards[0], (int)rewards.size(), SEND_MAIL_TYPE_ACTIVITY_BUY_SHOP, true, RAND_ACTIVITY_TYPE_SUMMER_STORE);
	}
	//如果活动开启时间相等就说明同一次活动里购买，要增加次数。
	if (buy_item->ra_begin_timestamp == m_param.ra_begin_timestamp)
	{	
		bool is_correct = (buy_item->param2 >= 0 && buy_item->param2 < MAX_RA_SUMMER_STORE_PHASE_SHOP_ITEM_NUM) ? true : false;
		if (is_correct && (unsigned int)buy_item->param3 == EngineAdapter::Instance().DayID())
		{
			m_param.today_buy_times[buy_item->param2]++;
		}
		if (buy_item->param1 >= 0 && buy_item->param1 < MAX_RA_SUMMER_STORE_SHOP_ITEM_NUM)
		{
			m_param.activity_buy_times[buy_item->param1]++;
		}
		else
		{
			is_correct = false;
		}
		if (is_correct)
		{
			this->SendSingleInfo(buy_item->param2, buy_item->param1);
		}
		else
		{
			this->SendInfo();
		}
		this->SetRoleDataChange();
		gamelog::g_log_burning_summer.printf(LL_INFO, "%s activity_type[%d] index[%d] seq[%d] chong_zhi_gold[%d]", __FUNCTION__, RAND_ACTIVITY_TYPE_SUMMER_STORE, buy_item->param1, buy_item->param2, chong_zhi_gold);
	}
	rcs->RemoveData(0);
	m_role_module_mgr->GetRole()->Save();
	return true;
}

void RoleActivitySummerStore::SendInfo(bool is_recal_phase)
{
	Protocol::SCRASummerStoreInfo info;
	info.is_need_recal_phase_flag = is_recal_phase ? 1 : 0;
	info.is_open_today_flag = m_param.is_open_today_flag;
	memcpy(info.today_buy_times, m_param.today_buy_times, sizeof(info.today_buy_times));
	memcpy(info.activity_buy_times, m_param.activity_buy_times, sizeof(info.activity_buy_times));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

//索引安全由外部调用保证
void RoleActivitySummerStore::SendSingleInfo(short seq, short index)
{
	Protocol::SCRASummerStoreSingleInfo info;
	info.seq = seq;
	info.index = index;
	info.today_buy_times = m_param.today_buy_times[seq];
	info.activity_buy_times = m_param.activity_buy_times[index];

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivitySummerStore::BuyItem(int seq)
{
	if (seq < 0 || seq >= MAX_RA_SUMMER_STORE_PHASE_SHOP_ITEM_NUM)
	{
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivitySummerStoreConfig * config = static_cast<const RandActivitySummerStoreConfig*>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SUMMER_STORE));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	const RASummerStoreShopCfg *buy_item_cfg = config->GetShopCfg(ramgr, seq);
	if (NULL == buy_item_cfg || RASummerStoreShopCfg::PRICE_TYPE_MONEY == buy_item_cfg->price_type)
	{
		return;
	}
	short & buy_times_today = m_param.today_buy_times[buy_item_cfg->seq];
	short & buy_times_activity = m_param.activity_buy_times[buy_item_cfg->index];
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
	case RASummerStoreShopCfg::PRICE_TYPE_COIN:
	{
		if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(buy_item_cfg->price))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
		m_role_module_mgr->GetMoney()->UseCoinBind(buy_item_cfg->price, __FUNCTION__);
	}
	break;
	case RASummerStoreShopCfg::PRICE_TYPE_GOLD:
	{
		if (!m_role_module_mgr->GetMoney()->GoldMoreThan(buy_item_cfg->price))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}
		m_role_module_mgr->GetMoney()->UseGold(buy_item_cfg->price, __FUNCTION__);
	}
	break;
	case RASummerStoreShopCfg::PRICE_TYPE_MONEY:
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
		if (m_role_module_mgr->GetKnapsack()->Put(buy_item_cfg->reward_item, PUT_REASON_SUMMER_STORE))
		{
			is_put_succ = true;
		}
	}
	if (!is_put_succ)
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&buy_item_cfg->reward_item, 1, SEND_MAIL_TYPE_ACTIVITY_BUY_SHOP, true, RAND_ACTIVITY_TYPE_SUMMER_STORE);
	}
	++buy_times_activity;
	++buy_times_today;
	this->SendSingleInfo(buy_item_cfg->seq, buy_item_cfg->index);
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_burning_summer.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d], index[%d], seq[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_SUMMER_STORE, buy_item_cfg->index, seq);
}

void RoleActivitySummerStore::OnBuyItemChecek(int seq)
{
	if (seq < 0 || seq >= MAX_RA_SUMMER_STORE_PHASE_SHOP_ITEM_NUM)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivitySummerStoreConfig * config = static_cast<const RandActivitySummerStoreConfig*>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SUMMER_STORE));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	const RASummerStoreShopCfg *buy_item_cfg = config->GetShopCfg(ramgr, seq);
	if (NULL == buy_item_cfg || RASummerStoreShopCfg::PRICE_TYPE_MONEY != buy_item_cfg->price_type || buy_item_cfg->price <= 0)
	{
		return;
	}
	short & buy_times_today = m_param.today_buy_times[buy_item_cfg->seq];
	short & buy_times_activity = m_param.activity_buy_times[buy_item_cfg->index];
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

	RoleCommonSave * rcs = m_role_module_mgr->GetRoleCommonSaveManager()->GetRoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_SUMMER_STORE);
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
		gamelog::g_log_burning_summer.printf(LL_INFO, "%s line:%d | activity_type[%d] role[%d %s] %s", __FUNCTION__, __LINE__, RAND_ACTIVITY_TYPE_SUMMER_STORE, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str1.c_str());
	}
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&buy_item->unique_id))
	{
		return;
	}
	buy_item->cfg_vesion = config->GetConfigVer();
	buy_item->need_gold = buy_item_cfg->price;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = m_param.ra_begin_timestamp;
	buy_item->param1 = buy_item_cfg->index;
	buy_item->param2 = seq;
	buy_item->param3 = EngineAdapter::Instance().DayID();
	buy_item->rewards_item[0].item_id = buy_item_cfg->reward_item.item_id;
	buy_item->rewards_item[0].num = buy_item_cfg->reward_item.num;
	buy_item->rewards_item[0].is_bind = buy_item_cfg->reward_item.is_bind;
	buy_item->rewards_num = 1;
	rcs->SetDataChange(0);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_burning_summer.printf(LL_INFO, "%s line:%d | activity_type[%d] role[%d %s] %s", __FUNCTION__, __LINE__, RAND_ACTIVITY_TYPE_SUMMER_STORE, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	Protocol::SCCmdBuyItemCheckRet info;
	info.buy_type = BUY_TIME_TYPE_SUMMER_STORE;
	info.cfg_ver = buy_item->cfg_vesion;
	info.unique_id = buy_item->unique_id;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivitySummerStore::SetOpenFlag()
{
	m_param.is_open_today_flag = 1;
	this->SendInfo();
}
