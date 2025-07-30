#include "randactivitygodbeastadvent.hpp"
#include "config/randactivityconfig/impl/randactivitygodbeastadventconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"

#include "gameworld/global/trademarket/trademarketmanager.hpp"
#include "other/rolemail/rolemail.hpp"
#include "other/route/mailroute.hpp"

#include "servercommon/string/gameworldstr.h"

RandActivityGodBeastAdvent::RandActivityGodBeastAdvent(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT), is_open(0), is_first_sync(0), buy_limit(0)
{
}

RandActivityGodBeastAdvent::~RandActivityGodBeastAdvent()
{
}

void RandActivityGodBeastAdvent::Init(const ActivityData & arg_data)
{
	RandActivity::Init(arg_data);

	const RandActivityData * ra_data = &arg_data.rand_activity_data;

	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(ra_data->data));

	memcpy(&m_param, ra_data->data, sizeof(m_param));
}

void RandActivityGodBeastAdvent::GetInitParam(ActivityData & arg_data)
{
	RandActivity::GetInitParam(arg_data);

	const RandActivityData * ra_data = &arg_data.rand_activity_data;

	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(ra_data->data));

	memcpy((void *)ra_data->data, &m_param, sizeof(m_param));
}

void RandActivityGodBeastAdvent::InitBuy(int buy_num)
{
	buy_limit = buy_num;
}

void RandActivityGodBeastAdvent::GetInitBuyParam(int& buy_num)
{
	buy_num = buy_limit;
}

void RandActivityGodBeastAdvent::Update(unsigned long interval, time_t now_second)
{
	RandActivity::Update(interval, now_second);

	this->CheckNextCycle();

	if (is_first_sync == 0)
	{
		this->SyncHiddenGameInfo();

		is_first_sync = 1;
	}
}


void RandActivityGodBeastAdvent::CheckNextCycle()
{
	const RandActivityYiShouLinShiConfig * config = static_cast<const RandActivityYiShouLinShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
	if (NULL == config) return;

	unsigned int start_time = 0;
	int reward_group = 0, activity_seq = 0;

	reward_group = config->GetStartTimeCfg(start_time, activity_seq);
	if (reward_group <= 0)
	{
		if (m_param.cycle_flag != 0)
		{
			is_open = 0;
			m_param.cycle_flag = 0;
			this->SendCycleAllInfo();
			gamelog::g_log_rand_activity.printf(LL_INFO, "%s Activity Cycle is Over? reward_group[%d]", __FUNCTION__, reward_group);			
		}
		return;
	}

	if (!is_open)
	{
		is_open = 1;
	}

	if (m_param.ra_begin_timestamp != start_time)
	{
		int t_cycle_flag = m_param.cycle_flag + 1;

		m_param.Reset();

		m_param.ra_begin_timestamp = start_time;
		m_param.reward_group = reward_group;
		m_param.cycle_flag = t_cycle_flag;
		m_param.activity_seq = activity_seq;
		buy_limit = config->GetOtherCfg().is_more_change;		// 每次切换周期直接初始化为最大

		this->SyncHiddenGameInfo();
		this->SendCycleAllInfo();

		gamelog::g_log_rand_activity.printf(LL_INFO, "%s yi_shou_lin_shi cycle cutover new_cycle[%d] start_time[%d] reward_group[%d] activity_seq[%d] buy_limit[%d]", __FUNCTION__,
			m_param.cycle_flag, m_param.ra_begin_timestamp, m_param.reward_group, m_param.activity_seq, buy_limit);
	}
}

bool RandActivityGodBeastAdvent::IsNextCycle(short cycle_flag)
{
	if (cycle_flag != m_param.cycle_flag)
	{
		gamelog::g_log_rand_activity.printf(LL_INFO, "role_cycle_flag[%d] system_cycle_flag[%d]", cycle_flag, m_param.cycle_flag);
		return true;
	}

	return false;
}


bool RandActivityGodBeastAdvent::CanSell(unsigned int sell_timestamp)
{
	sell_timestamp += SECOND_PER_DAY;

	const RandActivityYiShouLinShiConfig * config = static_cast<const RandActivityYiShouLinShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
	if (NULL == config) return false;

	const YSLSCycleCfg * cycle_cfg = config->GetCurCycleCfg(m_param.activity_seq);
	if (NULL == cycle_cfg) return false;

	if (cycle_cfg->start_time <= sell_timestamp && sell_timestamp < cycle_cfg->end_time)
	{
		return true;
	}

	return false;
}

bool RandActivityGodBeastAdvent::IsCycleExchangeItem(int item_id)
{
	const RandActivityYiShouLinShiConfig * config = static_cast<const RandActivityYiShouLinShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
	if (NULL == config) return false;

	const YSLSRecycleCfg* recycle_cfg = config->GetRecycleCfg(item_id);
	if (NULL != recycle_cfg) return true;

	return false;
}

bool RandActivityGodBeastAdvent::IsCurCycleExchangeItem(int item_id)
{
	const RandActivityYiShouLinShiConfig * config = static_cast<const RandActivityYiShouLinShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
	if (NULL == config) return false;

	const YSLSRecycleCfg* recycle_cfg = config->GetCurRecycleCfg(m_param.reward_group);
	if (NULL == recycle_cfg) return false;

	if (recycle_cfg->item_id == item_id)
	{
		return true;
	}

	return false;
}

int RandActivityGodBeastAdvent::BuyServerLimitItem(int user_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return -1;
	}

	if (buy_limit <= 0) return -2;

	buy_limit -= 1;

	this->SyncHiddenGameInfo();
	this->SendNumAllInfo();

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user_id[%d] last_num[%d]", __FUNCTION__, user_id, buy_limit);
	return 0;
}

void RandActivityGodBeastAdvent::SyncHiddenGameInfo()
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameGodBeastAdventNumInfo msg;
	msg.num = buy_limit;

	InternalComm::Instance().SendToAllGameThroughCross((const char*)&msg, sizeof(msg));
}

void RandActivityGodBeastAdvent::SynHiddenGameReceiveNumInfo(crossgameprotocal::HiddenGameGodBeastAdventNumInfo * msg)
{
	if (CrossConfig::Instance().IsHiddenServer() || msg == NULL) return;

	buy_limit = msg->num;

	this->SendNumAllInfo();
}

void RandActivityGodBeastAdvent::ReceiveTradeMarket(crossgameprotocal::GameHiddenGodBeastAdventReceiveTradeMarketReq * msg)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	int count = 0;
	if (!TradeMarketManager::Instance().ItemRecover(msg->uid, msg->item_id, &count))		// 交易行回收
	{
		return;
	}

	if (count == 0) return;	// 没有道具可回收

	const RandActivityYiShouLinShiConfig * config = static_cast<const RandActivityYiShouLinShiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
	if (NULL == config) return;

	const YSLSRecycleCfg* recycle_cfg = config->GetCurRecycleCfg(msg->exchange_group);
	if (recycle_cfg == NULL) return;

	int coin = 0, gold = 0, jade = 0;
	coin = recycle_cfg->coin * count;
	gold = recycle_cfg->gold * count;
	jade = recycle_cfg->silver_coin * count;

	std::vector<ItemConfigData> item_list;
	if (recycle_cfg->has_item)
	{
		for (size_t i = 0; i < recycle_cfg->item_list.size(); i++)
		{
			item_list.push_back(ItemConfigData(recycle_cfg->item_list[i].item_id, recycle_cfg->item_list[i].is_bind, recycle_cfg->item_list[i].num*count));
		}
	}

	{	// 发奖励邮件
		static MailContentParam contentparam; contentparam.Reset();
		if (coin > 0) contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = coin;
		if (gold > 0) contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] = gold;
		if (jade > 0) contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_SILVER_COIN] = jade;
		if (recycle_cfg->has_item)
		{
			FillMailContentParam(contentparam, item_list);
		}

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_god_beast_advent_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_god_beast_advent_content);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(msg->uid, MAIL_REASON_DEFAULT, contentparam);
		}
	}

	std::string log_str;
	for (size_t i = 0; i < item_list.size(); i++)
	{
		log_str += STRING_SPRINTF("[%d, %d] ", item_list[i].item_id, item_list[i].num);
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s role[%d] recycle[%d, %d] reward: coin[%d] gold[%d] silver_coin[%d] item[%s]", __FUNCTION__,
		msg->uid, recycle_cfg->item_id, count, coin, gold, jade, log_str.c_str());
}

void RandActivityGodBeastAdvent::SendNumAllInfo(RoleModuleManager* rmgr)
{
	Protocol::SCGodBeastAdventBuyInfo msg;
	msg.last_buy_limit = buy_limit;

	if (NULL != rmgr)
	{
		rmgr->NetSend(&msg, sizeof(msg));
	}
	else
	{
		World::GetInstWorld()->SendToAllGateway((const char *)&msg, sizeof(msg));
	}
}

void RandActivityGodBeastAdvent::SendCycleAllInfo(RoleModuleManager* rmgr)
{
	Protocol::SCGodBeastAdventOpenInfo msg;
	msg.open_flag = is_open;
	msg.seq = m_param.activity_seq;

	if (NULL != rmgr)
	{
		rmgr->NetSend(&msg, sizeof(msg));
	}
	else
	{
		World::GetInstWorld()->SendToAllGateway((const char *)&msg, sizeof(msg));
	}

}

