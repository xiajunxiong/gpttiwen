#include "trademarketmanager.hpp"
#include "engineadapter.h"
#include "gamelog.h"
#include "usertradeinfo.hpp"
#include "global/rmibackobjdef.h"
#include "internalcomm.h"
#include "serverlogic.h"
#include "servercommon/userprotocal/msgsystem.h"
#include "servercommon/errornum.h"
#include "global/trademarket/usersaleitem.hpp"
#include "obj/character/role.h"
#include "item/money.h"
#include "other/rolemodulemanager.hpp"
#include "item/knapsack.h"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "servercommon/noticenum.h"
#include "protocol/msgtrademarket.h"
#include "item/itempool.h"
#include "other/pet/pet.hpp"
#include "trademarketconfig.hpp"
#include "world.h"
#include "servercommon/string/gameworldstr.h"
#include "global/trademarket/trademarketconfig.hpp"
#include "item/itembase.h"
#include "global/randactivity/randactivitymanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "systemtradeinfo.hpp"
#include "systemsaleitem.hpp"
#include "other/pet/petconfig.hpp"
#include "servercommon/serverconfig/nameconfig.h"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "item/itembase.h"
#include "equipment/equipment.h"
#include "item/itempool.h"
#include "equipment/jewelry.h"
#include "skill/skillpool.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "other/trade_credit/trade_credit.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityreducetaxrateconfig.hpp"
#include "other/event/eventhandler.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/rolecross/rolecross.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "usertraderecord.hpp"
#include "global/randactivity/randactivityimpl/randactivitygodbeastadvent.hpp"

TradeMarketManager& TradeMarketManager::Instance()
{
	static TradeMarketManager instance;
	return instance;
}

TradeMarketManager::TradeMarketManager() : m_load_data_state(0), 
	m_load_user_record_state(0), m_load_system_record_state(0), 
	m_load_system_shelve_state(0), m_load_system_recycle_rule_state(0), 
	m_load_system_auto_upshelve_state(0), m_next_check_user_time(0u), 
	m_next_recycle_time(0u)
{

}

TradeMarketManager::~TradeMarketManager()
{
	this->Release();
}

void TradeMarketManager::OnServerStart()
{
	m_load_data_state = GLOBAL_SYSTEM_DATA_STATE_LOADING;
	m_load_user_record_state = GLOBAL_SYSTEM_DATA_STATE_LOADING;
	m_load_system_record_state = GLOBAL_SYSTEM_DATA_STATE_LOADING;
	m_load_system_shelve_state = GLOBAL_SYSTEM_DATA_STATE_LOADING;
	m_load_system_recycle_rule_state = GLOBAL_SYSTEM_DATA_STATE_LOADING;
	m_load_system_auto_upshelve_state = GLOBAL_SYSTEM_DATA_STATE_LOADING;

	this->LoadData(0LL);
	this->LoadSystemShelve(0LL);
	this->LoadUserRecord(0LL);
	this->LoadSystemRecord(0LL);
	this->LoadSystemRecycleRule(0LL);
	this->LoadSystemAutoUpshelve(0LL);
}

void TradeMarketManager::OnServerStop()
{
	this->SaveAll();
}

bool TradeMarketManager::IsLoadFinish()
{
	return GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH == m_load_data_state &&
		GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH == m_load_user_record_state &&
		GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH == m_load_system_record_state &&
		GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH == m_load_system_shelve_state &&
		GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH == m_load_system_recycle_rule_state &&
		GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH == m_load_system_auto_upshelve_state;
}

void TradeMarketManager::OnUserLogin(const UserID& user_id)
{
	this->NewVersionAutoOffshelve(user_id);
}

UserTradeInfo* TradeMarketManager::GetUserTradeInfo(int role_id, bool is_create /* = false */)
{
	UserTradeInfoMap::iterator it = m_user_trade_info_map.find(role_id);
	if (m_user_trade_info_map.end() == it)
	{
		if (!is_create)
		{
			return NULL;
		}
		else
		{
			UserTradeInfo* info = new UserTradeInfo(role_id);
			it = m_user_trade_info_map.insert(UserTradeInfoMap::value_type(role_id, info)).first;
		}
	}

	return it->second;
}

long long TradeMarketManager::GetUniqueItemKey()
{
	long long key = 0;
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&key))
	{
		return 0;
	}
	return key;
}

void TradeMarketManager::Update(unsigned long interval, time_t now_second)
{
	if (!this->IsLoadFinish()) return;

	if (now_second > m_next_check_user_time)
	{
		m_next_check_user_time = static_cast<unsigned int>(now_second); //1秒一次

		UserTradeInfoMap::iterator it = m_user_trade_info_map.begin();
		for (; it != m_user_trade_info_map.end(); ++it)
		{
			if (NULL != it->second)
			{
				it->second->Update(now_second);
			}
		}

		m_sys_trade_info.Update(now_second);
		this->DoSystemBuy();
		this->DoSystemOffshelve();
	}

	if (now_second > m_next_recycle_time)
	{
		m_next_recycle_time = static_cast<unsigned int>(now_second) + 60;
		this->DoRecycle();
	}

	m_transaction_record.Update(static_cast<unsigned int>(now_second));
	m_system_recycle.Update(static_cast<unsigned int>(now_second));
	m_auto_upshelve.Update(static_cast<unsigned int>(now_second));
	m_sale_item_index.Update(interval, static_cast<unsigned int>(now_second));
}

bool TradeMarketManager::OnAddItemOnShelveReq(Role* role, SaleItemParam& param, int used_coin, int used_credit)
{
	if (NULL == role) return false;

	if (param.item_wrapper.Invalid())
	{
#ifdef _DEBUG
		assert(0);
#endif
		gamelog::g_log_trade_market.printf(LL_ERROR, "TradeMarketManager::OnAddItemOnShelveReq role[%d,%s] add invalid item",
			role->GetUID(), role->GetName());
		return false;
	}

	UserTradeInfo* user_trade_info = this->GetUserTradeInfo(role->GetUID(), true);
	if (NULL == user_trade_info)
	{
		return false;
	}

	int errnum = 0;
	if (!this->CanSellItem(role->GetUID(), param, &errnum))
	{
		this->SendNoticeNum(role->GetNetId(), errnum);
		return false;
	}

	int empty_sale_index = 0;
	if (!user_trade_info->GetEmptyIndex(&empty_sale_index))
	{
		if (!this->GiveResourceToUser(role->GetUserId(), &param.item_wrapper, 0, used_coin, used_credit, __FUNCTION__))
		{
			gamelog::g_log_trade_market.printf(LL_INFO, "OnAddItemOnShelveReq Failed[no empty idx], ERROR on GiveResourceToUser, role[%d,%s] item[%d,%d] used_coin[%d] used_credit[%d]",
				role->GetUID(), role->GetName(), param.item_wrapper.item_id, param.item_wrapper.num, used_coin, used_credit);
		}
		
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_SHELVE_FULL);
		return false;
	}

	bool ret = user_trade_info->OnAddSaleItem(empty_sale_index, role->GetUID(), role->GetName(), param, false);
	if (!ret)
	{
		if (!this->GiveItemToRole(role, param.item_wrapper, "OnAddItemOnShelveReq Failed[add sale item fail]"))
		{
			gamelog::g_log_trade_market.printf(LL_INFO, "OnAddItemOnShelveReq Failed[add sale item fail], ERROR on GiveItemToRole, role[%d,%s] item[%d,%d]",
				role->GetUID(), role->GetName(), param.item_wrapper.item_id, param.item_wrapper.num);
		}
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ADD_ON_SHELVE_FAILED);
		return false;
	}
	else
	{
#ifdef TRACK_ITEM_FLAG
		gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG TradeMarketManager::OnAddItemOnShelveReq role[%d,%s] upshelve item_id[%d] item_unique_id[%lld] unique_key[%lld]",
			role->GetUID(), role->GetName(), param.item_wrapper.item_id, param.item_wrapper.item_unique_id, param.unique_key);
#endif
	}

	this->SendShelveChangeNotice(role->GetUID(), empty_sale_index, Protocol::SCTradeMarketUserShelveData::REASON_NOTIFY_ADD);
	
	LOG_QUICK12(gamelog::g_log_quick,
		LOG_TYPE_TRADE_MARTKET_ADD, role->GetUID(), role->GetName(), role->GetPlatName(),
		param.item_wrapper.item_id, param.item_wrapper.num, NULL, NULL,
		0, param.gold_price, 0, 0, 0);
	
	return true;
}

bool TradeMarketManager::OnRemoveItemOnShelveReq(Role* role, int sale_index)
{
	UserTradeInfo* user_trade_info = this->GetUserTradeInfo(role->GetUID());
	if (NULL == user_trade_info)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_REMOVE_ITEM_NOT_FOUND);
		return false;
	}

	const UserSaleItem* sale_item = user_trade_info->GetSaleItem(sale_index);
	if (NULL == sale_item)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_REMOVE_ITEM_NOT_FOUND);
		return false;
	}
	
	if (sale_item->GetSaleItemParamConst().sold_num > 0)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_CASH_OUT_FIRST);
		return false;
	}

	static ItemDataWrapper wrapper;
	wrapper = sale_item->GetSaleItemParamConst().item_wrapper;
	int gold_price = sale_item->GetSaleItemParamConst().gold_price;
	int group_num = wrapper.num / sale_item->GetSaleItemParamConst().num_per_group;
	long long return_credit_fee = 1LL * gold_price * group_num;
	long long unique_key = sale_item->GetSaleItemParamConst().unique_key;
	if (!user_trade_info->OnRemoveSaleItem(sale_index, __FUNCTION__))
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_UNKNOWN_ERROR);
		return false;
	}

	if (!this->GiveItemToRole(role, wrapper, __FUNCTION__))
	{
		gamelog::g_log_trade_market.printf(LL_INFO, "OnRemoveItemOnShelveReq, ERROR on GiveItemToRole, role[%d,%s] item[%d,%d]",
			role->GetUID(), role->GetName(), wrapper.item_id, wrapper.num);
	}
	else
	{
#ifdef TRACK_ITEM_FLAG
		gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG TradeMarketManager::OnRemoveItemOnShelveReq role[%d,%s] offshelve item_id[%d] item_unique_id[%lld] unique_key[%lld]",
			role->GetUID(), role->GetName(), wrapper.item_id, wrapper.item_unique_id, unique_key);
#endif

		role->GetRoleModuleManager()->GetTradeCredit()->AddCredit(return_credit_fee, __FUNCTION__);
	}

	this->SendShelveChangeNotice(role->GetUID(), sale_index, Protocol::SCTradeMarketUserShelveData::REASON_NOTIFY_REMOVE);
	
	LOG_QUICK12(gamelog::g_log_quick,
		LOG_TYPE_TRADE_MARTKET_DEL, role->GetUID(), role->GetName(), NULL,
		wrapper.item_id, wrapper.num, NULL, NULL,
		0, gold_price, 0, 0, 0);
	
	return true;
}

bool TradeMarketManager::OnCashOutReq(Role* role, int sale_index)
{
	UserTradeInfo* user_trade_info = this->GetUserTradeInfo(role->GetUID());
	if (NULL == user_trade_info)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_CASH_OUT_ITEM_NOT_FOUND);
		return false;
	}

	const UserSaleItem* sale_item = user_trade_info->GetSaleItem(sale_index);
	if (NULL == sale_item)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_CASH_OUT_ITEM_NOT_FOUND);
		return false;
	}

	if (sale_item->GetSaleItemParamConst().sold_num <= 0)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_CAN_NOT_CASH_OUT);
		return false;
	}

	ItemID sold_item_id = sale_item->GetSaleItemParamConst().item_wrapper.item_id;
	long long add_gold = 0;
	if (!user_trade_info->OnCashOutItem(sale_index, &add_gold))
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_CASH_OUT_FAILED);
		return false;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL != ramgr && ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_REDUCE_TAX_RATE))
	{
		const RandActivityReduceTaxRateConfig  * config = static_cast<const RandActivityReduceTaxRateConfig  *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_REDUCE_TAX_RATE));
		if (NULL != config)
		{
			if (add_gold > 0)
			{
				long long tax = static_cast<long long>(1.0 * add_gold * config->GetRateFromReduceTaxRateSectionCfg(ramgr) / TRADE_MARKET_PERCENT_NUM);
				tax = GetMax(tax, 1LL);
				add_gold = GetMax(add_gold - tax, 1LL);
			}

			RandActivity* act = ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_REDUCE_TAX_RATE);
			if (NULL != act)
			{
				act->OnRoleParticipateRA(role, __FUNCTION__);
			}
		}
	}
	else
	{
		if (add_gold > 0)
		{
			long long tax = static_cast<long long>(1.0 * add_gold * TRADEMARKET_CFG.GetConstantCfg().tax_rate / TRADE_MARKET_PERCENT_NUM);
			tax = GetMax(tax, 1LL);
			add_gold = GetMax(add_gold - tax, 1LL);
		}
	}

	this->GiveMoneyToRole(role, add_gold, 0, 0, __FUNCTION__);

	this->SendShelveChangeNotice(role->GetUID(), sale_index, Protocol::SCTradeMarketUserShelveData::REASON_NOTIFY_CHANGE);
	
	{
		//发系统信息
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_trade_market_sold_item_get_money,
			sold_item_id, add_gold);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role->GetUserId(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT));
		}
	}
	return true;
}

bool TradeMarketManager::OnUserShelveDataReq(Role* role)
{
	if (NULL == role) return false;

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return false;
	}

	this->SendShelveData(role);
	return true;
}

bool TradeMarketManager::OnSearchByItemIDReq(Role* role, ItemID item_id, short query_page, bool is_search_announce, short order_type)
{
	static SaleItemBase* item_list[SEARCH_SALE_ITEM_PER_PAGE];
	memset(item_list, 0, sizeof(item_list));
	int count = 0;
	int page = 0;
	int total_page = 0;
	m_sale_item_index.OnSearchByItem(is_search_announce, order_type, item_id, query_page, SEARCH_SALE_ITEM_PER_PAGE, item_list, &count, &page, &total_page);

	return this->SendSearchResult(role, count, item_list, page, total_page, order_type, is_search_announce, 0);
}

bool TradeMarketManager::OnSearchByItemTypeReq(Role* role, short item_search_type, short query_page, bool is_search_announce, short order_type)
{
	static SaleItemBase* item_list[SEARCH_SALE_ITEM_PER_PAGE];
	memset(item_list, 0, sizeof(item_list));
	int count = 0;
	int page = 0;
	int total_page = 0;
	m_sale_item_index.OnSearchByType(is_search_announce, order_type, item_search_type, query_page, SEARCH_SALE_ITEM_PER_PAGE, item_list, &count, &page, &total_page);

	return this->SendSearchResult(role, count, item_list, page, total_page, order_type, is_search_announce, 0);
}

bool TradeMarketManager::OnSearchByItemBigTypeReq(Role* role, short item_search_big_type, short query_page, bool is_search_announce, short order_type)
{
	static SaleItemBase* item_list[SEARCH_SALE_ITEM_PER_PAGE];
	memset(item_list, 0, sizeof(item_list));
	int count = 0;
	int page = 0;
	int total_page = 0;
	m_sale_item_index.OnSearchByBigType(is_search_announce, order_type, item_search_big_type, query_page, SEARCH_SALE_ITEM_PER_PAGE, item_list, &count, &page, &total_page);

	return this->SendSearchResult(role, count, item_list, page, total_page, order_type, is_search_announce, 0);
}

bool TradeMarketManager::OnSearchByItemTypeWithLevelReq(Role* role, short level, short item_search_type, short query_page, bool is_search_announce, short order_type)
{
	static SaleItemBase* item_list[SEARCH_SALE_ITEM_PER_PAGE];
	memset(item_list, 0, sizeof(item_list));
	int count = 0;
	int page = 0;
	int total_page = 0;
	m_sale_item_index.OnSearchByTypeWithLevel(is_search_announce, order_type, level, item_search_type, query_page, SEARCH_SALE_ITEM_PER_PAGE, item_list, &count, &page, &total_page);

	return this->SendSearchResult(role, count, item_list, page, total_page, order_type, is_search_announce, 0);
}

bool TradeMarketManager::OnBuyItemReq(Role* role, int seller_uid, short sale_index, short buy_num, unsigned int upshelve_time, bool is_pet, long long unique_key, ARG_OUT int* consume_gold)
{
	if (IsSystemSeller(seller_uid))
	{
		return this->OnBuySystemItemReq(role, buy_num, upshelve_time, unique_key, consume_gold);
	}

	UserTradeInfo* buyer_trade_info = this->GetUserTradeInfo(role->GetUID(), true);
	if (NULL == buyer_trade_info)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_UNKNOWN_ERROR);

		this->SendRefreshNotice(role, role->GetUID());
		return false;
	}

	UserTradeInfo* seller_trade_info = this->GetUserTradeInfo(seller_uid);
	if (NULL == seller_trade_info)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_SELLER_NOT_FOUND);
		
		this->SendRefreshNotice(role, role->GetUID());
		return false;
	}

	const UserSaleItem* usi = seller_trade_info->GetSaleItem(sale_index);
	if (NULL == usi)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND);
		
		this->SendRefreshNotice(role, role->GetUID());
		return false;
	}

	if (upshelve_time != usi->GetSaleItemParamConst().put_on_shelve_time) // 物品与实际搜索的物品不对应
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND);
		
		this->SendRefreshNotice(role, role->GetUID());
		return false;
	}

	if (SALE_ITEM_STATUS_ANNOUNCEMENT == usi->GetSaleItemParamConst().cur_status)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_ANNOUNCING);
		
		this->SendRefreshNotice(role, role->GetUID());
		return false;
	}

	if (SALE_ITEM_STATUS_ON_SALE != usi->GetSaleItemParamConst().cur_status) // 既不在公示期，也不在发售期，则可能是玩家作弊或系统错误
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_SOLD_OUT);
		
		this->SendRefreshNotice(role, role->GetUID());
		return false;
	}

	if (usi->GetSaleItemParamConst().item_wrapper.num < buy_num)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_NOT_ENOUGH);
		
		this->SendRefreshNotice(role, role->GetUID());
		return false;
	}

	ItemID item_id = usi->GetSaleItemParamConst().item_wrapper.item_id;
	const ItemBase* item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base)
	{
		return false;
	}

	int num_per_group = usi->GetSaleItemParamConst().num_per_group;
	if (0 == num_per_group)
	{
		if (buy_num % item_base->GetTradeNumPerGroup() != 0)
		{
#ifdef _DEBUG
			assert(0);
#endif
			this->SendNoticeNum(role->GetNetId(), errornum::EN_UNKNOWN_ERROR);
			return false;
		}
		num_per_group = item_base->GetTradeNumPerGroup();
	}

	int buy_group_num = buy_num / num_per_group;
	int single_price = usi->GetSaleItemParamConst().gold_price;
	int total_price = single_price * buy_group_num;
	int price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD;
	if (usi->GetSaleItemParamConst().gold_price <= 0)
	{
		single_price = usi->GetSaleItemParamConst().immortal_coin_price;
		total_price = single_price * buy_group_num;
		price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_IMMORTAL_COIN;
	}

	if (ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD == price_type)
	{
		if (!role->GetRoleModuleManager()->GetMoney()->GoldMoreThan(total_price))
		{
			this->SendNoticeNum(role->GetNetId(), errornum::EN_GOLD_NOT_ENOUGH);
			return false;
		}
	}
	else
	{
		if (!role->GetRoleModuleManager()->GetMoney()->ImmortalCoinMoreThan(total_price))
		{
			this->SendNoticeNum(role->GetNetId(), errornum::EN_IMMORTAL_COIN_NOT_ENOUGH);
			return false;
		}
	}

	if (Role::AUTHORITY_TYPE_TEST == role->GetAuthorityType())
	{		// 内玩购买限制
		if (total_price + role->GetRoleModuleManager()->GetCommonData().autority_type_test_day_buy_limit > GLOBALCONFIG->GetBuyLimit())
		{
			this->SendNoticeNum(role->GetNetId(), errornum::EN_AUTHORITY_TYPE_TEST_DAY_BUY_LIMIT);
			return false;
		}
		else
		{
			role->GetRoleModuleManager()->GetCommonData().autority_type_test_day_buy_limit += total_price;
			gamelog::g_log_trade_market.printf(LL_INFO, "%s role[%d, %s] today_trading_up[%d]", __FUNCTION__, role->GetUID(), role->GetName(), role->GetRoleModuleManager()->GetCommonData().autority_type_test_day_buy_limit);
		}
	}

	if (ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD == price_type)
	{
		if (!role->GetRoleModuleManager()->GetMoney()->UseGold(total_price, __FUNCTION__, false))
		{
			this->SendNoticeNum(role->GetNetId(), errornum::EN_GOLD_NOT_ENOUGH);
			return false;
		}
	}
	else
	{
		if (!role->GetRoleModuleManager()->GetMoney()->UseImmortalCoin(total_price, __FUNCTION__))
		{
			this->SendNoticeNum(role->GetNetId(), errornum::EN_IMMORTAL_COIN_NOT_ENOUGH);
			return false;
		}
	}
	
	static ItemDataWrapper wrapper;
	wrapper = usi->GetSaleItemParamConst().item_wrapper;
	wrapper.num = buy_num;

	if (seller_trade_info->OnItemSold(sale_index, buy_num, role->GetUID(), role->GetName(), role->GetPlatName()))
	{
		if (!this->GiveItemToRole(role, wrapper, __FUNCTION__, true))
		{
			gamelog::g_log_trade_market.printf(LL_INFO, "OnBuyItemReq, ERROR on GiveItemToRole, role[%d,%s] item[%d,%d]",
				role->GetUID(), role->GetName(), wrapper.item_id, wrapper.num);
			return false;
		}
		else
		{
#ifdef TRACK_ITEM_FLAG
			gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG TradeMarketManager::OnBuyItemReq buyer[%d,%s] seller[%d,%s] item[%d] item_unique_id[%lld] unique_key[%lld]",
				role->GetUID(), role->GetName(), seller_uid, usi->GetSellerName(), wrapper.item_id, wrapper.item_unique_id, usi->GetSaleItemParamConst().unique_key);
#endif
		}

		int param_length = NetValueItemParam::GetDataLengthByItemType(item_base->GetItemType());
		m_transaction_record.AddTransactionRecord(item_id, buy_num, price_type, single_price, param_length, wrapper.param_data);
		seller_trade_info->AddSoldRecord(item_id, buy_num, price_type, single_price, wrapper);
		buyer_trade_info->AddBuyRecord(item_id, buy_num, price_type, single_price, wrapper);

		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_trademark_buy_succ_content, item_id, price_type, buy_group_num * single_price);
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
			{
				EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
			}
		}
		if(single_price >= TRADEMARKET_CFG.GetConstantCfg().need_gold_notice)
		{
			length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_sold_succ_send_global_message, 
				item_base->GetItemName(), role->GetUID(), usi->GetSaleItemParamConst().sale_index,
				usi->GetSaleItemParamConst().put_on_shelve_time, single_price, item_base->GetItemId(), usi->GetSaleItemParamConst().put_on_shelve_time);
			if (length > 0)
			{
				Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
				{
					InternalComm::Instance().SendToGameAllUserThroughCross(role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType(), role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId(), (const char*)&sm, sendlen);
					World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
				}
			}
		}

		{
			// 通知卖家货架改变
			this->SendShelveChangeNotice(seller_uid, sale_index, Protocol::SCTradeMarketUserShelveData::REASON_NOTIFY_CHANGE);
		}

		gamelog::g_log_trade_market.printf(LL_INFO, "TradeMarketManager::OnBuyItemReq SUCC, buyer[%d,%s] seller[%d,%s] sale_index[%d], item[%d,%d] buy_num[%d] buy_group_num[%d] unique_key[%lld]",
			role->GetUID(), role->GetName(), usi->GetSellerUid(), usi->GetSellerName(), sale_index, usi->GetSaleItemParamConst().item_wrapper.item_id,
			usi->GetSaleItemParamConst().item_wrapper.num, buy_num, buy_group_num, unique_key);
	
		LOG_QUICK12(gamelog::g_log_quick,
			LOG_TYPE_TRADE_MARTKET_BUY, usi->GetSellerUid(), usi->GetSellerName(), NULL,
			usi->GetSaleItemParamConst().item_wrapper.item_id, usi->GetSaleItemParamConst().item_wrapper.num, role->GetPlatName(), role->GetName(),
			role->GetUID(), total_price, 0, 0, 0);
	}
	else
	{
		gamelog::g_log_trade_market.printf(LL_ERROR, "TradeMarketManager::OnBuyItemReq Failed, buyer[%d,%s] seller[%d,%s] sale_index[%d], item[%d,%d] buy_num[%d] unique_key[%lld]",
			role->GetUID(), role->GetName(), usi->GetSellerUid(), usi->GetSellerName(), sale_index, usi->GetSaleItemParamConst().item_wrapper.item_id,
			usi->GetSaleItemParamConst().item_wrapper.num, buy_num, unique_key);
	}

	if (NULL != consume_gold)
	{
		*consume_gold = total_price;
	}

	return true;
}

bool TradeMarketManager::OnBuySystemItemReq(Role* role, short buy_num, unsigned int upshelve_time, long long unique_key, ARG_OUT int* consume_gold)
{
	SystemTradeItemInfo* stii = m_sys_trade_info.GetTradeItemInfo(unique_key);
	if (NULL == stii)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_SELLER_NOT_FOUND);
		
		this->SendRefreshNotice(role, role->GetUID());
		return false;
	}

	const SystemSaleItem* ssi = stii->sale_item_ptr;
	if (NULL == ssi)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND);
		
		this->SendRefreshNotice(role, role->GetUID());
		return false;
	}

	UserTradeInfo* buyer_trade_info = this->GetUserTradeInfo(role->GetUID(), true);
	if (NULL == buyer_trade_info)
	{
		return false;
	}

	if (upshelve_time != ssi->GetSaleItemParamConst().put_on_shelve_time) // 物品与实际搜索的物品不对应
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND);
		
		this->SendRefreshNotice(role, role->GetUID());
		return false;
	}

	if (SALE_ITEM_STATUS_ANNOUNCEMENT == ssi->GetSaleItemParamConst().cur_status)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_ANNOUNCING);
		
		this->SendRefreshNotice(role, role->GetUID());
		return false;
	}

	if (SALE_ITEM_STATUS_ON_SALE != ssi->GetSaleItemParamConst().cur_status) // 既不在公示期，也不在发售期，则可能是玩家作弊或系统错误
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_SOLD_OUT);
		
		this->SendRefreshNotice(role, role->GetUID());
		return false;
	}

	if (ssi->GetSaleItemParamConst().item_wrapper.num < buy_num)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_NOT_ENOUGH);
		
		this->SendRefreshNotice(role, role->GetUID());
		return false;
	}

	ItemID item_id = ssi->GetSaleItemParamConst().item_wrapper.item_id;
	const ItemBase* item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base)
	{
		return false;
	}

	int num_per_group = ssi->GetSaleItemParamConst().num_per_group;
	if (0 == num_per_group)
	{
		if (buy_num % item_base->GetTradeNumPerGroup() != 0)
		{
#ifdef _DEBUG
			assert(0);
#endif
			this->SendNoticeNum(role->GetNetId(), errornum::EN_UNKNOWN_ERROR);
			return false;
		}
		num_per_group = item_base->GetTradeNumPerGroup();
	}

	int buy_group_num = buy_num / num_per_group;
	int single_price = ssi->GetSaleItemParamConst().gold_price;
	int total_price = single_price * buy_group_num;
	int price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD;
	if (ssi->GetSaleItemParamConst().gold_price <= 0)
	{
		single_price = ssi->GetSaleItemParamConst().immortal_coin_price;
		total_price = single_price * buy_group_num;
		price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_IMMORTAL_COIN;
	}

	if (ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD == price_type)
	{
		if (!role->GetRoleModuleManager()->GetMoney()->GoldMoreThan(total_price))
		{
			this->SendNoticeNum(role->GetNetId(), errornum::EN_GOLD_NOT_ENOUGH);
			return false;
		}
	}
	else
	{
		if (!role->GetRoleModuleManager()->GetMoney()->ImmortalCoinMoreThan(total_price))
		{
			this->SendNoticeNum(role->GetNetId(), errornum::EN_IMMORTAL_COIN_NOT_ENOUGH);
			return false;
		}
	}

	if (Role::AUTHORITY_TYPE_TEST == role->GetAuthorityType())
	{		// 内玩购买限制
		if (total_price + role->GetRoleModuleManager()->GetCommonData().autority_type_test_day_buy_limit > GLOBALCONFIG->GetBuyLimit())
		{
			this->SendNoticeNum(role->GetNetId(), errornum::EN_AUTHORITY_TYPE_TEST_DAY_BUY_LIMIT);
			return false;
		}
		else
		{
			role->GetRoleModuleManager()->GetCommonData().autority_type_test_day_buy_limit += total_price;
			gamelog::g_log_trade_market.printf(LL_INFO, "%s role[%d, %s] today_trading_up[%d]", __FUNCTION__, role->GetUID(), role->GetName(), role->GetRoleModuleManager()->GetCommonData().autority_type_test_day_buy_limit);
		}
	}

	if (ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD == price_type)
	{
		if (!role->GetRoleModuleManager()->GetMoney()->UseGold(total_price, __FUNCTION__, false))
		{
			this->SendNoticeNum(role->GetNetId(), errornum::EN_GOLD_NOT_ENOUGH);
			return false;
		}
	}
	else
	{
		if (!role->GetRoleModuleManager()->GetMoney()->UseImmortalCoin(total_price, __FUNCTION__))
		{
			this->SendNoticeNum(role->GetNetId(), errornum::EN_IMMORTAL_COIN_NOT_ENOUGH);
			return false;
		}
	}

	static ItemDataWrapper wrapper;
	wrapper = ssi->GetSaleItemParamConst().item_wrapper;
	wrapper.num = buy_num;

	if (m_sys_trade_info.OnItemSold(unique_key, buy_num, role->GetUID(), role->GetName(), role->GetPlatName()))
	{
		if (!this->GiveItemToRole(role, wrapper, __FUNCTION__, true))
		{
			gamelog::g_log_trade_market.printf(LL_INFO, "OnBuyItemReq, ERROR on GiveItemToRole, role[%d,%s] item[%d,%d]",
				role->GetUID(), role->GetName(), wrapper.item_id, wrapper.num);
			return false;
		}
		else
		{
#ifdef TRACK_ITEM_FLAG
			gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG TradeMarketManager::OnBuySystemItemReq buyer[%d,%s] seller[%d,%s] item[%d] item_unique_id[%lld] unique_key[%lld]",
				role->GetUID(), role->GetName(), ssi->GetSellerUid(), ssi->GetSellerName(), wrapper.item_id, wrapper.item_unique_id, ssi->GetSaleItemParamConst().unique_key);
#endif
		}

		int param_length = NetValueItemParam::GetDataLengthByItemType(item_base->GetItemType());
		m_transaction_record.AddTransactionRecord(item_id, buy_num, price_type, single_price, param_length, wrapper.param_data);
		//seller_trade_info->AddSoldRecord(item_id, buy_num, price_type, single_price, wrapper);
		buyer_trade_info->AddBuyRecord(item_id, buy_num, price_type, single_price, wrapper);

		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_trademark_buy_succ_content, item_id, price_type, buy_group_num * single_price);
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
			{
				EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
			}
		}
		if (single_price >= TRADEMARKET_CFG.GetConstantCfg().need_gold_notice)
		{
			length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_sold_succ_send_global_message,
				item_base->GetItemName(), role->GetUID(), ssi->GetSaleItemParamConst().sale_index,
				ssi->GetSaleItemParamConst().put_on_shelve_time, single_price, item_base->GetItemId(), ssi->GetSaleItemParamConst().put_on_shelve_time);
			
			if (length > 0)
			{
				Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
				{
					InternalComm::Instance().SendToGameAllUserThroughCross(role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType(), role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId(), (const char*)&sm, sendlen);
					World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
				}
			}
		}

		gamelog::g_log_trade_market.printf(LL_INFO, "TradeMarketManager::OnBuySystemItemReq SUCC, buyer[%d,%s] unique_key[%lld], item[%d] cur_num[%d] buy_num[%d] buy_group_num[%d]",
			role->GetUID(), role->GetName(), unique_key, ssi->GetSaleItemParamConst().item_wrapper.item_id,
			ssi->GetSaleItemParamConst().item_wrapper.num, buy_num, buy_group_num);
	}
	else
	{
		gamelog::g_log_trade_market.printf(LL_WARNING, "TradeMarketManager::OnBuySystemItemReq Failed, buyer[%d,%s] unique_key[%lld], item[%d] cur_num[%d] buy_num[%d]",
			role->GetUID(), role->GetName(), unique_key, ssi->GetSaleItemParamConst().item_wrapper.item_id,
			ssi->GetSaleItemParamConst().item_wrapper.num, buy_num);
	}

	if (NULL != consume_gold)
	{
		*consume_gold = total_price;
	}

	return true;
}

bool TradeMarketManager::OnAutoBuyItemReq(Role* role, ItemID buy_item_id, short buy_num, int expect_price, ARG_OUT int* consume_gold)
{
	const ItemBase* item_base = ITEMPOOL->GetItem(buy_item_id);
	if (NULL == item_base) return false;

	int sum_consume_gold = 0;

	//自动购买
	std::vector<SaleItemBase*> item_list;
	if (!m_sale_item_index.OnSearchForAutoBuy(buy_item_id, buy_num, expect_price, &item_list))
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND_FOR_AUTO_BUY);
		
		this->SendRefreshNotice(role, role->GetUID());
		return false;
	}

	UserTradeInfo* buyer_trade_info = this->GetUserTradeInfo(role->GetUID(), true);
	if (NULL == buyer_trade_info)
	{
		return false;
	}

	int need_buy_num = buy_num;
	if ((int)item_list.size() <= 0)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_AUTOBUY_EXCEED_EXPECT_PRICE);
		
		this->SendRefreshNotice(role, role->GetUID());
		return false;
	}
	for (size_t i = 0; i < item_list.size() && need_buy_num > 0; ++i)
	{
		SaleItemBase* sib = item_list[i];

		bool is_system_seller = IsSystemSeller(sib->GetSellerUid());
		if (!is_system_seller)
		{
			UserTradeInfo* seller_trade_info = this->GetUserTradeInfo(sib->GetSellerUid());
			if (NULL == seller_trade_info) continue;
		}

		int cur_buy_num = need_buy_num;
		int num_per_group = sib->GetSaleItemParamConst().num_per_group;
		if (0 == num_per_group)
		{
			if (need_buy_num % item_base->GetTradeNumPerGroup() != 0)
			{
#ifdef _DEBUG
				assert(0);
#endif
				this->SendNoticeNum(role->GetNetId(), errornum::EN_UNKNOWN_ERROR);
				return false;
			}
			num_per_group = item_base->GetTradeNumPerGroup();
		}

		if (sib->GetSaleItemParamConst().item_wrapper.num < cur_buy_num)
		{
			cur_buy_num = sib->GetSaleItemParamConst().item_wrapper.num;
			if (cur_buy_num % num_per_group != 0)
			{
				cur_buy_num = cur_buy_num / num_per_group * num_per_group;
			}
			if (cur_buy_num <= 0)
			{
				continue;
			}
		}

		int buy_group_num = cur_buy_num / num_per_group;
		ItemID item_id = sib->GetSaleItemParamConst().item_wrapper.item_id;
		int single_price = sib->GetSaleItemParamConst().gold_price;
		int total_price = single_price * buy_group_num;
		int price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD;
		if (sib->GetSaleItemParamConst().gold_price <= 0)
		{
			single_price = sib->GetSaleItemParamConst().immortal_coin_price;
			total_price = single_price * buy_group_num;
			price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_IMMORTAL_COIN;
		}

		if (ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD == price_type)
		{
			if (!role->GetRoleModuleManager()->GetMoney()->UseGold(total_price, is_system_seller ? "TradeMarketManager::OnAutoBuyItemReq_SYSTEM" : "TradeMarketManager::OnAutoBuyItemReq_USER", false))
			{
				this->SendNoticeNum(role->GetNetId(), errornum::EN_GOLD_NOT_ENOUGH);
				break;
			}

			sum_consume_gold += total_price;
		}
		else
		{
			if (!role->GetRoleModuleManager()->GetMoney()->UseImmortalCoin(total_price, is_system_seller ? "TradeMarketManager::OnAutoBuyItemReq_SYSTEM" : "TradeMarketManager::OnAutoBuyItemReq_USER", false))
			{
				this->SendNoticeNum(role->GetNetId(), errornum::EN_IMMORTAL_COIN_NOT_ENOUGH);
				break;
			}
		}

		static ItemDataWrapper wrapper;
		wrapper = sib->GetSaleItemParamConst().item_wrapper;
		wrapper.num = cur_buy_num;

		int sale_index = sib->GetSaleItemParamConst().sale_index;
		bool is_sold_succ = false;
		if (is_system_seller)
		{
			is_sold_succ = m_sys_trade_info.OnItemSold(sib->GetSaleItemParamConst().unique_key, cur_buy_num, role->GetUID(), role->GetName(), role->GetPlatName());
		}
		else
		{
			UserTradeInfo* seller_trade_info = this->GetUserTradeInfo(sib->GetSellerUid());
			if (NULL != seller_trade_info)
			{
				is_sold_succ = seller_trade_info->OnItemSold(sale_index, cur_buy_num, role->GetUID(), role->GetName(), role->GetPlatName());
			}
		}
		if (is_sold_succ)
		{			
			if (!this->GiveItemToRole(role, wrapper, __FUNCTION__, true))
			{
				gamelog::g_log_trade_market.printf(LL_INFO, "OnAutoBuyItemReq, ERROR on GiveItemToRole, role[%d,%s] item[%d,%d]",
					role->GetUID(), role->GetName(), wrapper.item_id, wrapper.num);
				return false;
			}
			else
			{
#ifdef TRACK_ITEM_FLAG
				gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG TradeMarketManager::OnAutoBuyItemReq buyer[%d,%s] seller[%d,%s] item[%d] item_unique_id[%lld] unique_key[%lld]",
					role->GetUID(), role->GetName(), sib->GetSellerUid(), sib->GetSellerName(), wrapper.item_id, wrapper.item_unique_id, sib->GetSaleItemParamConst().unique_key);
#endif
			}

			int param_length = NetValueItemParam::GetDataLengthByItemType(item_base->GetItemType());
			m_transaction_record.AddTransactionRecord(item_id, cur_buy_num, price_type, single_price, param_length, wrapper.param_data);
			
			if (!IsSystemSeller(sib->GetSellerUid()))
			{
				UserTradeInfo* seller_trade_info = this->GetUserTradeInfo(sib->GetSellerUid());
				if (NULL != seller_trade_info)
				{
					seller_trade_info->AddSoldRecord(item_id, cur_buy_num, price_type, single_price, wrapper);
				}
			}
			
			buyer_trade_info->AddBuyRecord(item_id, cur_buy_num, price_type, single_price, wrapper);

			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_trademark_buy_succ_content, item_id, price_type, buy_group_num * single_price);
			if (length > 0)
			{
				Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
				{
					EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
				}
			}
			
			if (single_price >= TRADEMARKET_CFG.GetConstantCfg().need_gold_notice)
			{
				length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_sold_succ_send_global_message,
					item_base->GetItemName(), role->GetUID(), sib->GetSaleItemParamConst().sale_index,
					sib->GetSaleItemParamConst().put_on_shelve_time, single_price, item_base->GetItemId(), sib->GetSaleItemParamConst().put_on_shelve_time);
				if (length > 0)
				{
					Protocol::SCSystemMsg sm;
					int sendlen = 0;
					if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
					{
						InternalComm::Instance().SendToGameAllUserThroughCross(role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType(), role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId(), (const char*)&sm, sendlen);
						World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
					}
				}
			}

			if (!IsSystemSeller(sib->GetSellerUid()))
			{
				this->SendShelveChangeNotice(sib->GetSellerUid(), sale_index, Protocol::SCTradeMarketUserShelveData::REASON_NOTIFY_CHANGE);
				
				gamelog::g_log_trade_market.printf(LL_ERROR, "TradeMarketManager::OnAutoBuyItemReq SUCC, buyer[%d,%s] seller[%d,%s] sale_index[%d], item[%d,%d] buy_num[%d] buy_group_num[%d]",
					role->GetUID(), role->GetName(), sib->GetSellerUid(), sib->GetSellerName(), sale_index, sib->GetSaleItemParamConst().item_wrapper.item_id,
					sib->GetSaleItemParamConst().item_wrapper.num, cur_buy_num, buy_group_num);
			}
			else
			{
				gamelog::g_log_trade_market.printf(LL_ERROR, "TradeMarketManager::OnAutoBuyItemReq SUCC, buyer[%d,%s] seller[system] unique_key[%lld], item[%d,%d] buy_num[%d] buy_group_num[%d]",
					role->GetUID(), role->GetName(), sib->GetSaleItemParamConst().unique_key, sib->GetSaleItemParamConst().item_wrapper.item_id,
					sib->GetSaleItemParamConst().item_wrapper.num, cur_buy_num, buy_group_num);
			}
			

			need_buy_num -= cur_buy_num;
		}
		else
		{
			if (!IsSystemSeller(sib->GetSellerUid()))
			{
				gamelog::g_log_trade_market.printf(LL_ERROR, "TradeMarketManager::OnAutoBuyItemReq Failed, buyer[%d,%s] seller[%d,%s] sale_index[%d], item[%d] cur_num[%d] buy_num[%d]",
					role->GetUID(), role->GetName(), sib->GetSellerUid(), sib->GetSellerName(), sale_index, sib->GetSaleItemParamConst().item_wrapper.item_id,
					sib->GetSaleItemParamConst().item_wrapper.num, cur_buy_num);
			}
			else
			{
				gamelog::g_log_trade_market.printf(LL_ERROR, "TradeMarketManager::OnAutoBuyItemReq Failed, buyer[%d,%s] seller[system] unique_key[%lld], item[%d] cur_num[%d] buy_num[%d]",
					role->GetUID(), role->GetName(), sib->GetSaleItemParamConst().unique_key, sib->GetSaleItemParamConst().item_wrapper.item_id,
					sib->GetSaleItemParamConst().item_wrapper.num, cur_buy_num);
			}

			return false;
		}
	}

	if (NULL != consume_gold)
	{
		*consume_gold = sum_consume_gold;
	}

	return true;
}

bool TradeMarketManager::OnGetTransactionRecordsReq(Role* role, ItemID item_id)
{
	Protocol::SCRecentTransactionRecords resp;
	resp.count = m_transaction_record.GetTransactionRecords(item_id, TRANSACTION_RECORD_MAX_NUM, resp.record_list);

	EngineAdapter::Instance().NetSendSerializeMsg(role->GetNetId(), &resp);
	//int send_len = sizeof(resp) - (TRANSACTION_RECORD_MAX_NUM - resp.count) * sizeof(resp.record_list[0]);
	//EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&resp, send_len);

	return true;
}

bool TradeMarketManager::OnAddFocusReq(Role* role, int seller_uid, int sale_index, unsigned int upshelve_time)
{
	UserTradeInfo* seller_info = this->GetUserTradeInfo(seller_uid);
	if (NULL == seller_info)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_FOCUS_ITEM_NOT_FOUND);
		return false;
	}

	const UserSaleItem* seller_usi = seller_info->GetSaleItem(sale_index);
	if (NULL == seller_usi)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_FOCUS_ITEM_NOT_FOUND);
		return false;
	}

	const ItemBase* item = ITEMPOOL->GetItem(seller_usi->GetSaleItemParamConst().item_wrapper.item_id);
	if (NULL == item)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_UNKNOWN_ERROR);
		return false;
	}

	if (item->GetItemType() != I_TYPE_EQUIPMENT && item->GetItemType() != I_TYPE_PET && item->GetItemType() != I_TYPE_JEWELRY)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_TYPE_CANNOT_FOCUS);
		return false;
	}

	if (seller_usi->GetSaleItemParamConst().cur_status != SALE_ITEM_STATUS_ANNOUNCEMENT &&
		seller_usi->GetSaleItemParamConst().cur_status != SALE_ITEM_STATUS_ON_SALE)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_FOCUS_ITEM_NOT_FOUND);
		return false;
	}

	if (seller_usi->GetSaleItemParamConst().put_on_shelve_time != upshelve_time)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_FOCUS_ITEM_NOT_FOUND);
		return false;
	}

	UserTradeInfo* role_info = this->GetUserTradeInfo(role->GetUID(), true);
	if (NULL == role_info)
	{
		return false;
	}

	int empty_index = 0;
	if (!role_info->GetEmptyFocusIndex(&empty_index))
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_FOCUS_ITEM_NOT_FOUND);
		return false;
	}

	if (!role_info->OnAddFocusItem(empty_index, seller_uid, seller_usi->GetSellerName(), seller_usi->GetSaleItemParamConst(), false))
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_FOCUS_FAILED);
		return false;
	}

	this->OnGetFocusListReq(role);

	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_trade_market_focus_success);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role->GetUserId(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
		}
	}

	return true;
}

bool TradeMarketManager::OnRemoveFocusReq(Role* role, int focus_index, short cur_page, bool is_announce)
{
	UserTradeInfo* info = this->GetUserTradeInfo(role->GetUID());
	if (NULL == info)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_UNKNOWN_ERROR);
		return false;
	}

	if (!info->OnRemoveFocusItem(focus_index))
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_UNFOCUS_FAILED);
		return false;
	}

	this->OnGetFocusListReq(role); // 删除成功则刷新当前页的数据

	return true;
}

bool TradeMarketManager::OnGetFocusListReq(Role* role)
{
	Protocol::SCTradeMarketGetFocusList list;
	list.sale_count = 0;
	list.announce_count = 0;

	this->MakeUserFocusListData(role->GetUID(), &list);
	return EngineAdapter::Instance().NetSendSerializeMsg(role->GetNetId(), &list);
}

bool TradeMarketManager::OnGetFocusItemInfoReq(Role* role, int focus_index, int page, bool is_announce)
{
	UserTradeInfo* info = this->GetUserTradeInfo(role->GetUID());
	if (NULL == info)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_UNKNOWN_ERROR);
		return false;
	}

	const UserSaleItem* usi = info->GetFocusItem(focus_index);
	if (NULL == usi)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_UNKNOWN_ERROR);
		return false;
	}

	UserTradeInfo* seller_info = this->GetUserTradeInfo(focus_index);
	const UserSaleItem* seller_usi = NULL;
	if (NULL == seller_info || NULL == (seller_usi = seller_info->GetSaleItem(usi->GetSaleItemParamConst().sale_index)))
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_FOCUS_ITEM_OFFSHELVE);
		this->OnRemoveFocusReq(role, focus_index, page, is_announce);
		return true;
	}
	else
	{	
		if (is_announce && SALE_ITEM_STATUS_ON_SALE == seller_usi->GetSaleItemParamConst().cur_status)
		{
			this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_FOCUS_ITEM_ON_SALE);
			this->OnRemoveFocusReq(role, focus_index, page, is_announce);
			return true;
		}
		else
		{
			Protocol::SCTradeMarketGetFocusItemInfo resp;
			this->MakeFocusItemInfoData(seller_usi, focus_index, &resp);

			EngineAdapter::Instance().NetSendSerializeMsg(role->GetNetId(), &resp);
		}
	}

	return true;
}

bool TradeMarketManager::OnAdvertiseReq(Role* role, int sale_index)
{
	UserTradeInfo* info = this->GetUserTradeInfo(role->GetUID());
	if (NULL == info)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_UNKNOWN_ERROR);
		return false;
	}

	const UserSaleItem* usi = info->GetSaleItem(sale_index);
	if (NULL == usi)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_UNKNOWN_ERROR);
		return false;
	}
	const ItemBase* item = ITEMPOOL->GetItem(usi->GetSaleItemParamConst().item_wrapper.item_id);
	if (NULL == item) return false;

	if (!usi->IsAnnouncing() && !usi->IsOnSale())
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_NOT_ANOUNCE_OR_SALE);
		return false;
	}

	short service_fee_rate = TRADEMARKET_CFG.GetConstantCfg().advertise_fee_rate;
	long long total_price = 0;
	long long service_fee = 0;
	if (usi->GetSaleItemParamConst().gold_price > 0)
	{
		total_price = 1LL * usi->GetSaleItemParamConst().gold_price * usi->GetSaleItemParamConst().item_wrapper.num;
		service_fee = static_cast<long long>(1.0 * total_price * service_fee_rate / TRADE_MARKET_PERCENT_NUM);
		service_fee = GetMax(static_cast<long long>(TRADEMARKET_CFG.GetConstantCfg().gold_advertise_fee_min), service_fee);
		service_fee = GetMin(static_cast<long long>(TRADEMARKET_CFG.GetConstantCfg().gold_advertise_fee_max), service_fee);

		if (!role->GetRoleModuleManager()->GetMoney()->GoldMoreThan(service_fee))
		{
			this->SendNoticeNum(role->GetNetId(), errornum::EN_GOLD_NOT_ENOUGH);
			return false;
		}

		if (!role->GetRoleModuleManager()->GetMoney()->UseGold(service_fee, __FUNCTION__, false))
		{
			this->SendNoticeNum(role->GetNetId(), errornum::EN_GOLD_NOT_ENOUGH);
			return false;
		}
	}
	else
	{
		total_price = 1LL * usi->GetSaleItemParamConst().immortal_coin_price * usi->GetSaleItemParamConst().item_wrapper.num;
		service_fee = static_cast<long long>(1.0 * total_price * service_fee_rate / TRADE_MARKET_PERCENT_NUM);
		service_fee = GetMax(static_cast<long long>(TRADEMARKET_CFG.GetConstantCfg().immortal_coin_advertise_fee_min), service_fee);
		service_fee = GetMin(static_cast<long long>(TRADEMARKET_CFG.GetConstantCfg().immortal_coin_advertise_fee_max), service_fee);

		if (!role->GetRoleModuleManager()->GetMoney()->CoinBindMoreThan(service_fee))
		{
			this->SendNoticeNum(role->GetNetId(), errornum::EN_COIN_NOT_ENOUGH);
			return false;
		}

		if (!role->GetRoleModuleManager()->GetMoney()->UseCoinBind(service_fee, __FUNCTION__))
		{
			this->SendNoticeNum(role->GetNetId(), errornum::EN_COIN_NOT_ENOUGH);
			return false;
		}
	}

	//发公告
	int price = usi->GetSaleItemParamConst().gold_price > 0 ? usi->GetSaleItemParamConst().gold_price : usi->GetSaleItemParamConst().immortal_coin_price;
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_trade_market_advertise, 
		item->GetItemName(), role->GetUID(), usi->GetSaleItemParamConst().sale_index, 
		usi->GetSaleItemParamConst().put_on_shelve_time, price, item->GetItemId(), usi->GetSaleItemParamConst().unique_key, usi->GetSaleItemParamConst().put_on_shelve_time);
	if (length > 0)
	{
		Protocol::SCSystemMsg sm;
		int sendlen = 0;
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
		{
			InternalComm::Instance().SendToGameAllUserThroughCross(role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType(), role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId(), (const char*)&sm, sendlen);
			World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
		}
	}

	return true;
}

bool TradeMarketManager::OnSearchByAdvertiseReq(Role* role, int seller_uid, short sale_index, unsigned int upshelve_time, int price, ItemID item_id, short item_sub_type, long long unique_key)
{
	UserTradeInfo* seller_info = this->GetUserTradeInfo(seller_uid);
	if (NULL == seller_info)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND);
		return false;
	}
	
	const UserSaleItem* seller_usi = seller_info->GetSaleItem(sale_index);
	if (NULL == seller_usi || seller_usi->GetSaleItemParamConst().item_wrapper.item_id != item_id ||
		seller_usi->GetSaleItemParamConst().put_on_shelve_time != upshelve_time || seller_usi->IsOffShelve())
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND);
		return false;
	}

	static SaleItemBase* item_list[SEARCH_SALE_ITEM_PER_PAGE];
	memset(item_list, 0, sizeof(item_list));
	int count = 0;
	int page = 0;
	int total_page = 0;
	short cursor_index = 0;
	if (!m_sale_item_index.OnSearchByAdvertise(seller_usi->IsAnnouncing(), item_id, price, upshelve_time, seller_uid, unique_key, SEARCH_SALE_ITEM_PER_PAGE, item_list, &count, &page, &total_page, &cursor_index))
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND);
		return false;
	}

	return this->SendSearchResult(role, count, item_list, page, total_page, TRADE_ITEM_ORDER_PRICE_UP, seller_usi->IsAnnouncing(), cursor_index);
}

bool TradeMarketManager::OnSearchSystemByAdvertiseReq(Role* role, int seller_uid, short sale_index, unsigned int upshelve_time, int price, ItemID item_id, short item_sub_type, long long unique_key)
{
	const SystemTradeItemInfo* ssi = m_sys_trade_info.GetTradeItemInfo(unique_key);
	if (NULL == ssi || ssi->sale_item_ptr->GetSaleItemParamConst().item_wrapper.item_id != item_id ||
		ssi->sale_item_ptr->GetSaleItemParamConst().put_on_shelve_time != upshelve_time || ssi->sale_item_ptr->IsOffShelve())
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND);
		return false;
	}

	static SaleItemBase* item_list[SEARCH_SALE_ITEM_PER_PAGE];
	memset(item_list, 0, sizeof(item_list));
	int count = 0;
	int page = 0;
	int total_page = 0;
	short cursor_index = 0;
	if (!m_sale_item_index.OnSearchByAdvertise(ssi->sale_item_ptr->IsAnnouncing(), item_id, price, upshelve_time, seller_uid, unique_key, SEARCH_SALE_ITEM_PER_PAGE, item_list, &count, &page, &total_page, &cursor_index))
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND);
		return false;
	}

	return this->SendSearchResult(role, count, item_list, page, total_page, TRADE_ITEM_ORDER_PRICE_UP, ssi->sale_item_ptr->IsAnnouncing(), cursor_index);
}

bool TradeMarketManager::OnGetSellingListReq(Role* role, ItemID item_id, short special_effect_type)
{
	const ItemBase* item = ITEMPOOL->GetItem(item_id);
	if (NULL == item) return false;

	static SaleItemBase* usi_list[TRANSACTION_RECORD_MAX_NUM];
	int count = 0;
	
	if (I_TYPE_MEDAL == item->GetItemType())
	{
		m_sale_item_index.OnSearchForMedalSellingList(special_effect_type, TRANSACTION_RECORD_MAX_NUM, usi_list, &count);
	}
	else
	{
		m_sale_item_index.OnSearchForSellingList(item_id, TRANSACTION_RECORD_MAX_NUM, usi_list, &count);
	}
	
	Protocol::SCTradeMarketGetSellingList resp;
	resp.count = 0;
	for (int i = 0; i < count && i < TRANSACTION_RECORD_MAX_NUM; ++i)
	{
		const ItemBase* item = ITEMPOOL->GetItem(usi_list[i]->GetSaleItemParamConst().item_wrapper.item_id);
		
		if (NULL == item) continue;
		if (usi_list[i]->GetSaleItemParamConst().sold_num >= usi_list[i]->GetSaleItemParamConst().item_wrapper.num) continue;

		resp.selling_list[resp.count].item_id = usi_list[i]->GetSaleItemParamConst().item_wrapper.item_id;
		if (usi_list[i]->GetSaleItemParamConst().gold_price > 0)
		{
			resp.selling_list[resp.count].price = usi_list[i]->GetSaleItemParamConst().gold_price;
			resp.selling_list[resp.count].price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD;
		}
		else
		{
			resp.selling_list[resp.count].price = usi_list[i]->GetSaleItemParamConst().immortal_coin_price;
			resp.selling_list[resp.count].price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_IMMORTAL_COIN;
		}

		int param_length = NetValueItemParam::GetDataLengthByItemType(item->GetItemType());
		resp.selling_list[resp.count].data_len = param_length;
		memcpy(resp.selling_list[resp.count].data, usi_list[i]->GetSaleItemParamConst().item_wrapper.param_data, sizeof(NetValueItemParamData));

		resp.selling_list[resp.count].item_num = usi_list[i]->GetSaleItemParamConst().item_wrapper.num - usi_list[i]->GetSaleItemParamConst().sold_num;
		resp.count += 1;
	}

	EngineAdapter::Instance().NetSendSerializeMsg(role->GetNetId(), &resp);

	return true;
}

bool TradeMarketManager::OnGetRoleTradeRecordReq(Role* role)
{
	Protocol::SCTradeMarketGetRoleTransactionRecord resp;
	resp.buy_record_num = 0;
	resp.sold_record_num = 0;
	resp.total_buy_gold = 0LL;
	resp.total_sold_gold = 0LL;

	this->MakeUserTradeRecordData(role->GetUID(), &resp);
	
	return EngineAdapter::Instance().NetSendSerializeMsg(role->GetNetId(), &resp);
}

bool TradeMarketManager::OnRedoAddItemOnShelveReq(Role* role, short sale_index, int price)
{
	if (price <= 0)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	UserTradeInfo* info = this->GetUserTradeInfo(role->GetUID());
	if (NULL == info)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_REDO_ADD_ITEM_ON_SHELVE_FAILED);
		return false;
	}

	const UserSaleItem* sale_item = info->GetSaleItem(sale_index);
	if (NULL == sale_item)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_REDO_ADD_ITEM_ON_SHELVE_FAILED);
		return false;
	}

	if (!sale_item->IsOffShelve())
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_NOT_OFF_SHELVE);
		return false;
	}

	if (sale_item->GetSaleItemParamConst().sold_num > 0)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_CASH_OUT_FIRST);
		return false;
	}

	if (sale_item->GetSaleItemParamConst().item_wrapper.num <= 0)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_SOLD_OUT);
		return false;
	}

	const ItemBase* item_base = ITEMPOOL->GetItem(sale_item->GetSaleItemParamConst().item_wrapper.item_id);
	if (NULL == item_base)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_REDO_ADD_ITEM_ON_SHELVE_FAILED);
		return false;
	}

	if (!item_base->CanTrade())
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_ITEM_CAN_NOT_TRADE);
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_REDO_ADD_ITEM_ON_SHELVE_FAILED);
		return false;
	}

	if (item_base->GetTradeNumPerGroup() != sale_item->GetSaleItemParamConst().num_per_group)
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_ITEM_NUM_PER_GROUP_CHANGED);
		return false;
	}

	if (sale_item->GetSaleItemParamConst().gold_price > TradeMarketManager::GetTradeMaxPrice(item_base, &sale_item->GetSaleItemParamConst().item_wrapper) ||
		sale_item->GetSaleItemParamConst().gold_price < TradeMarketManager::GetTradeMinPrice(item_base, &sale_item->GetSaleItemParamConst().item_wrapper))
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_PRICE_NOT_IN_RANGE);
		return false;
	}

	int errnum = 0;
	if (!this->CanSellItem(role->GetUID(), sale_item->GetSaleItemParamConst(), &errnum))
	{
		this->SendNoticeNum(role->GetNetId(), errnum);
		return false;
	}

	if (!info->OnItemRedoAddOnShelve(sale_index, price))
	{
		this->SendNoticeNum(role->GetNetId(), errornum::EN_TRADE_MARKET_REDO_ADD_ITEM_ON_SHELVE_FAILED);
		return false;
	}
	else
	{
		this->SendShelveChangeNotice(role->GetUID(), sale_index, Protocol::SCTradeMarketUserShelveData::REASON_NOTIFY_CHANGE);
	}

	return true;
}

bool TradeMarketManager::OnSearchPetForQuickBuyReq(Role* role, short order_type, ItemID item_id, short quality, int last_seller_uid, int last_price, unsigned int last_put_on_shelve_time, short last_str_level)
{
	static SaleItemBase* item_list[SEARCH_PET_QUICK_BUY_ITEM_NUM];
	memset(item_list, 0, sizeof(item_list));
	int count = 0;

	if (item_id > 0)
	{
		static SalePetIndex index;
		index.item_id = item_id;
		index.price = last_price;
		index.put_on_shelve_time = last_put_on_shelve_time;
		index.uid = last_seller_uid;
		index.pet_str_level = last_str_level;

		m_sale_item_index.OnSearchPetForQuickBuy(order_type, index, SEARCH_PET_QUICK_BUY_ITEM_NUM, item_list, &count);
	}
	else
	{
		static SalePetQualityIndex index;
		index.quality = quality;
		index.price = last_price;
		index.put_on_shelve_time = last_put_on_shelve_time;
		index.uid = last_seller_uid;
		index.pet_str_level = last_str_level;

		m_sale_item_index.OnSearchPetQualityForQuickBuy(order_type, index, SEARCH_PET_QUICK_BUY_ITEM_NUM, item_list, &count);
	}
	
	return this->SendSearchPetForQuickBuyResult(role, order_type, count, item_list);
}

bool TradeMarketManager::OnSearchMedalWithColor(Role* role, short medal_color, short query_page, bool is_search_announce, short order_type, int prof_param /* = -1 */)
{
	static SaleItemBase* item_list[SEARCH_SALE_ITEM_PER_PAGE];
	memset(item_list, 0, sizeof(item_list));
	int count = 0;
	int page = 0;
	int total_page = 0;
	if (-1 == prof_param)
	{
		m_sale_item_index.OnSearchByMedalColor(is_search_announce, order_type, medal_color, query_page, SEARCH_SALE_ITEM_PER_PAGE, item_list, &count, &page, &total_page);
	}
	else
	{
		m_sale_item_index.OnSearchByMedalColorAndProf(is_search_announce, order_type, medal_color, prof_param, query_page, SEARCH_SALE_ITEM_PER_PAGE, item_list, &count, &page, &total_page);
	}

	return this->SendSearchResult(role, count, item_list, page, total_page, order_type, is_search_announce, 0);
}

bool TradeMarketManager::OnGetMerchandiseNumReq(Role* role, short big_search_type, bool is_announce)
{
	Protocol::SCTradeMarketGetMerchandiseNum msg;
	msg.num = 0;
	msg.big_search_type = big_search_type;
	msg.is_announce = is_announce ? 1 : 0;

	static int search_type_list[TRADE_ITEM_SEARCH_TYPE_MAX] = { 0 };
	int search_type_num = TradeMarketConfig::Instance().GetSmallSearchTypeList(big_search_type, search_type_list, TRADE_ITEM_SEARCH_TYPE_MAX);

	for (int i = 0; i < search_type_num; ++i)
	{
		msg.info_list[msg.num].small_search_type = search_type_list[i];
		msg.info_list[msg.num].merchandise_num = m_sale_item_index.GetMerchandiseNum(search_type_list[i], is_announce);
		msg.num += 1;
	}

	int send_len = sizeof(msg) - sizeof(msg.info_list) + msg.num * sizeof(msg.info_list[0]);
	return EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&msg, send_len);
}

bool TradeMarketManager::OnSearchMedalByEffectTypeAndColorReq(Role* role, short sp_type, short medal_color, short query_page, bool is_search_announce, short order_type)
{
	static SaleItemBase* item_list[SEARCH_SALE_ITEM_PER_PAGE];
	memset(item_list, 0, sizeof(item_list));
	int count = 0;
	int page = 0;
	int total_page = 0;
	m_sale_item_index.OnSearchByMedalEffectTypeAndColor(is_search_announce, order_type, sp_type, medal_color, query_page, SEARCH_SALE_ITEM_PER_PAGE, item_list, &count, &page, &total_page);

	return this->SendSearchResult(role, count, item_list, page, total_page, order_type, is_search_announce, 0);
}

void TradeMarketManager::NewVersionAutoOffshelve(const UserID& user_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	UserTradeInfo* uti = this->GetUserTradeInfo(UidToInt(user_id));
	if (NULL == uti) return;

	int count = 0;
	long long total_cashout_gold = 0LL;
	long long total_return_credit = 0LL;
	static MailContentParam mcp;
	mcp.Reset();
	
	for (int i = 0; i < USER_SALE_ITEM_MAX_NUM; ++i)
	{
		const UserSaleItem* usi = uti->GetSaleItem(i);
		if (NULL == usi)
		{
			continue;
		}

		long long add_gold = 0;
		if (uti->OnCashOutItem(i, &add_gold))
		{
			total_cashout_gold += add_gold;
		}

		usi = uti->GetSaleItem(i);
		if (NULL == usi)
		{
			continue; // 已售罄并提现完毕
		}

		static ItemDataWrapper wrapper;
		wrapper = usi->GetSaleItemParamConst().item_wrapper;
		int gold_price = usi->GetSaleItemParamConst().gold_price;
		int group_num = wrapper.num / usi->GetSaleItemParamConst().num_per_group;
		long long return_credit_fee = 1LL * gold_price * group_num;
		if (uti->OnRemoveSaleItem(i, __FUNCTION__))
		{
			total_return_credit += return_credit_fee;
			UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(mcp.item_list) >= USER_SALE_ITEM_MAX_NUM);
			mcp.item_list[count++] = wrapper;
		}
	}

	if (0 == count && 0 == total_cashout_gold && 0 == total_return_credit)
	{
		// 没有需要提现下架的物品，直接返回
		return;
	}

	int lensub = SNPRINTF(mcp.subject, sizeof(mcp.subject), gamestring::g_new_version_auto_offshelve_subject);
	int length = SNPRINTF(mcp.contenttxt, sizeof(mcp.contenttxt), gamestring::g_new_version_auto_offshelve_content);
	if (lensub > 0 && length > 0)
	{
		mcp.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] = static_cast<int>(total_cashout_gold);
		mcp.virtual_item_list.virtual_item[MAIL_VIRTUAL_TRADE_CREDIT] = static_cast<int>(total_return_credit);
		MailRoute::Instance().MailToUser(user_id, MAIL_REASON_DEFAULT, mcp);
	}
}

bool TradeMarketManager::GiveItemToRole(Role* role, const ItemDataWrapper& item_data, const char* reason, bool is_from_buy /* = false */)
{
	if (item_data.num <= 0) return false;

	const ItemBase* item = ITEMPOOL->GetItem(item_data.item_id);
	if (NULL == item) return false;
 
	if (I_TYPE_PET == item->GetItemType())
	{
		if (role->GetRoleModuleManager()->GetPet()->HaveSpace(1, false))
		{
			static NetValueItemParam param;
			param.Reset();
			param.SetData(item_data.param_data);
			if (is_from_buy)
			{
				param.pet_param.source = PET_SOURCE_TYPE_TRANSACTION;
				param.pet_param.fei_sheng_task_jie_duan = param.pet_param.fei_sheng_times * PET_JIE_DUAN_NUM_PER_FEI_SHENG_TIMES;
			}
			role->GetRoleModuleManager()->GetPet()->AcquirePetWithParam(&param, __FUNCTION__, false, false, item_data.item_unique_id, is_from_buy);
		}
		else
		{
			static MailContentParam contentparam; contentparam.Reset();
			contentparam.item_list[0] = item_data;

			if (is_from_buy)
			{
				static NetValueItemParam param;
				param.Reset();
				param.SetData(contentparam.item_list[0].param_data);
				param.pet_param.source = PET_SOURCE_TYPE_TRANSACTION;
				param.pet_param.fei_sheng_task_jie_duan = param.pet_param.fei_sheng_times * PET_JIE_DUAN_NUM_PER_FEI_SHENG_TIMES;
				param.GetData(contentparam.item_list[0].param_data);
			}		

			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_pet_full_mail_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_pet_full_send_mail);
			MailRoute::MailToUser(IntToUid(role->GetUID()), MAIL_REASON_DEFAULT, contentparam);

			role->NoticeNum(noticenum::NT_KNAPSACKFULL_SEND_MAIL);

			gamelog::g_log_trade_market.printf(LL_INFO, "TradeMarketManager::GivePetToRole by mail, role[%d,%s] pet_id[%d] num[%d] reason[%s]",
				role->GetUID(), role->GetName(), item_data.item_id, item_data.num, reason);
		}
	}
	else
	{
		if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(item_data.item_id, item_data.num))
		{
			role->GetRoleModuleManager()->GetKnapsack()->Put(item_data, is_from_buy ? PUT_REASON_TRADE_MARKET_BUY : PUT_REASON_TRADE_MARKET_OTHER);

			gamelog::g_log_trade_market.printf(LL_INFO, "TradeMarketManager::GiveItemToRole by knapsack, role[%d,%s] item_id[%d] num[%d] reason[%s]",
				role->GetUID(), role->GetName(), item_data.item_id, item_data.num, reason);
		}
		else
		{
			static MailContentParam contentparam; contentparam.Reset();
			contentparam.item_list[0] = item_data;

			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_knapsack_full_mail_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_knapsack_full_send_mail);
			MailRoute::MailToUser(IntToUid(role->GetUID()), MAIL_REASON_DEFAULT, contentparam);

			role->NoticeNum(noticenum::NT_KNAPSACKFULL_SEND_MAIL);

			gamelog::g_log_trade_market.printf(LL_INFO, "TradeMarketManager::GiveItemToRole by mail, role[%d,%s] item_id[%d] num[%d] reason[%s]",
				role->GetUID(), role->GetName(), item_data.item_id, item_data.num, reason);
		}
	}

	return true;
}

bool TradeMarketManager::GiveMoneyToRole(Role* role, long long gold, long long immortal_coin, long long bind_coin, const char* reason)
{
	bool result = true;
	if (gold > 0)
	{
		result = result && role->GetRoleModuleManager()->GetMoney()->AddGold(gold, reason);
	}

	if (bind_coin > 0)
	{
		result = result && role->GetRoleModuleManager()->GetMoney()->AddCoinBind(bind_coin, reason);
	}

	if (immortal_coin > 0)
	{
		result = result && role->GetRoleModuleManager()->GetMoney()->AddImmortalCoin(immortal_coin, reason);
	}

	gamelog::g_log_trade_market.printf(LL_INFO, "TradeMarketManager::GiveMoneyToRole role[%d,%s] gold[%lld] immortal_coin[%lld] bind_coin[%lld] reason[%s]",
		role->GetUID(), role->GetName(), gold, immortal_coin, bind_coin, reason);

	return result;
}

bool TradeMarketManager::GiveResourceToUser(const UserID& user_id, const ItemDataWrapper* item_data, int gold, int bind_coin, int trade_credit, const char* reason, bool is_from_buy /* = false */)
{
	bool is_item_valid = (NULL != item_data && !item_data->Invalid());

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL != role)
	{
		bool ret = true;
		if (is_item_valid)
		{
			ret = ret && this->GiveItemToRole(role, *item_data, reason, is_from_buy);
		}
		
		if (gold > 0 || bind_coin > 0)
		{
			ret = ret && this->GiveMoneyToRole(role, gold, 0, bind_coin, reason);
		}

		if (trade_credit > 0)
		{
			ret = ret && role->GetRoleModuleManager()->GetTradeCredit()->AddCredit(trade_credit, reason);
		}

		return ret;
	}
	else
	{
		static MailContentParam contentparam; contentparam.Reset();
		if (NULL != item_data && !item_data->Invalid())
		{
			contentparam.item_list[0] = *item_data;
			const ItemBase* item = ITEMPOOL->GetItem(item_data->item_id);
			if (NULL != item && I_TYPE_PET == item->GetItemType() && is_from_buy)
			{
				static NetValueItemParam param;
				param.Reset();
				param.SetData(contentparam.item_list[0].param_data);
				param.pet_param.source = PET_SOURCE_TYPE_TRANSACTION; // 设置
				param.pet_param.fei_sheng_task_jie_duan = param.pet_param.fei_sheng_times * PET_JIE_DUAN_NUM_PER_FEI_SHENG_TIMES;
				param.GetData(contentparam.item_list[0].param_data);
			}
		}
		if (gold > 0) contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] = gold;
		if (bind_coin > 0) contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = bind_coin;
		if (trade_credit > 0) contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_TRADE_CREDIT] = trade_credit;

		SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_trademarket_give_item_to_user_offline_subject);
		SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_trademarket_give_item_to_user_offline_content);
		MailRoute::MailToUser(user_id, MAIL_REASON_DEFAULT, contentparam);

		gamelog::g_log_trade_market.printf(LL_INFO, "TradeMarketManager::GiveResourceToUser by mail, role_id[%d] item_id[%d] num[%d] gold[%d] bind_coin[%d] credit[%d] reason[%s]",
			UidToInt(user_id), is_item_valid ? item_data->item_id : 0, is_item_valid ? item_data->num : 0, gold, bind_coin, trade_credit, reason);

		return true;
	}
}

bool TradeMarketManager::LoadData(long long id_offset)
{
	if (GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH == m_load_data_state) return false;

	RMIInitTradeMarketDataBackObjectImpl *impl = new RMIInitTradeMarketDataBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitTradeMarketDataAsync(id_offset, false, impl);
}

bool TradeMarketManager::LoadSystemShelve(long long id_offset)
{
	if (GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH == m_load_system_shelve_state) return false;

	RMIInitTradeMarketDataBackObjectImpl *impl = new RMIInitTradeMarketDataBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitTradeMarketDataAsync(id_offset, true, impl);
}

bool TradeMarketManager::LoadUserRecord(long long id_offset)
{
	if (GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH == m_load_user_record_state) return false;

	RMIInitUserTradeRecordBackObjectImpl *impl = new RMIInitUserTradeRecordBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitTradeMarketUserRecordAsync(id_offset, impl);
}

bool TradeMarketManager::LoadSystemRecord(long long id_offset)
{
	if (GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH == m_load_system_record_state) return false;

	RMIInitSystemTradeRecordBackObjectImpl *impl = new RMIInitSystemTradeRecordBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitTradeMarketSystemRecordAsync(id_offset, impl);
}

bool TradeMarketManager::LoadSystemRecycleRule(long long id_offset)
{
	if (GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH == m_load_system_recycle_rule_state) return false;

	RMIInitSystemRecycleRuleBackObjectImpl *impl = new RMIInitSystemRecycleRuleBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitTradeMarketSystemRecycleRuleAsync(id_offset, impl);
}

bool TradeMarketManager::LoadSystemAutoUpshelve(long long id_offset)
{
	if (GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH == m_load_system_auto_upshelve_state) return false;

	RMIInitSystemAutoUpshelveBackObjectImpl *impl = new RMIInitSystemAutoUpshelveBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitSystemAutoUpshelveDataAsync(id_offset, impl);
}

void RMIInitSystemRecycleRuleBackObjectImpl::InitRet(int ret, const SystemRecycleRuleList& data)
{
	if (ret < 0 || data.count < 0 || data.count > SystemRecycleRuleList::MAX_RULE_NUM)
	{
		printf(" RMIInitSystemRecycleRuleBackObjectImpl::Init Failed result[%d] data.count[%d]\n", ret, data.count);
		gamelog::g_log_trade_market.printf(LL_ERROR, "RMIInitSystemRecycleRuleBackObjectImpl::InitRet ret[%d] data.count[%d]", ret, data.count);
		ServerLogic::GetInstServerLogic()->StopServer("RMIInitSystemRecycleRuleBackObjectImpl::InitRet_1");
		return;
	}

	long long next_id_from = 0;

	for (int i = 0; i < data.count; ++i)
	{
		next_id_from = GetMax(next_id_from, data.rule_list[i].id);

		ItemID item_id = data.rule_list[i].param.item_id;
		long long rule_id = data.rule_list[i].param.rule_id;

		bool ret = TradeMarketManager::Instance().m_system_recycle.AddRecycleRule(data.rule_list[i].param, true);
		if (!ret)
		{
			gamelog::g_log_trade_market.printf(LL_WARNING, "RMIInitSystemRecycleRuleBackObjectImpl::InitRet init recycle rule failed!\
				item_id[%d] rule_id[%lld]",
				item_id, rule_id);
			continue;
		}
	}

	if (data.count > 0)
	{
		TradeMarketManager::Instance().LoadSystemRecycleRule(next_id_from);
	}
	else
	{
		TradeMarketManager::Instance().OnLoadSystemRecycleRuleSucc();
	}
}

void RMIInitSystemAutoUpshelveBackObjectImpl::InitRet(int ret, const AutoUpshelveDataList& data)
{
	if (ret < 0 || data.count < 0 || data.count > AutoUpshelveDataList::MAX_AUTO_UPSHELVE_ITEM_NUM)
	{
		printf(" RMIInitSystemAutoUpshelveBackObjectImpl::Init Failed result[%d] data.count[%d]\n", ret, data.count);
		gamelog::g_log_trade_market.printf(LL_ERROR, "RMIInitSystemAutoUpshelveBackObjectImpl::InitRet ret[%d] data.count[%d]", ret, data.count);
		ServerLogic::GetInstServerLogic()->StopServer("RMIInitSystemAutoUpshelveBackObjectImpl::InitRet_1");
		return;
	}

	long long next_id_from = 0;

	for (int i = 0; i < data.count; ++i)
	{
		next_id_from = GetMax(next_id_from, data.data_list[i].id);

		ItemID item_id = data.data_list[i].param.item_id;
		long long unique_id = data.data_list[i].param.unique_id;
		bool ret = TradeMarketManager::Instance().m_auto_upshelve.AddAutoUpshelveInfo(data.data_list[i].param, true);
		if (!ret)
		{
			gamelog::g_log_trade_market.printf(LL_WARNING, "RMIInitSystemAutoUpshelveBackObjectImpl::InitRet init upshelve info failed! item_id[%d] unique_id[%lld]", item_id, unique_id);
			continue;
		}
	}

	if (data.count > 0)
	{
		TradeMarketManager::Instance().LoadSystemAutoUpshelve(next_id_from);
	}
	else
	{
		TradeMarketManager::Instance().OnLoadSystemAutoUpshelveSucc();
	}
}

void TradeMarketManager::OnLoadDataSucc()
{
	gamelog::g_log_trade_market.printf(LL_INFO, "TradeMarketManager::OnLoadDataSucc at time[%u]", 
		static_cast<unsigned int>(EngineAdapter::Instance().Time()));
	
	m_load_data_state = GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH;
	if (this->IsLoadFinish())
	{
		this->OnAllLoadSucc();
	}
}

void TradeMarketManager::OnLoadUserRecordSucc()
{
	gamelog::g_log_trade_market.printf(LL_INFO, "TradeMarketManager::OnLoadUserRecordSucc at time[%u]",
		static_cast<unsigned int>(EngineAdapter::Instance().Time()));

	m_load_user_record_state = GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH;
	if (this->IsLoadFinish())
	{
		this->OnAllLoadSucc();
	}
}

void TradeMarketManager::OnLoadSystemRecordSucc()
{
	gamelog::g_log_trade_market.printf(LL_INFO, "TradeMarketManager::OnLoadSystemRecordSucc at time[%u]",
		static_cast<unsigned int>(EngineAdapter::Instance().Time()));

	m_load_system_record_state = GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH;
	if (this->IsLoadFinish())
	{
		this->OnAllLoadSucc();
	}

	m_transaction_record.OnInitFinish();
}

void TradeMarketManager::OnLoadSystemShelveSucc()
{
	gamelog::g_log_trade_market.printf(LL_INFO, "TradeMarketManager::OnLoadSystemShelveSucc at time[%u]",
		static_cast<unsigned int>(EngineAdapter::Instance().Time()));

	m_load_system_shelve_state = GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH;
	if (this->IsLoadFinish())
	{
		this->OnAllLoadSucc();
	}
}

void TradeMarketManager::OnLoadSystemRecycleRuleSucc()
{
	gamelog::g_log_trade_market.printf(LL_INFO, "TradeMarketManager::OnLoadSystemRecycleRuleSucc at time[%u]",
		static_cast<unsigned int>(EngineAdapter::Instance().Time()));

	m_load_system_recycle_rule_state = GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH;
	if (this->IsLoadFinish())
	{
		this->OnAllLoadSucc();
	}

	m_system_recycle.OnInitFinish();
}

void TradeMarketManager::OnLoadSystemAutoUpshelveSucc()
{
	gamelog::g_log_trade_market.printf(LL_INFO, "TradeMarketManager::OnLoadSystemAutoUpshelveSucc at time[%u]",
		static_cast<unsigned int>(EngineAdapter::Instance().Time()));

	m_load_system_auto_upshelve_state = GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH;
	if (this->IsLoadFinish())
	{
		this->OnAllLoadSucc();
	}

	m_auto_upshelve.OnInitFinish();
}

void TradeMarketManager::OnAllLoadSucc()
{
	gamelog::g_log_world.printf(LL_INFO, "TradeMarketManager OnAllLoadSucc");
}

void RMIInitTradeMarketDataBackObjectImpl::InitRet(int result, const TradeMarketData& data)
{
	if (result < 0 || data.count < 0 || data.count > TradeMarketData::MAX_SALE_ITEM_COUNT)
	{
		printf(" RMIInitTradeMarketDataBackObjectImpl::Init Failed result[%d] data.count[%d]\n",
			result, data.count);
		gamelog::g_log_trade_market.printf(LL_ERROR, "RMIInitTradeMarketDataBackObjectImpl::InitRet ret[%d] data.count[%d]",
			result, data.count);
		ServerLogic::GetInstServerLogic()->StopServer("RMIInitTradeMarketDataBackObjectImpl::InitRet_1");
		return;
	}

	if (!data.is_system_shelve)
	{
		long long next_id_from = 0;

		for (int i = 0; i < data.count; ++i)
		{
			next_id_from = GetMax(next_id_from, data.sale_list[i].id_saleitem);

			if (data.sale_list[i].owner_uid <= 0)
			{
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[RMIInitTradeMarketDataBackObjectImpl::InitRet INVALID USER INFO] uid[%d]",
					data.sale_list[i].owner_uid);
				continue;
			}

			UserTradeInfo *user_trade_info = TradeMarketManager::Instance().GetUserTradeInfo(data.sale_list[i].owner_uid, true);
			if (NULL == user_trade_info)
			{
				continue;
			}

			if (1 != data.sale_list[i].is_focus)
			{
				user_trade_info->InitSaleItem(data.sale_list[i]);
			}
			else
			{
				user_trade_info->InitFocusItem(data.sale_list[i]);
			}
		}

		if (data.count > 0)
		{
			TradeMarketManager::Instance().LoadData(next_id_from);
		}
		else
		{
			TradeMarketManager::Instance().OnLoadDataSucc();
		}
	}
	else
	{
		long long next_id_from = 0;

		for (int i = 0; i < data.count; ++i)
		{
			next_id_from = GetMax(next_id_from, data.sale_list[i].id_saleitem);
			TradeMarketManager::Instance().m_sys_trade_info.InitSaleItem(data.sale_list[i]);
		}

		if (data.count > 0)
		{
			TradeMarketManager::Instance().LoadSystemShelve(next_id_from);
		}
		else
		{
			TradeMarketManager::Instance().OnLoadSystemShelveSucc();
		}
	}
	
}

void RMIInitUserTradeRecordBackObjectImpl::InitRet(int ret, const UserTradeRecordData& data)
{
	if (ret < 0 || data.count < 0 || data.count > UserTradeRecordData::MAX_USER_RECORD_NUM)
	{
		printf(" RMIInitUserTradeRecordBackObjectImpl::Init Failed result[%d] data.count[%d]\n",
			ret, data.count);
		gamelog::g_log_trade_market.printf(LL_ERROR, "RMIInitUserTradeRecordBackObjectImpl::InitRet ret[%d] data.count[%d]",
			ret, data.count);
		ServerLogic::GetInstServerLogic()->StopServer("RMIInitUserTradeRecordBackObjectImpl::InitRet_1");
		return;
	}

	long long next_id_from = 0;

	for (int i = 0; i < data.count; ++i)
	{
		next_id_from = GetMax(next_id_from, data.record_list[i].id);

		if (data.record_list[i].role_id <= 0)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[RMIInitUserTradeRecordBackObjectImpl::InitRet INVALID USER INFO] uid[%d]",
				data.record_list[i].role_id);
			continue;
		}

		UserTradeInfo *user_trade_info = TradeMarketManager::Instance().GetUserTradeInfo(data.record_list[i].role_id, true);
		if (NULL == user_trade_info)
		{
			continue;
		}

		user_trade_info->InitRoleRecordParam(data.record_list[i].other_param, data.record_list[i].sold_record, data.record_list[i].buy_record);
	}

	if (data.count > 0)
	{
		TradeMarketManager::Instance().LoadUserRecord(next_id_from);
	}
	else
	{
		TradeMarketManager::Instance().OnLoadUserRecordSucc();
	}
}

void RMIInitSystemTradeRecordBackObjectImpl::InitRet(int ret, const SystemTradeRecord& data)
{
	if (ret < 0 || data.count < 0 || data.count > SystemTradeRecord::MAX_SYSTEM_RECORD_NUM)
	{
		printf(" RMIInitSystemTradeRecordBackObjectImpl::Init Failed result[%d] data.count[%d]\n", ret, data.count);
		gamelog::g_log_trade_market.printf(LL_ERROR, "RMIInitSystemTradeRecordBackObjectImpl::InitRet ret[%d] data.count[%d]", ret, data.count);
		ServerLogic::GetInstServerLogic()->StopServer("RMIInitSystemTradeRecordBackObjectImpl::InitRet_1");
		return;
	}

	long long next_id_from = 0;

	for (int i = 0; i < data.count; ++i)
	{
		next_id_from = GetMax(next_id_from, data.record_list[i].id);

		ItemID item_id = data.record_list[i].ti.item_id;
		const ItemBase* item = ITEMPOOL->GetItem(item_id);
		if (NULL == item || !item->CanTrade())
		{
			gamelog::g_log_trade_market.printf(LL_WARNING, "[RMIInitSystemTradeRecordBackObjectImpl::InitRet item_not_tradable] item_id[%d]", item_id);
			continue;
		}

		const SystemTradeRecord::SystemRecord& record = data.record_list[i];
		if (!TradeMarketManager::Instance().m_transaction_record.InitTransactionRecord(record.index, item_id,
			record.ti.item_num, record.ti.price_type, record.ti.price, record.transaction_time, record.ti.data_len, record.ti.data))
		{
			gamelog::g_log_trade_market.printf(LL_WARNING, "RMIInitSystemTradeRecordBackObjectImpl::InitRet init transaction record failed!\
				item_id[%d] item_num[%d] index[%d]", 
				item_id, record.ti.item_num, record.index);
			continue;
		}
	}

	if (data.count > 0)
	{
		TradeMarketManager::Instance().LoadSystemRecord(next_id_from);
	}
	else
	{
		TradeMarketManager::Instance().OnLoadSystemRecordSucc();
	}
}

void TradeMarketManager::Release()
{
	UserTradeInfoMap::iterator it = m_user_trade_info_map.begin();
	for (; it != m_user_trade_info_map.end(); ++it)
	{
		if (NULL != it->second)
		{
			delete it->second;
			it->second = NULL;
		}
	}
}

void TradeMarketManager::SaveAll()
{
	UserTradeInfoMap::iterator it = m_user_trade_info_map.begin();
	for (; it != m_user_trade_info_map.end(); ++it)
	{
		if (NULL != it->second)
		{
			it->second->SaveDB();
		}
	}
	m_transaction_record.SaveDB();
	m_sys_trade_info.SaveDB();
	m_system_recycle.SaveDB();
	m_auto_upshelve.SaveDB();
}

void TradeMarketManager::SendNoticeNum(const GSNetID& netid, int notice_num)
{
	Protocol::SCNoticeNum msg;
	msg.notice_num = notice_num;
	EngineAdapter::Instance().NetSend(netid, (const char*)&msg, sizeof(msg));
}

void TradeMarketManager::SendNoticeNumToOriginRole(int plat_type, int server_id, long long uuid, int notice_num)
{
	UniqueUserID uuser_id = LongLongToUniqueUserID(uuid);
	int uid = UidToInt(uuser_id.user_id);

	Protocol::SCNoticeNum msg;
	msg.notice_num = notice_num;
	InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, (const char*)&msg, sizeof(msg), uid);
}

void TradeMarketManager::SendNoticeNumToOriginRole(int uid, int notice_num)
{
	int plat_type = LocalConfig::Instance().GetPlatType();
	int server_id = IntToUid(uid).db_index;

	Protocol::SCNoticeNum msg;
	msg.notice_num = notice_num;
	InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, (const char*)&msg, sizeof(msg), uid);
}

void TradeMarketManager::SendShelveData(Role* role)
{
	Protocol::SCTradeMarketUserShelveData msg;
	this->MakeShelveData(role->GetUID(), &msg);

	EngineAdapter::Instance().NetSendSerializeMsg(role->GetNetId(), &msg);
}

void TradeMarketManager::SendShelveDataToOriginRole(int uid)
{
	Protocol::SCTradeMarketUserShelveData msg;
	this->MakeShelveData(uid, &msg);

	InternalComm::Instance().SendSerializeMsgToGameUserThroughCross(uid, &msg);
}

void TradeMarketManager::SendRefreshNotice(Role* role, int uid)
{
	if (NULL != role)
	{
	Protocol::SCTradeMarketRefreshNotice notice;
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&notice, sizeof(notice));
	}
	else if (CrossConfig::Instance().IsHiddenServer())
	{
		Protocol::SCTradeMarketRefreshNotice notice;
		InternalComm::Instance().SendToGameUserThroughCross(uid, &notice, sizeof(notice));
	}
}

void TradeMarketManager::SendShelveChangeNotice(int uid, int sale_index, short reason)
{
	UserTradeInfo* user_trade_info = this->GetUserTradeInfo(uid);
	if (NULL == user_trade_info) return;

	Protocol::SCTradeMarketUserShelveData msg;
	msg.item_count = 0;
	msg.reason = reason;

	const UserSaleItem* si = user_trade_info->GetSaleItem(sale_index);
	if (NULL != si)
	{
		const SaleItemParam& sip = si->GetSaleItemParamConst();
		msg.item_list[msg.item_count].sale_index = sale_index;
		msg.item_list[msg.item_count].cur_status = sip.cur_status;
		msg.item_list[msg.item_count].next_status_time = sip.next_status_time;
		if (sip.gold_price > 0)
		{
			msg.item_list[msg.item_count].price = sip.gold_price;
			msg.item_list[msg.item_count].price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD;
		}
		else
		{
			msg.item_list[msg.item_count].price = sip.immortal_coin_price;
			msg.item_list[msg.item_count].price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_IMMORTAL_COIN;
		}
		msg.item_list[msg.item_count].sold_num = sip.sold_num;

		msg.item_list[msg.item_count].item_detail.item_id = sip.item_wrapper.item_id;
		msg.item_list[msg.item_count].item_detail.item_num = sip.item_wrapper.num;
		msg.item_list[msg.item_count].item_detail.invalid_time = sip.item_wrapper.invalid_time;
		msg.item_list[msg.item_count].item_detail.param_length = 0;
		const ItemBase* item = ITEMPOOL->GetItem(sip.item_wrapper.item_id);
		if (NULL != item)
		{
			int param_length = NetValueItemParam::GetDataLengthByItemType(item->GetItemType());
			msg.item_list[msg.item_count].item_detail.param_length = param_length;
			memcpy(msg.item_list[msg.item_count].item_detail.data, sip.item_wrapper.param_data, param_length);
		}
		msg.item_count += 1;
	}
	else
	{
		msg.reason = Protocol::SCTradeMarketUserShelveData::REASON_NOTIFY_REMOVE;

		msg.item_list[msg.item_count].sale_index = sale_index;
		msg.item_list[msg.item_count].cur_status = 0;
		msg.item_list[msg.item_count].next_status_time = 0;
		msg.item_list[msg.item_count].price = 0;
		msg.item_list[msg.item_count].price_type = 0;
		msg.item_list[msg.item_count].sold_num = 0;

		msg.item_list[msg.item_count].item_detail.item_id = 0;
		msg.item_list[msg.item_count].item_detail.item_num = 0;
		msg.item_list[msg.item_count].item_detail.invalid_time = 0;
		msg.item_list[msg.item_count].item_detail.param_length = 0;
		
		msg.item_count += 1;
	}

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(uid));
	if (NULL != role)
	{
		EngineAdapter::Instance().NetSendSerializeMsg(role->GetNetId(), &msg);
	}
	else
	{
		if (CrossConfig::Instance().IsHiddenServer())
		{
			// 发到原服
			InternalComm::Instance().SendSerializeMsgToGameUserThroughCross(uid, &msg);
		}
		else
		{
			// 原服不在线，不发了
		}
	}
}

void TradeMarketManager::OnOriginUserGetShelveDataReq(int plat_type, int server_id, long long uuid)
{
	UniqueUserID uuser_id = LongLongToUniqueUserID(uuid);
	int uid = UidToInt(uuser_id.user_id);
	
	Protocol::SCTradeMarketUserShelveData msg;
	this->MakeShelveData(uid, &msg);
	int send_len = 0;
	const char* data = EngineAdapter::Instance().GetSerializeData(&msg, &send_len);
	InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, data, send_len, uid);
}

void TradeMarketManager::OnOriginUserSearchItemByIdReq(int plat_type, int server_id, long long uuid, ItemID item_id, short query_page, bool is_search_announce, short order_type)
{
	UniqueUserID uuser_id = LongLongToUniqueUserID(uuid);
	int uid = UidToInt(uuser_id.user_id);

	static SaleItemBase* item_list[SEARCH_SALE_ITEM_PER_PAGE];
	memset(item_list, 0, sizeof(item_list));
	int count = 0;
	int page = 0;
	int total_page = 0;
	m_sale_item_index.OnSearchByItem(is_search_announce, order_type, item_id, query_page, SEARCH_SALE_ITEM_PER_PAGE, item_list, &count, &page, &total_page);

	Protocol::SCTradeMarketSearchResult result;
	result.item_count = 0;
	this->MakeSearchResult(&result, count, item_list, page, total_page, order_type, is_search_announce, 0);

	int send_len = 0;
	const char* data = EngineAdapter::Instance().GetSerializeData(&result, &send_len);
	InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, data, send_len, uid);
}

void TradeMarketManager::OnOriginUserSearchItemByTypeReq(int plat_type, int server_id, long long uuid, short item_search_type, short query_page, bool is_search_announce, short order_type)
{
	UniqueUserID uuser_id = LongLongToUniqueUserID(uuid);
	int uid = UidToInt(uuser_id.user_id);

	static SaleItemBase* item_list[SEARCH_SALE_ITEM_PER_PAGE];
	memset(item_list, 0, sizeof(item_list));
	int count = 0;
	int page = 0;
	int total_page = 0;
	m_sale_item_index.OnSearchByType(is_search_announce, order_type, item_search_type, query_page, SEARCH_SALE_ITEM_PER_PAGE, item_list, &count, &page, &total_page);

	Protocol::SCTradeMarketSearchResult result;
	result.item_count = 0;
	this->MakeSearchResult(&result, count, item_list, page, total_page, order_type, is_search_announce, 0);

	int send_len = 0;
	const char* data = EngineAdapter::Instance().GetSerializeData(&result, &send_len);
	InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, data, send_len, uid);
}

void TradeMarketManager::OnOriginUserSearchItemByBigTypeReq(int plat_type, int server_id, long long uuid, short item_search_big_type, short query_page, bool is_search_announce, short order_type)
{
	UniqueUserID uuser_id = LongLongToUniqueUserID(uuid);
	int uid = UidToInt(uuser_id.user_id);

	static SaleItemBase* item_list[SEARCH_SALE_ITEM_PER_PAGE];
	memset(item_list, 0, sizeof(item_list));
	int count = 0;
	int page = 0;
	int total_page = 0;
	m_sale_item_index.OnSearchByBigType(is_search_announce, order_type, item_search_big_type, query_page, SEARCH_SALE_ITEM_PER_PAGE, item_list, &count, &page, &total_page);

	Protocol::SCTradeMarketSearchResult result;
	result.item_count = 0;
	this->MakeSearchResult(&result, count, item_list, page, total_page, order_type, is_search_announce, 0);

	int send_len = 0;
	const char* data = EngineAdapter::Instance().GetSerializeData(&result, &send_len);
	InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, data, send_len, uid);
}

void TradeMarketManager::OnOriginUserSearchItemByTypeWithLevelReq(int plat_type, int server_id, long long uuid, short level, short item_search_type, short query_page, bool is_search_announce, short order_type)
{
	UniqueUserID uuser_id = LongLongToUniqueUserID(uuid);
	int uid = UidToInt(uuser_id.user_id);

	static SaleItemBase* item_list[SEARCH_SALE_ITEM_PER_PAGE];
	memset(item_list, 0, sizeof(item_list));
	int count = 0;
	int page = 0;
	int total_page = 0;
	m_sale_item_index.OnSearchByTypeWithLevel(is_search_announce, order_type, level, item_search_type, query_page, SEARCH_SALE_ITEM_PER_PAGE, item_list, &count, &page, &total_page);

	Protocol::SCTradeMarketSearchResult result;
	result.item_count = 0;
	this->MakeSearchResult(&result, count, item_list, page, total_page, order_type, is_search_announce, 0);

	int send_len = 0;
	const char* data = EngineAdapter::Instance().GetSerializeData(&result, &send_len);
	InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, data, send_len, uid);
}

void TradeMarketManager::OnOriginUserSearchMedalByColorReq(int plat_type, int server_id, long long uuid, short medal_color, short query_page, bool is_search_announce, short order_type, int prof_param /* = -1 */)
{
	UniqueUserID uuser_id = LongLongToUniqueUserID(uuid);
	int uid = UidToInt(uuser_id.user_id);

	static SaleItemBase* item_list[SEARCH_SALE_ITEM_PER_PAGE];
	memset(item_list, 0, sizeof(item_list));
	int count = 0;
	int page = 0;
	int total_page = 0;
	if (-1 == prof_param)
	{
		m_sale_item_index.OnSearchByMedalColor(is_search_announce, order_type, medal_color, query_page, SEARCH_SALE_ITEM_PER_PAGE, item_list, &count, &page, &total_page);
	}
	else
	{
		m_sale_item_index.OnSearchByMedalColorAndProf(is_search_announce, order_type, medal_color, prof_param, query_page, SEARCH_SALE_ITEM_PER_PAGE, item_list, &count, &page, &total_page);
	}

	Protocol::SCTradeMarketSearchResult result;
	result.item_count = 0;
	this->MakeSearchResult(&result, count, item_list, page, total_page, order_type, is_search_announce, 0);

	int send_len = 0;
	const char* data = EngineAdapter::Instance().GetSerializeData(&result, &send_len);
	InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, data, send_len, uid);
}

void TradeMarketManager::OnOriginUserSearchMedalByEffectTypeAndColor(int plat_type, int server_id, long long uuid, short sp_type, short medal_color, short query_page, bool is_search_announce, short order_type)
{
	UniqueUserID uuser_id = LongLongToUniqueUserID(uuid);
	int uid = UidToInt(uuser_id.user_id);

	static SaleItemBase* item_list[SEARCH_SALE_ITEM_PER_PAGE];
	memset(item_list, 0, sizeof(item_list));
	int count = 0;
	int page = 0;
	int total_page = 0;
	m_sale_item_index.OnSearchByMedalEffectTypeAndColor(is_search_announce, order_type, sp_type, medal_color, query_page, SEARCH_SALE_ITEM_PER_PAGE, item_list, &count, &page, &total_page);

	Protocol::SCTradeMarketSearchResult result;
	result.item_count = 0;
	this->MakeSearchResult(&result, count, item_list, page, total_page, order_type, is_search_announce, 0);

	int send_len = 0;
	const char* data = EngineAdapter::Instance().GetSerializeData(&result, &send_len);
	InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, data, send_len, uid);
}

void TradeMarketManager::OnOriginUserGetTransactionRecordReq(int plat_type, int server_id, long long uuid, ItemID item_id)
{
	UniqueUserID uuser_id = LongLongToUniqueUserID(uuid);
	int uid = UidToInt(uuser_id.user_id);

	Protocol::SCRecentTransactionRecords resp;
	resp.count = m_transaction_record.GetTransactionRecords(item_id, TRANSACTION_RECORD_MAX_NUM, resp.record_list);

	int send_len = 0;
	const char* data = EngineAdapter::Instance().GetSerializeData(&resp, &send_len);
	InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, data, send_len, uid);
}

void TradeMarketManager::OnOriginUserGetRoleTradeRecordReq(int plat_type, int server_id, long long uuid)
{
	UniqueUserID uuser_id = LongLongToUniqueUserID(uuid);
	int uid = UidToInt(uuser_id.user_id);

	Protocol::SCTradeMarketGetRoleTransactionRecord resp;
	resp.buy_record_num = 0;
	resp.sold_record_num = 0;
	resp.total_buy_gold = 0LL;
	resp.total_sold_gold = 0LL;

	this->MakeUserTradeRecordData(uid, &resp);

	int send_len = 0;
	const char* data = EngineAdapter::Instance().GetSerializeData(&resp, &send_len);
	InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, data, send_len, uid);
}

void TradeMarketManager::OnOriginUserGetRoleFocusListReq(int plat_type, int server_id, long long uuid)
{
	UniqueUserID uuser_id = LongLongToUniqueUserID(uuid);
	int uid = UidToInt(uuser_id.user_id);

	Protocol::SCTradeMarketGetFocusList list;
	list.sale_count = 0;
	list.announce_count = 0;

	this->MakeUserFocusListData(uid, &list);

	int send_len = 0;
	const char* data = EngineAdapter::Instance().GetSerializeData(&list, &send_len);
	InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, data, send_len, uuid);
}

void TradeMarketManager::OnOriginUserGetFocusItemInfoReq(int plat_type, int server_id, long long uuid, int focus_index, int page, bool is_announce)
{
	UniqueUserID uuser_id = LongLongToUniqueUserID(uuid);
	int uid = UidToInt(uuser_id.user_id);

	UserTradeInfo* info = this->GetUserTradeInfo(uid);
	if (NULL == info)
	{
		this->SendNoticeNumToOriginRole(plat_type, server_id, uuid, errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const UserSaleItem* usi = info->GetFocusItem(focus_index);
	if (NULL == usi)
	{
		this->SendNoticeNumToOriginRole(plat_type, server_id, uuid, errornum::EN_UNKNOWN_ERROR);
		return;
	}

	UserTradeInfo* seller_info = this->GetUserTradeInfo(focus_index);
	const UserSaleItem* seller_usi = NULL;
	if (NULL == seller_info || NULL == (seller_usi = seller_info->GetSaleItem(usi->GetSaleItemParamConst().sale_index)))
	{
		this->SendNoticeNumToOriginRole(plat_type, server_id, uuid, errornum::EN_TRADE_MARKET_FOCUS_ITEM_OFFSHELVE);
		this->OnOriginUserRemoveFocusReq(plat_type, server_id, uuid, focus_index, page, is_announce);
		return;
	}
	else
	{
		if (is_announce && SALE_ITEM_STATUS_ON_SALE == seller_usi->GetSaleItemParamConst().cur_status)
		{
			this->SendNoticeNumToOriginRole(plat_type, server_id, uuid, errornum::EN_TRADE_MARKET_FOCUS_ITEM_ON_SALE);
			this->OnOriginUserRemoveFocusReq(plat_type, server_id, uuid, focus_index, page, is_announce);
			return;
		}
		else
		{
			Protocol::SCTradeMarketGetFocusItemInfo resp;
			this->MakeFocusItemInfoData(seller_usi, focus_index, &resp);

			int send_len = 0;
			const char* data = EngineAdapter::Instance().GetSerializeData(&resp, &send_len);
			InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, data, send_len, uid);
		}
	}
}

void TradeMarketManager::OnOriginUserRemoveFocusReq(int plat_type, int server_id, long long uuid, int focus_index, short cur_page, bool is_announce)
{
	UniqueUserID uuser_id = LongLongToUniqueUserID(uuid);
	int uid = UidToInt(uuser_id.user_id);

	UserTradeInfo* info = this->GetUserTradeInfo(uid);
	if (NULL == info)
	{
		this->SendNoticeNumToOriginRole(plat_type, server_id, uuid, errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (!info->OnRemoveFocusItem(focus_index))
	{
		this->SendNoticeNumToOriginRole(plat_type, server_id, uuid, errornum::EN_TRADE_MARKET_UNFOCUS_FAILED);
		return;
	}

	this->OnOriginUserGetRoleFocusListReq(plat_type, server_id, uuid); // 删除成功则刷新当前页的数据
}

void TradeMarketManager::OnOriginUserAddFocusReq(int plat_type, int server_id, long long uuid, int seller_uid, int sale_index, unsigned int upshelve_time)
{
	UniqueUserID uuser_id = LongLongToUniqueUserID(uuid);
	int uid = UidToInt(uuser_id.user_id);

	UserTradeInfo* seller_info = this->GetUserTradeInfo(seller_uid);
	if (NULL == seller_info)
	{
		this->SendNoticeNumToOriginRole(plat_type, server_id, uuid, errornum::EN_TRADE_MARKET_FOCUS_ITEM_NOT_FOUND);
		return;
	}

	const UserSaleItem* seller_usi = seller_info->GetSaleItem(sale_index);
	if (NULL == seller_usi)
	{
		this->SendNoticeNumToOriginRole(plat_type, server_id, uuid, errornum::EN_TRADE_MARKET_FOCUS_ITEM_NOT_FOUND);
		return;
	}

	const ItemBase* item = ITEMPOOL->GetItem(seller_usi->GetSaleItemParamConst().item_wrapper.item_id);
	if (NULL == item)
	{
		this->SendNoticeNumToOriginRole(plat_type, server_id, uuid, errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (item->GetItemType() != I_TYPE_EQUIPMENT && item->GetItemType() != I_TYPE_PET && item->GetItemType() != I_TYPE_JEWELRY)
	{
		this->SendNoticeNumToOriginRole(plat_type, server_id, uuid, errornum::EN_TRADE_MARKET_ITEM_TYPE_CANNOT_FOCUS);
		return;
	}

	if (seller_usi->GetSaleItemParamConst().cur_status != SALE_ITEM_STATUS_ANNOUNCEMENT &&
		seller_usi->GetSaleItemParamConst().cur_status != SALE_ITEM_STATUS_ON_SALE)
	{
		this->SendNoticeNumToOriginRole(plat_type, server_id, uuid, errornum::EN_TRADE_MARKET_FOCUS_ITEM_NOT_FOUND);
		return;
	}

	if (seller_usi->GetSaleItemParamConst().put_on_shelve_time != upshelve_time)
	{
		this->SendNoticeNumToOriginRole(plat_type, server_id, uuid, errornum::EN_TRADE_MARKET_FOCUS_ITEM_NOT_FOUND);
		return;
	}

	UserTradeInfo* role_info = this->GetUserTradeInfo(uid, true);
	if (NULL == role_info)
	{
		return;
	}

	int empty_index = 0;
	if (!role_info->GetEmptyFocusIndex(&empty_index))
	{
		this->SendNoticeNumToOriginRole(plat_type, server_id, uuid, errornum::EN_TRADE_MARKET_FOCUS_ITEM_NOT_FOUND);
		return;
	}

	if (!role_info->OnAddFocusItem(empty_index, seller_uid, seller_usi->GetSellerName(), seller_usi->GetSaleItemParamConst(), false))
	{
		this->SendNoticeNumToOriginRole(plat_type, server_id, uuid, errornum::EN_TRADE_MARKET_ITEM_FOCUS_FAILED);
		return;
	}

	this->OnOriginUserGetRoleFocusListReq(plat_type, server_id, uuid); // 关注成功则刷新当前页的数据
}

void TradeMarketManager::OnOriginUserGetMerchandiseNum(int plat_type, int server_id, long long uuid, short big_search_type, bool is_announce)
{
	UniqueUserID uuser_id = LongLongToUniqueUserID(uuid);
	int uid = UidToInt(uuser_id.user_id);

	Protocol::SCTradeMarketGetMerchandiseNum msg;
	msg.num = 0;
	msg.big_search_type = big_search_type;
	msg.is_announce = is_announce ? 1 : 0;

	static int search_type_list[TRADE_ITEM_SEARCH_TYPE_MAX] = { 0 };
	int search_type_num = TradeMarketConfig::Instance().GetSmallSearchTypeList(big_search_type, search_type_list, TRADE_ITEM_SEARCH_TYPE_MAX);

	for (int i = 0; i < search_type_num; ++i)
	{
		msg.info_list[msg.num].small_search_type = search_type_list[i];
		msg.info_list[msg.num].merchandise_num = m_sale_item_index.GetMerchandiseNum(search_type_list[i], is_announce);
		msg.num += 1;
	}

	int send_len = sizeof(msg) - sizeof(msg.info_list) + msg.num * sizeof(msg.info_list[0]);
	InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, (const char*)&msg, send_len, uid);
}

void TradeMarketManager::OnOriginUserGetSellingListReq(int plat_type, int server_id, long long uuid, ItemID item_id, short medal_effect_type)
{
	UniqueUserID uuser_id = LongLongToUniqueUserID(uuid);
	int uid = UidToInt(uuser_id.user_id);

	const ItemBase* item = ITEMPOOL->GetItem(item_id);
	if (NULL == item) return;

	static SaleItemBase* usi_list[TRANSACTION_RECORD_MAX_NUM];
	int count = 0;

	if (I_TYPE_MEDAL == item->GetItemType())
	{
		m_sale_item_index.OnSearchForMedalSellingList(medal_effect_type, TRANSACTION_RECORD_MAX_NUM, usi_list, &count);
	}
	else
	{
		m_sale_item_index.OnSearchForSellingList(item_id, TRANSACTION_RECORD_MAX_NUM, usi_list, &count);
	}

	Protocol::SCTradeMarketGetSellingList resp;
	resp.count = 0;
	for (int i = 0; i < count && i < TRANSACTION_RECORD_MAX_NUM; ++i)
	{
		const ItemBase* item = ITEMPOOL->GetItem(usi_list[i]->GetSaleItemParamConst().item_wrapper.item_id);

		if (NULL == item) continue;
		if (usi_list[i]->GetSaleItemParamConst().sold_num >= usi_list[i]->GetSaleItemParamConst().item_wrapper.num) continue;

		resp.selling_list[resp.count].item_id = usi_list[i]->GetSaleItemParamConst().item_wrapper.item_id;
		if (usi_list[i]->GetSaleItemParamConst().gold_price > 0)
		{
			resp.selling_list[resp.count].price = usi_list[i]->GetSaleItemParamConst().gold_price;
			resp.selling_list[resp.count].price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD;
		}
		else
		{
			resp.selling_list[resp.count].price = usi_list[i]->GetSaleItemParamConst().immortal_coin_price;
			resp.selling_list[resp.count].price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_IMMORTAL_COIN;
		}

		int param_length = NetValueItemParam::GetDataLengthByItemType(item->GetItemType());
		resp.selling_list[resp.count].data_len = param_length;
		memcpy(resp.selling_list[resp.count].data, usi_list[i]->GetSaleItemParamConst().item_wrapper.param_data, sizeof(NetValueItemParamData));

		resp.selling_list[resp.count].item_num = usi_list[i]->GetSaleItemParamConst().item_wrapper.num - usi_list[i]->GetSaleItemParamConst().sold_num;
		resp.count += 1;
	}

	InternalComm::Instance().SendSerializeMsgToGameUserThroughCross(uid, &resp);

	return;
}

void TradeMarketManager::OnOriginUserSearchPetForQuickBuyReq(int plat_type, int server_id, int role_id, short order_type, ItemID item_id, short quality, int last_seller_uid, int last_price, unsigned int last_put_on_shelve_time, short last_str_level)
{
	static SaleItemBase* item_list[SEARCH_SALE_ITEM_PER_PAGE];
	memset(item_list, 0, sizeof(item_list));
	int count = 0;

	if (item_id > 0)
	{
		static SalePetIndex index;
		index.item_id = item_id;
		index.price = last_price;
		index.put_on_shelve_time = last_put_on_shelve_time;
		index.uid = last_seller_uid;
		index.pet_str_level = last_str_level;

		m_sale_item_index.OnSearchPetForQuickBuy(order_type, index, SEARCH_PET_QUICK_BUY_ITEM_NUM, item_list, &count);
	}
	else
	{
		static SalePetQualityIndex index;
		index.quality = quality;
		index.price = last_price;
		index.put_on_shelve_time = last_put_on_shelve_time;
		index.uid = last_seller_uid;
		index.pet_str_level = last_str_level;

		m_sale_item_index.OnSearchPetQualityForQuickBuy(order_type, index, SEARCH_PET_QUICK_BUY_ITEM_NUM, item_list, &count);
	}

	Protocol::SCTradeMarketSearchPetForQuickBuy resp;
	this->AssembleSearchPetForQuickBuyResult(&resp, order_type, count, item_list);
	
	InternalComm::Instance().SendSerializeMsgToGameUserThroughCross(role_id, &resp);
}

void TradeMarketManager::OnOriginUserSearchAdvertiseItemReq(crossgameprotocal::GameHiddenTradeMarketSearchAdvertiseItemReq* msg)
{
	bool is_announcing = false;

	if (msg->seller_uid == SYSTEM_TRADE_UID)
	{
		const SystemTradeItemInfo* ssi = m_sys_trade_info.GetTradeItemInfo(msg->unique_key);
		if (NULL == ssi || ssi->sale_item_ptr->GetSaleItemParamConst().item_wrapper.item_id != msg->item_id ||
			ssi->sale_item_ptr->GetSaleItemParamConst().put_on_shelve_time != msg->put_on_shelve_time || ssi->sale_item_ptr->IsOffShelve())
		{
			this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND);
			return;
		}
		is_announcing = ssi->sale_item_ptr->IsAnnouncing();
	}
	else
	{
		UserTradeInfo* seller_info = this->GetUserTradeInfo(msg->seller_uid);
		if (NULL == seller_info)
		{
			this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND);
			return;
		}

		const UserSaleItem* seller_usi = seller_info->GetSaleItem(msg->sale_index);
		if (NULL == seller_usi || seller_usi->GetSaleItemParamConst().item_wrapper.item_id != msg->item_id ||
			seller_usi->GetSaleItemParamConst().put_on_shelve_time != msg->put_on_shelve_time || seller_usi->IsOffShelve())
		{
			this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND);
			return;
		}

		is_announcing = seller_usi->IsAnnouncing();
	}

	static SaleItemBase* item_list[SEARCH_SALE_ITEM_PER_PAGE];
	memset(item_list, 0, sizeof(item_list));
	int count = 0;
	int page = 0;
	int total_page = 0;
	short cursor_index = 0;
	if (!m_sale_item_index.OnSearchByAdvertise(is_announcing, msg->item_id, msg->price, msg->put_on_shelve_time, msg->seller_uid, msg->unique_key, SEARCH_SALE_ITEM_PER_PAGE, item_list, &count, &page, &total_page, &cursor_index))
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND);
	}

	Protocol::SCTradeMarketSearchResult result;
	result.item_count = 0;

	this->MakeSearchResult(&result, count, item_list, page, total_page, total_page, is_announcing, cursor_index);
	InternalComm::Instance().SendSerializeMsgToGameUserThroughCross(msg->role_id, &result);
}

void TradeMarketManager::OnOriginUserRequestBuy(crossgameprotocal::GameHiddenTradeMarketRequestBuy* msg)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (IsSystemSeller(msg->seller_uid))
	{
		this->OnOriginUserRequestSystemBuy(msg);
		return;
	}

	UserTradeInfo* buyer_trade_info = this->GetUserTradeInfo(msg->role_id, true);
	if (NULL == buyer_trade_info)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_UNKNOWN_ERROR);
		this->SendRefreshNotice(NULL, msg->role_id);
		return;
	}

	UserTradeInfo* seller_trade_info = this->GetUserTradeInfo(msg->seller_uid);
	if (NULL == seller_trade_info)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_SELLER_NOT_FOUND);
		this->SendRefreshNotice(NULL, msg->role_id);
		return;
	}

	const UserSaleItem* usi = seller_trade_info->GetSaleItem(msg->sale_index);
	if (NULL == usi)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND);
		this->SendRefreshNotice(NULL, msg->role_id);
		return;
	}

	if (msg->upshelve_time != usi->GetSaleItemParamConst().put_on_shelve_time) // 物品与实际搜索的物品不对应
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND);
		this->SendRefreshNotice(NULL, msg->role_id);
		return;
	}

	if (SALE_ITEM_STATUS_ANNOUNCEMENT == usi->GetSaleItemParamConst().cur_status)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_ITEM_ANNOUNCING);
		this->SendRefreshNotice(NULL, msg->role_id);
		return;
	}

	if (SALE_ITEM_STATUS_ON_SALE != usi->GetSaleItemParamConst().cur_status) // 既不在公示期，也不在发售期，则可能是玩家作弊或系统错误
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_ITEM_SOLD_OUT);
		this->SendRefreshNotice(NULL, msg->role_id);
		return;
	}

	if (usi->GetSaleItemParamConst().item_wrapper.num < msg->buy_num)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_ITEM_NOT_ENOUGH);
		this->SendRefreshNotice(NULL, msg->role_id);
		return;
	}

	ItemID item_id = usi->GetSaleItemParamConst().item_wrapper.item_id;
	const ItemBase* item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int num_per_group = usi->GetSaleItemParamConst().num_per_group;
	if (0 == num_per_group)
	{
		if (msg->buy_num % item_base->GetTradeNumPerGroup() != 0)
		{
#ifdef _DEBUG
			assert(0);
#endif

			this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_UNKNOWN_ERROR);
			return;
		}
		num_per_group = item_base->GetTradeNumPerGroup();
	}

	int buy_group_num = msg->buy_num / num_per_group;
	int single_price = usi->GetSaleItemParamConst().gold_price;
	int total_price = single_price * buy_group_num;
	int price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD;
	if (usi->GetSaleItemParamConst().gold_price <= 0)
	{
		single_price = usi->GetSaleItemParamConst().immortal_coin_price;
		total_price = single_price * buy_group_num;
		price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_IMMORTAL_COIN;
	}

	crossgameprotocal::HiddenGameTradeMarketRequestBuyResp resp;
	resp.buy_item_id = msg->buy_item_id;
	resp.buy_num = msg->buy_num;
	resp.expect_price = msg->expect_price;
	resp.reserve_sh = msg->reserve_sh;
	resp.role_id = msg->role_id;
	resp.sale_index = msg->sale_index;
	resp.seller_uid = msg->seller_uid;
	resp.total_price = total_price;
	F_STRNCPY(resp.unique_key_str, msg->unique_key_str, sizeof(resp.unique_key_str));
	resp.upshelve_time = msg->upshelve_time;
	resp.order_id = msg->order_id;

	InternalComm::Instance().SendToGameThroughCross(msg->plat_type, msg->server_id, (const char*)&resp, sizeof(resp));
}

void TradeMarketManager::OnOriginUserRequestSystemBuy(crossgameprotocal::GameHiddenTradeMarketRequestBuy* msg)
{
	UserTradeInfo* buyer_trade_info = this->GetUserTradeInfo(msg->role_id, true);
	if (NULL == buyer_trade_info)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_UNKNOWN_ERROR);
		this->SendRefreshNotice(NULL, msg->role_id);
		return;
	}

	long long unique_key = 0LL;
	if (!ConvertUniqueKeyStrToNumber(msg->unique_key_str, &unique_key))
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	SystemTradeItemInfo* stii = m_sys_trade_info.GetTradeItemInfo(unique_key);
	if (NULL == stii)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_SELLER_NOT_FOUND);
		this->SendRefreshNotice(NULL, msg->role_id);
		return;
	}

	const SystemSaleItem* ssi = stii->sale_item_ptr;
	if (NULL == ssi)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND);
		this->SendRefreshNotice(NULL, msg->role_id);
		return;
	}

	if (msg->upshelve_time != ssi->GetSaleItemParamConst().put_on_shelve_time) // 物品与实际搜索的物品不对应
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND);
		this->SendRefreshNotice(NULL, msg->role_id);
		return;
	}

	if (SALE_ITEM_STATUS_ANNOUNCEMENT == ssi->GetSaleItemParamConst().cur_status)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_ITEM_ANNOUNCING);
		this->SendRefreshNotice(NULL, msg->role_id);
		return;
	}

	if (SALE_ITEM_STATUS_ON_SALE != ssi->GetSaleItemParamConst().cur_status) // 既不在公示期，也不在发售期，则可能是玩家作弊或系统错误
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_ITEM_SOLD_OUT);
		this->SendRefreshNotice(NULL, msg->role_id);
		return;
	}

	if (ssi->GetSaleItemParamConst().item_wrapper.num < msg->buy_num)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_ITEM_NOT_ENOUGH);
		this->SendRefreshNotice(NULL, msg->role_id);
		return;
	}

	ItemID item_id = ssi->GetSaleItemParamConst().item_wrapper.item_id;
	const ItemBase* item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base)
	{
		return;
	}

	int num_per_group = ssi->GetSaleItemParamConst().num_per_group;
	if (0 == num_per_group)
	{
		if (msg->buy_num % item_base->GetTradeNumPerGroup() != 0)
		{
#ifdef _DEBUG
			assert(0);
#endif
			this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_UNKNOWN_ERROR);
			return;
		}
		num_per_group = item_base->GetTradeNumPerGroup();
	}

	int buy_group_num = msg->buy_num / num_per_group;
	int single_price = ssi->GetSaleItemParamConst().gold_price;
	int total_price = single_price * buy_group_num;
	int price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD;
	if (ssi->GetSaleItemParamConst().gold_price <= 0)
	{
		single_price = ssi->GetSaleItemParamConst().immortal_coin_price;
		total_price = single_price * buy_group_num;
		price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_IMMORTAL_COIN;
	}

	crossgameprotocal::HiddenGameTradeMarketRequestBuyResp resp;
	resp.buy_item_id = msg->buy_item_id;
	resp.buy_num = msg->buy_num;
	resp.expect_price = msg->expect_price;
	resp.reserve_sh = msg->reserve_sh;
	resp.role_id = msg->role_id;
	resp.sale_index = msg->sale_index;
	resp.seller_uid = msg->seller_uid;
	resp.total_price = total_price;
	F_STRNCPY(resp.unique_key_str, msg->unique_key_str, sizeof(resp.unique_key_str));
	resp.upshelve_time = msg->upshelve_time;
	resp.order_id = msg->order_id;

	InternalComm::Instance().SendToGameThroughCross(msg->plat_type, msg->server_id, (const char*)&resp, sizeof(resp));
}

void TradeMarketManager::OnOriginUserRequestBuyResp(crossgameprotocal::HiddenGameTradeMarketRequestBuyResp* msg)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(msg->role_id));
	if (NULL == role)
	{
		return;
	}

	if (!role->GetRoleModuleManager()->GetMoney()->GoldMoreThan(msg->total_price))
	{
		role->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}

	if (Role::AUTHORITY_TYPE_TEST == role->GetAuthorityType())
	{		// 内玩购买限制
		if (msg->total_price + role->GetRoleModuleManager()->GetCommonData().autority_type_test_day_buy_limit > GLOBALCONFIG->GetBuyLimit())
		{
			role->NoticeNum(errornum::EN_AUTHORITY_TYPE_TEST_DAY_BUY_LIMIT);
			return;
		}
		else
		{
			role->GetRoleModuleManager()->GetCommonData().autority_type_test_day_buy_limit += msg->total_price;
			gamelog::g_log_trade_market.printf(LL_INFO, "%s role[%d, %s] today_trading_up[%d]", __FUNCTION__, role->GetUID(), role->GetName(), role->GetRoleModuleManager()->GetCommonData().autority_type_test_day_buy_limit);

		}
	}

	if (!role->GetRoleModuleManager()->GetMoney()->UseGold(msg->total_price, __FUNCTION__, false))
	{
		role->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}

	OrderManager::Instance().OnOrderCostGold(msg->order_id, msg->total_price);
	OrderManager::Instance().OnOrderSwitchToStatus(msg->order_id, TRADEMARKET_ORDER_STATUS_CONSUME);

	crossgameprotocal::GameHiddenTradeMarketRequestBuyConfirm req;
	req.buy_item_id = msg->buy_item_id;
	req.buy_num = msg->buy_num;
	req.expect_price = msg->expect_price;
	req.reserve_sh = 0;
	req.role_id = msg->role_id;
	F_STRNCPY(req.role_name, role->GetName(), sizeof(req.role_name));
	F_STRNCPY(req.role_pname, role->GetPlatName(), sizeof(req.role_pname));
	req.sale_index = msg->sale_index;
	req.seller_uid = msg->seller_uid;
	F_STRNCPY(req.unique_key_str, msg->unique_key_str, sizeof(req.unique_key_str));
	req.upshelve_time = msg->upshelve_time;
	req.order_id = msg->order_id;
	req.consumed_gold = msg->total_price;

	req.plat_type = LocalConfig::Instance().GetPlatType();
	req.server_id = LocalConfig::Instance().GetMergeServerId();

	InternalComm::Instance().SendToHiddenThroughCross((const char*)&req, sizeof(req));
}

void TradeMarketManager::OnOriginUserRequestBuyConfirm(crossgameprotocal::GameHiddenTradeMarketRequestBuyConfirm* msg)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (IsSystemSeller(msg->seller_uid))
	{
		this->OnOriginUserRequestSystemBuyConfirm(msg);
		return;
	}

	UserTradeInfo* buyer_trade_info = this->GetUserTradeInfo(msg->role_id, true);
	if (NULL == buyer_trade_info)
	{
		this->SendOriginUserConfirmBuyErrorResp(msg->plat_type, msg->server_id, msg->role_id, msg->seller_uid, msg->sale_index, msg->buy_num, msg->buy_item_id, msg->unique_key_str, msg->order_id, errornum::EN_UNKNOWN_ERROR, "BuyerTradeInfo Not Found");
		return;
	}

	UserTradeInfo* seller_trade_info = this->GetUserTradeInfo(msg->seller_uid);
	if (NULL == seller_trade_info)
	{
		this->SendOriginUserConfirmBuyErrorResp(msg->plat_type, msg->server_id, msg->role_id, msg->seller_uid, msg->sale_index, msg->buy_num, msg->buy_item_id, msg->unique_key_str, msg->order_id, errornum::EN_TRADE_MARKET_SELLER_NOT_FOUND, "SellerTradeInfo Not Found");
		return;
	}

	const UserSaleItem* usi = seller_trade_info->GetSaleItem(msg->sale_index);
	if (NULL == usi)
	{
		this->SendOriginUserConfirmBuyErrorResp(msg->plat_type, msg->server_id, msg->role_id, msg->seller_uid, msg->sale_index, msg->buy_num, msg->buy_item_id, msg->unique_key_str, msg->order_id, errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND, "SaleItemInfo Not Found");
		return;
	}

	if (msg->upshelve_time != usi->GetSaleItemParamConst().put_on_shelve_time) // 物品与实际搜索的物品不对应
	{
		this->SendOriginUserConfirmBuyErrorResp(msg->plat_type, msg->server_id, msg->role_id, msg->seller_uid, msg->sale_index, msg->buy_num, msg->buy_item_id, msg->unique_key_str, msg->order_id, errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND, "PutOnShelveTime Not Match");
		return;
	}

	if (SALE_ITEM_STATUS_ANNOUNCEMENT == usi->GetSaleItemParamConst().cur_status)
	{
		this->SendOriginUserConfirmBuyErrorResp(msg->plat_type, msg->server_id, msg->role_id, msg->seller_uid, msg->sale_index, msg->buy_num, msg->buy_item_id, msg->unique_key_str, msg->order_id, errornum::EN_TRADE_MARKET_ITEM_ANNOUNCING, "Item Is Announcing");
		return;
	}

	if (SALE_ITEM_STATUS_ON_SALE != usi->GetSaleItemParamConst().cur_status) // 既不在公示期，也不在发售期，则可能是玩家作弊或系统错误
	{
		this->SendOriginUserConfirmBuyErrorResp(msg->plat_type, msg->server_id, msg->role_id, msg->seller_uid, msg->sale_index, msg->buy_num, msg->buy_item_id, msg->unique_key_str, msg->order_id, errornum::EN_TRADE_MARKET_ITEM_SOLD_OUT, "Item Not On Sale");
		return;
	}

	if (usi->GetSaleItemParamConst().item_wrapper.num < msg->buy_num)
	{
		this->SendOriginUserConfirmBuyErrorResp(msg->plat_type, msg->server_id, msg->role_id, msg->seller_uid, msg->sale_index, msg->buy_num, msg->buy_item_id, msg->unique_key_str, msg->order_id, errornum::EN_TRADE_MARKET_ITEM_NOT_ENOUGH, "Item Num Not Enough");
		return;
	}

	ItemID item_id = usi->GetSaleItemParamConst().item_wrapper.item_id;
	const ItemBase* item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base)
	{
		return;
	}

	int num_per_group = usi->GetSaleItemParamConst().num_per_group;
	if (0 == num_per_group)
	{
		if (msg->buy_num % item_base->GetTradeNumPerGroup() != 0)
		{
#ifdef _DEBUG
			assert(0);
#endif
			this->SendOriginUserConfirmBuyErrorResp(msg->plat_type, msg->server_id, msg->role_id, msg->seller_uid, msg->sale_index, msg->buy_num, msg->buy_item_id, msg->unique_key_str, msg->order_id, errornum::EN_UNKNOWN_ERROR, "Item Num Per Group Not Correct");
			return;
		}
		num_per_group = item_base->GetTradeNumPerGroup();
	}

	int buy_group_num = msg->buy_num / num_per_group;
	int single_price = usi->GetSaleItemParamConst().gold_price;
	int total_price = single_price * buy_group_num;
	int price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD;
	if (usi->GetSaleItemParamConst().gold_price <= 0)
	{
		single_price = usi->GetSaleItemParamConst().immortal_coin_price;
		total_price = single_price * buy_group_num;
		price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_IMMORTAL_COIN;
	}

	static ItemDataWrapper wrapper;
	wrapper = usi->GetSaleItemParamConst().item_wrapper;
	wrapper.num = msg->buy_num;

	if (seller_trade_info->OnItemSold(msg->sale_index, msg->buy_num, msg->role_id, msg->role_name, msg->role_pname))
	{
		crossgameprotocal::HiddenGameTradeMarketRequestBuyConfirmResp resp;
		resp.buy_item_id = msg->buy_item_id;
		resp.buy_num = msg->buy_num;
		resp.expect_price = msg->expect_price;
		resp.reserve_sh = msg->reserve_sh;
		resp.role_id = msg->role_id;
		F_STRNCPY(resp.role_name, msg->role_name, sizeof(resp.role_name));
		F_STRNCPY(resp.role_pname, msg->role_pname, sizeof(resp.role_pname));
		resp.sale_index = msg->sale_index;
		resp.seller_uid = msg->seller_uid;
		F_STRNCPY(resp.unique_key_str, msg->unique_key_str, sizeof(resp.unique_key_str));
		resp.wrapper = wrapper;
		resp.order_id = msg->order_id;
		resp.consumed_gold = msg->consumed_gold;
		InternalComm::Instance().SendToGameThroughCross(msg->plat_type, msg->server_id, (const char*)&resp, sizeof(resp));

		int param_length = NetValueItemParam::GetDataLengthByItemType(item_base->GetItemType());
		m_transaction_record.AddTransactionRecord(item_id, msg->buy_num, price_type, single_price, param_length, wrapper.param_data);
		seller_trade_info->AddSoldRecord(item_id, msg->buy_num, price_type, single_price, wrapper);
		buyer_trade_info->AddBuyRecord(item_id, msg->buy_num, price_type, single_price, wrapper);

		if (single_price >= TRADEMARKET_CFG.GetConstantCfg().need_gold_notice)
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_sold_succ_send_global_message,
				item_base->GetItemName(), msg->role_id, usi->GetSaleItemParamConst().sale_index,
				usi->GetSaleItemParamConst().put_on_shelve_time, single_price, item_base->GetItemId(), usi->GetSaleItemParamConst().put_on_shelve_time);
			if (length > 0)
			{
				Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
				{
					InternalComm::Instance().SendToGameAllUserThroughCross(msg->plat_type, msg->server_id, (const char*)&sm, sendlen);
					World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
				}
			}
		}

		{
			// 通知卖家货架更新
			this->SendShelveChangeNotice(seller_trade_info->GetRoleID(), msg->sale_index, Protocol::SCTradeMarketUserShelveData::REASON_NOTIFY_CHANGE);
		}

		gamelog::g_log_trade_market.printf(LL_INFO, "TradeMarketManager::OnOriginUserRequestBuyConfirm SUCC, buyer[%d,%s] seller[%d,%s] sale_index[%d], item[%d,%d] buy_num[%d] buy_group_num[%d] unique_key[%lld]",
			msg->role_id, msg->role_name, usi->GetSellerUid(), usi->GetSellerName(), msg->sale_index, usi->GetSaleItemParamConst().item_wrapper.item_id,
			usi->GetSaleItemParamConst().item_wrapper.num, msg->buy_num, buy_group_num, usi->GetSaleItemParamConst().unique_key);

		LOG_QUICK12(gamelog::g_log_quick,
			LOG_TYPE_TRADE_MARTKET_BUY, usi->GetSellerUid(), usi->GetSellerName(), NULL,
			usi->GetSaleItemParamConst().item_wrapper.item_id, usi->GetSaleItemParamConst().item_wrapper.num, msg->role_pname, msg->role_name,
			msg->role_id, total_price, 0, 0, 0);
	}
	else
	{
		gamelog::g_log_trade_market.printf(LL_ERROR, "TradeMarketManager::OnOriginUserRequestBuyConfirm Failed, buyer[%d,%s] seller[%d,%s] sale_index[%d], item[%d,%d] buy_num[%d] unique_key[%lld]",
			msg->role_id, msg->role_name, usi->GetSellerUid(), usi->GetSellerName(), msg->sale_index, usi->GetSaleItemParamConst().item_wrapper.item_id,
			usi->GetSaleItemParamConst().item_wrapper.num, msg->buy_num, usi->GetSaleItemParamConst().unique_key);
	}
}

void TradeMarketManager::OnOriginUserRequestSystemBuyConfirm(crossgameprotocal::GameHiddenTradeMarketRequestBuyConfirm* msg)
{
	UserTradeInfo* buyer_trade_info = this->GetUserTradeInfo(msg->role_id, true);
	if (NULL == buyer_trade_info)
	{
		this->SendOriginUserConfirmBuyErrorResp(msg->plat_type, msg->server_id, msg->role_id, msg->seller_uid, msg->sale_index, msg->buy_num, msg->buy_item_id, msg->unique_key_str, msg->order_id, errornum::EN_UNKNOWN_ERROR, "BuyerTradeInfo Not Found");
		return;
	}

	long long unique_key = 0LL;
	if (!ConvertUniqueKeyStrToNumber(msg->unique_key_str, &unique_key))
	{
		this->SendOriginUserConfirmBuyErrorResp(msg->plat_type, msg->server_id, msg->role_id, msg->seller_uid, msg->sale_index, msg->buy_num, msg->buy_item_id, msg->unique_key_str, msg->order_id, errornum::EN_INPUT_PARAM_ERROR, "Input Param Error");
		return;
	}

	SystemTradeItemInfo* stii = m_sys_trade_info.GetTradeItemInfo(unique_key);
	if (NULL == stii)
	{
		this->SendOriginUserConfirmBuyErrorResp(msg->plat_type, msg->server_id, msg->role_id, msg->seller_uid, msg->sale_index, msg->buy_num, msg->buy_item_id, msg->unique_key_str, msg->order_id, errornum::EN_TRADE_MARKET_SELLER_NOT_FOUND, "SystemSeller Not Found");
		return;
	}

	const SystemSaleItem* ssi = stii->sale_item_ptr;
	if (NULL == ssi)
	{
		this->SendOriginUserConfirmBuyErrorResp(msg->plat_type, msg->server_id, msg->role_id, msg->seller_uid, msg->sale_index, msg->buy_num, msg->buy_item_id, msg->unique_key_str, msg->order_id, errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND, "Item Not Found");
		return;
	}

	if (msg->upshelve_time != ssi->GetSaleItemParamConst().put_on_shelve_time) // 物品与实际搜索的物品不对应
	{
		this->SendOriginUserConfirmBuyErrorResp(msg->plat_type, msg->server_id, msg->role_id, msg->seller_uid, msg->sale_index, msg->buy_num, msg->buy_item_id, msg->unique_key_str, msg->order_id, errornum::EN_TRADE_MARKET_ITEM_NOT_FOUND, "PutOnShelveTime Not Match");
		return;
	}

	if (SALE_ITEM_STATUS_ANNOUNCEMENT == ssi->GetSaleItemParamConst().cur_status)
	{
		this->SendOriginUserConfirmBuyErrorResp(msg->plat_type, msg->server_id, msg->role_id, msg->seller_uid, msg->sale_index, msg->buy_num, msg->buy_item_id, msg->unique_key_str, msg->order_id, errornum::EN_TRADE_MARKET_ITEM_ANNOUNCING, "Item Is Announcing");
		return;
	}

	if (SALE_ITEM_STATUS_ON_SALE != ssi->GetSaleItemParamConst().cur_status) // 既不在公示期，也不在发售期，则可能是玩家作弊或系统错误
	{
		this->SendOriginUserConfirmBuyErrorResp(msg->plat_type, msg->server_id, msg->role_id, msg->seller_uid, msg->sale_index, msg->buy_num, msg->buy_item_id, msg->unique_key_str, msg->order_id, errornum::EN_TRADE_MARKET_ITEM_SOLD_OUT, "Item Not On Sale");
		return;
	}

	if (ssi->GetSaleItemParamConst().item_wrapper.num < msg->buy_num)
	{
		this->SendOriginUserConfirmBuyErrorResp(msg->plat_type, msg->server_id, msg->role_id, msg->seller_uid, msg->sale_index, msg->buy_num, msg->buy_item_id, msg->unique_key_str, msg->order_id, errornum::EN_TRADE_MARKET_ITEM_NOT_ENOUGH, "Item Num Not Enough");
		return;
	}

	ItemID item_id = ssi->GetSaleItemParamConst().item_wrapper.item_id;
	const ItemBase* item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base)
	{
		return;
	}

	int num_per_group = ssi->GetSaleItemParamConst().num_per_group;
	if (0 == num_per_group)
	{
		if (msg->buy_num % item_base->GetTradeNumPerGroup() != 0)
		{
#ifdef _DEBUG
			assert(0);
#endif
			this->SendOriginUserConfirmBuyErrorResp(msg->plat_type, msg->server_id, msg->role_id, msg->seller_uid, msg->sale_index, msg->buy_num, msg->buy_item_id, msg->unique_key_str, msg->order_id, errornum::EN_UNKNOWN_ERROR, "Item Num Per Group Not Correct");
			return;
		}
		num_per_group = item_base->GetTradeNumPerGroup();
	}

	int buy_group_num = msg->buy_num / num_per_group;
	int single_price = ssi->GetSaleItemParamConst().gold_price;
	int total_price = single_price * buy_group_num;
	int price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD;
	if (ssi->GetSaleItemParamConst().gold_price <= 0)
	{
		single_price = ssi->GetSaleItemParamConst().immortal_coin_price;
		total_price = single_price * buy_group_num;
		price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_IMMORTAL_COIN;
	}

	static ItemDataWrapper wrapper;
	wrapper = ssi->GetSaleItemParamConst().item_wrapper;
	wrapper.num = msg->buy_num;

	if (m_sys_trade_info.OnItemSold(unique_key, msg->buy_num, msg->role_id, msg->role_name, msg->role_pname))
	{
		crossgameprotocal::HiddenGameTradeMarketRequestBuyConfirmResp resp;
		resp.buy_item_id = msg->buy_item_id;
		resp.buy_num = msg->buy_num;
		resp.expect_price = msg->expect_price;
		resp.reserve_sh = msg->reserve_sh;
		resp.role_id = msg->role_id;
		F_STRNCPY(resp.role_name, msg->role_name, sizeof(resp.role_name));
		F_STRNCPY(resp.role_pname, msg->role_pname, sizeof(resp.role_pname));
		resp.sale_index = msg->sale_index;
		resp.seller_uid = msg->seller_uid;
		F_STRNCPY(resp.unique_key_str, msg->unique_key_str, sizeof(resp.unique_key_str));
		resp.wrapper = wrapper;
		resp.order_id = msg->order_id;
		resp.consumed_gold = msg->consumed_gold;
		InternalComm::Instance().SendToGameThroughCross(msg->plat_type, msg->server_id, (const char*)&resp, sizeof(resp));

		int param_length = NetValueItemParam::GetDataLengthByItemType(item_base->GetItemType());
		m_transaction_record.AddTransactionRecord(item_id, msg->buy_num, price_type, single_price, param_length, wrapper.param_data);
		buyer_trade_info->AddBuyRecord(item_id, msg->buy_num, price_type, single_price, wrapper);

		if (single_price >= TRADEMARKET_CFG.GetConstantCfg().need_gold_notice)
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_sold_succ_send_global_message,
				item_base->GetItemName(), msg->role_id, ssi->GetSaleItemParamConst().sale_index,
				ssi->GetSaleItemParamConst().put_on_shelve_time, single_price, item_base->GetItemId(), ssi->GetSaleItemParamConst().put_on_shelve_time);
			if (length > 0)
			{
				Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
				{
					InternalComm::Instance().SendToGameAllUserThroughCross(msg->plat_type, msg->server_id, (const char*)&sm, sendlen);
					World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
				}
			}
		}

		gamelog::g_log_trade_market.printf(LL_INFO, "TradeMarketManager::OnOriginUserRequestSystemBuyConfirm SUCC, buyer[%d,%s] seller[%d,%s] sale_index[%d], item[%d,%d] buy_num[%d] buy_group_num[%d] unique_key[%lld]",
			msg->role_id, msg->role_name, ssi->GetSellerUid(), ssi->GetSellerName(), msg->sale_index, ssi->GetSaleItemParamConst().item_wrapper.item_id,
			ssi->GetSaleItemParamConst().item_wrapper.num, msg->buy_num, buy_group_num, ssi->GetSaleItemParamConst().unique_key);
	}
	else
	{
		gamelog::g_log_trade_market.printf(LL_ERROR, "TradeMarketManager::OnOriginUserRequestSystemBuyConfirm Failed, buyer[%d,%s] seller[%d,%s] sale_index[%d], item[%d,%d] buy_num[%d] unique_key[%lld]",
			msg->role_id, msg->role_name, ssi->GetSellerUid(), ssi->GetSellerName(), msg->sale_index, ssi->GetSaleItemParamConst().item_wrapper.item_id,
			ssi->GetSaleItemParamConst().item_wrapper.num, msg->buy_num, ssi->GetSaleItemParamConst().unique_key);
	}
}

void TradeMarketManager::OnOriginUserRequestBuyConfirmResp(crossgameprotocal::HiddenGameTradeMarketRequestBuyConfirmResp* msg)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	OrderManager::Instance().OnOrderFinish(msg->order_id);

	this->GiveResourceToUser(IntToUid(msg->role_id), &msg->wrapper, 0, 0, 0, __FUNCTION__, true);

	EventHandler::Instance().OnTradeMarketSaleItem(msg->seller_uid, msg->wrapper.item_id, msg->seller_uid == msg->role_id);
	Role* buyer = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(msg->role_id));
	if (NULL != buyer)
	{
		this->SendRefreshNotice(buyer, buyer->GetUID());

		EventHandler::Instance().OnTradeMarketPurchase(buyer, msg->wrapper.item_id, msg->consumed_gold);
	}
	else
	{
		OfflineRegisterManager::Instance().OnTradeMarketBuySucc(msg->role_id, msg->wrapper.item_id, msg->consumed_gold);
	}
}

void TradeMarketManager::OnOriginUserRequestBuyConfirmErrorResp(crossgameprotocal::HiddenGameTradeMarketRequestBuyConfirmErrorResp* msg)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	OrderManager::Instance().OnOrderFailed(msg->order_id, true, msg->error_msg);

	gamelog::g_log_trade_market.printf(LL_WARNING, "OnOriginUserRequestBuyConfirmErrorResp role[%d] seller[%d] sale_index[%d] item_id[%d] buy_num[%d]\
		unique_key[%s] error_msg[%s] error_num[%d] order_id[%lld]",
		msg->role_id, msg->seller_uid, msg->sale_index, msg->buy_item_id, msg->buy_num, 
		msg->unique_key_str, msg->error_msg, msg->error_num, msg->order_id);

	Role* buyer = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(msg->role_id));
	if (NULL != buyer)
	{
		this->SendRefreshNotice(buyer, buyer->GetUID());
		buyer->NoticeNum(msg->error_num);
	}
}

void TradeMarketManager::OnOriginUserRequestUpshelve(crossgameprotocal::GameHiddenTradeMarketRequestUpshelve* msg)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (msg->item_wrapper.Invalid())
	{
#ifdef _DEBUG
		assert(0);
#endif
		gamelog::g_log_trade_market.printf(LL_ERROR, "TradeMarketManager::OnOriginUserRequestUpshelve role[%d,%s] add invalid item",
			msg->role_id, msg->role_name);
		
		return;
	}

	const ItemBase* item = ITEMPOOL->GetItem(msg->item_wrapper.item_id);
	if (NULL == item)
	{
		return;
	}

	UserTradeInfo* user_trade_info = this->GetUserTradeInfo(msg->role_id, true);
	if (NULL == user_trade_info)
	{
		return;
	}

	if (msg->gold_price < TradeMarketManager::GetTradeMinPrice(item, &msg->item_wrapper))
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_PRICE_TOO_LOW_TO_SALE);
		return;
	}

	if (msg->gold_price > TradeMarketManager::GetTradeMaxPrice(item, &msg->item_wrapper))
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_PRICE_TOO_HIGH_TO_SALE);
		return;
	}

	int empty_sale_index = 0;
	if (!user_trade_info->GetEmptyIndex(&empty_sale_index))
	{
		this->SendOriginUserRequestUpshelveResp(msg->plat_type, msg->server_id, msg->role_id,
			crossgameprotocal::HiddenGameTradeMarketRequestUpshelveResp::RESULT_NO_EMPTY_SPACE,
			msg->item_wrapper, msg->service_fee, msg->credit_fee, msg->order_id);

		gamelog::g_log_trade_market.printf(LL_INFO, "OnOriginUserRequestUpshelve Failed[no empty idx], role[%d,%s] item[%d,%d]",
			msg->role_id, msg->role_name, msg->item_wrapper.item_id, msg->item_wrapper.num);

		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_SHELVE_FULL);
		return;
	}

	SaleItemParam param;
	param.Reset();
	param.cur_status = SALE_ITEM_STATUS_ON_SHELVE;
	param.is_need_announcement = TradeMarketConfig::Instance().IsNeedAnnounce(item);
	param.item_search_type = item->GetTradeSearchType();
	param.num_per_group = item->GetTradeNumPerGroup();
	param.item_wrapper = msg->item_wrapper;
	param.gold_price = msg->gold_price;
	param.sale_index = empty_sale_index;

	//获取下一个整点的时间戳
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	param.put_on_shelve_time = now;

	//获取unique_key
	param.unique_key = TradeMarketManager::Instance().GetUniqueItemKey();

#ifdef _WIN32
	param.next_status_time = now + 5; // 方便调试
#else
	param.next_status_time = now + EngineAdapter::Instance().NextHourInterval(0, 0);
#endif	

	int errnum = 0;
	if (!this->CanSellItem(msg->role_id, param, &errnum))
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errnum);
		return;
	}

	bool ret = user_trade_info->OnAddSaleItem(empty_sale_index, msg->role_id, msg->role_name, param, false);
	if (!ret)
	{
		this->SendOriginUserRequestUpshelveResp(msg->plat_type, msg->server_id, msg->role_id,
			crossgameprotocal::HiddenGameTradeMarketRequestUpshelveResp::RESULT_ADD_SALE_ITEM_FAIL, 
			msg->item_wrapper, msg->service_fee, msg->credit_fee, msg->order_id);

		gamelog::g_log_trade_market.printf(LL_INFO, "OnOriginUserRequestUpshelve Failed[add sale item fail], role[%d,%s] item[%d,%d]",
					msg->role_id, msg->role_name, param.item_wrapper.item_id, param.item_wrapper.num);

		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_ADD_ON_SHELVE_FAILED);
		return;
	}
	else
	{
#ifdef TRACK_ITEM_FLAG
		gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG TradeMarketManager::OnOriginUserRequestUpshelve role[%d,%s] upshelve item_id[%d] item_unique_id[%lld] unique_key[%lld]",
			msg->role_id, msg->role_name, param.item_wrapper.item_id, param.item_wrapper.item_unique_id, param.unique_key);
#endif
	}

	this->SendShelveChangeNotice(msg->role_id, empty_sale_index, Protocol::SCTradeMarketUserShelveData::REASON_NOTIFY_ADD);

	LOG_QUICK12(gamelog::g_log_quick,
		LOG_TYPE_TRADE_MARTKET_ADD, msg->role_id, msg->role_name, msg->role_pname,
		param.item_wrapper.item_id, param.item_wrapper.num, NULL, NULL,
		0, param.gold_price, 0, 0, 0);

	this->SendOriginUserRequestUpshelveResp(msg->plat_type, msg->server_id, msg->role_id,
		crossgameprotocal::HiddenGameTradeMarketRequestUpshelveResp::RESULT_SUCC,
		msg->item_wrapper, msg->service_fee, msg->credit_fee, msg->order_id);
}

void TradeMarketManager::OnOriginUserRequestUpshelveResp(crossgameprotocal::HiddenGameTradeMarketRequestUpshelveResp* msg)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (crossgameprotocal::HiddenGameTradeMarketRequestUpshelveResp::RESULT_SUCC == msg->result)
	{
		gamelog::g_log_trade_market.printf(LL_INFO, "OnOriginUserRequestUpshelveResp SUCC, role[%d] item[%d] num[%d] unique_item_id[%lld] item_param[%s] service_fee[%d] trade_credit[%d] order_id[%lld]",
			msg->role_id, msg->item_wrapper.item_id, msg->item_wrapper.num, msg->item_wrapper.item_unique_id, NetValueItemParam::GetParamLog(msg->item_wrapper.item_id, msg->item_wrapper.param_data), msg->service_fee, msg->trade_credit, msg->order_id);

		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(msg->role_id));
		if (NULL != role) EventHandler::Instance().OnTradeMarketPutaway(role);

		
		OrderManager::Instance().OnOrderFinish(msg->order_id);
		
		return;
	}
	else
	{
		OrderManager::Instance().OnOrderFailed(msg->order_id, true); // 统一返还（即使返还失败，也可通过下面的日志进行申诉）
		
		//this->GiveResourceToUser(IntToUid(msg->role_id), &msg->item_wrapper, 0, msg->service_fee, msg->trade_credit, __FUNCTION__, false);

		gamelog::g_log_trade_market.printf(LL_WARNING, "OnOriginUserRequestUpshelveResp FAIL, role[%d] item[%d] num[%d] unique_item_id[%lld] item_param[%s] service_fee[%d] trade_credit[%d] order_id[%lld] result[%d]",
			msg->role_id, msg->item_wrapper.item_id, msg->item_wrapper.num, msg->item_wrapper.item_unique_id, NetValueItemParam::GetParamLog(msg->item_wrapper.item_id, msg->item_wrapper.param_data), msg->service_fee, msg->trade_credit, msg->order_id, msg->result);
		return;
	}
}

void TradeMarketManager::OnOriginUserOffshelveReq(crossgameprotocal::GameHiddenTradeMarketOffshelveReq* msg)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	UserTradeInfo* user_trade_info = this->GetUserTradeInfo(msg->role_id);
	if (NULL == user_trade_info)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_REMOVE_ITEM_NOT_FOUND);
		return;
	}

	const UserSaleItem* sale_item = user_trade_info->GetSaleItem(msg->sale_index);
	if (NULL == sale_item)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_REMOVE_ITEM_NOT_FOUND);
		return;
	}

	if (sale_item->GetSaleItemParamConst().sold_num > 0)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_CASH_OUT_FIRST);
		return;
	}

	static ItemDataWrapper wrapper;
	wrapper = sale_item->GetSaleItemParamConst().item_wrapper;
	int gold_price = sale_item->GetSaleItemParamConst().gold_price;
	int group_num = wrapper.num / sale_item->GetSaleItemParamConst().num_per_group;
	long long return_credit_fee = 1LL * gold_price * group_num;
	if (!user_trade_info->OnRemoveSaleItem(msg->sale_index, __FUNCTION__))
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_UNKNOWN_ERROR);
		return;
	}

	crossgameprotocal::HiddenGameTradeMarketOffshelveResp resp;
	resp.role_id = msg->role_id;
	resp.sale_index = msg->sale_index;
	resp.result = crossgameprotocal::HiddenGameTradeMarketOffshelveResp::RESULT_SUCC;
	resp.item_wrapper = wrapper;
	resp.return_credit = return_credit_fee;

	InternalComm::Instance().SendToGameThroughCross(msg->plat_type, msg->server_id, (const char*)&resp, sizeof(resp));

	this->SendShelveChangeNotice(msg->role_id, msg->sale_index, Protocol::SCTradeMarketUserShelveData::REASON_NOTIFY_REMOVE);

	LOG_QUICK12(gamelog::g_log_quick,
		LOG_TYPE_TRADE_MARTKET_DEL, msg->role_id, msg->role_name, NULL,
		wrapper.item_id, wrapper.num, NULL, NULL,
		0, gold_price, 0, 0, 0);
}

void TradeMarketManager::OnOriginUserOffshelveResp(crossgameprotocal::HiddenGameTradeMarketOffshelveResp* msg)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (crossgameprotocal::HiddenGameTradeMarketOffshelveResp::RESULT_SUCC == msg->result)
	{
		gamelog::g_log_trade_market.printf(LL_INFO, "OnOriginUserRequestUpshelveResp SUCC, role[%d] item[%d] num[%d] unique_item_id[%lld]",
			msg->role_id, msg->item_wrapper.item_id, msg->item_wrapper.num, msg->item_wrapper.item_unique_id);

		this->GiveResourceToUser(IntToUid(msg->role_id), &msg->item_wrapper, 0, 0, msg->return_credit, __FUNCTION__, false);
	}
	else
	{
		gamelog::g_log_trade_market.printf(LL_WARNING, "OnOriginUserOffshelveResp FAIL, role[%d] item[%d] num[%d] unique_item_id[%lld] result[%d]",
			msg->role_id, msg->item_wrapper.item_id, msg->item_wrapper.num, msg->item_wrapper.item_unique_id, msg->result);
	}
}

void TradeMarketManager::OnOriginUserRedoUpshelveReq(crossgameprotocal::GameHiddenTradeMarketRedoUpshelveReq* msg)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (msg->price <= 0)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_INPUT_PARAM_ERROR);
		this->SendOriginUserRedoUpshelveResp(msg->plat_type, msg->server_id, msg->role_id, msg->sale_index, crossgameprotocal::HiddenGameTradeMarketRedoUpshelveResp::RESULT_INPUT_PARAM_ERROR);
		return;
	}

	UserTradeInfo* info = this->GetUserTradeInfo(msg->role_id);
	if (NULL == info)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_REDO_ADD_ITEM_ON_SHELVE_FAILED);
		this->SendOriginUserRedoUpshelveResp(msg->plat_type, msg->server_id, msg->role_id, msg->sale_index, crossgameprotocal::HiddenGameTradeMarketRedoUpshelveResp::RESULT_GET_INFO_FAILED);
		return;
	}

	const UserSaleItem* sale_item = info->GetSaleItem(msg->sale_index);
	if (NULL == sale_item)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_REDO_ADD_ITEM_ON_SHELVE_FAILED);
		this->SendOriginUserRedoUpshelveResp(msg->plat_type, msg->server_id, msg->role_id, msg->sale_index, crossgameprotocal::HiddenGameTradeMarketRedoUpshelveResp::RESULT_GET_INFO_FAILED);
		return;
	}

	if (!sale_item->IsOffShelve())
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_ITEM_NOT_OFF_SHELVE);
		this->SendOriginUserRedoUpshelveResp(msg->plat_type, msg->server_id, msg->role_id, msg->sale_index, crossgameprotocal::HiddenGameTradeMarketRedoUpshelveResp::RESULT_NOT_OFFSHELVE);
		return;
	}

	if (sale_item->GetSaleItemParamConst().sold_num > 0)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_CASH_OUT_FIRST);
		this->SendOriginUserRedoUpshelveResp(msg->plat_type, msg->server_id, msg->role_id, msg->sale_index, crossgameprotocal::HiddenGameTradeMarketRedoUpshelveResp::RESULT_CASH_OUT_FIRST);
		return;
	}

	if (sale_item->GetSaleItemParamConst().item_wrapper.num <= 0)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_ITEM_SOLD_OUT);
		this->SendOriginUserRedoUpshelveResp(msg->plat_type, msg->server_id, msg->role_id, msg->sale_index, crossgameprotocal::HiddenGameTradeMarketRedoUpshelveResp::RESULT_SOLD_OUT);
		return;
	}

	const ItemBase* item_base = ITEMPOOL->GetItem(sale_item->GetSaleItemParamConst().item_wrapper.item_id);
	if (NULL == item_base)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_REDO_ADD_ITEM_ON_SHELVE_FAILED);
		this->SendOriginUserRedoUpshelveResp(msg->plat_type, msg->server_id, msg->role_id, msg->sale_index, crossgameprotocal::HiddenGameTradeMarketRedoUpshelveResp::RESULT_GET_INFO_FAILED);
		return;
	}

	if (!item_base->CanTrade())
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_ITEM_CAN_NOT_TRADE);
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_REDO_ADD_ITEM_ON_SHELVE_FAILED);

		this->SendOriginUserRedoUpshelveResp(msg->plat_type, msg->server_id, msg->role_id, msg->sale_index, crossgameprotocal::HiddenGameTradeMarketRedoUpshelveResp::RESULT_CANNOT_TRADE);
		return;
	}

	if (item_base->GetTradeNumPerGroup() != sale_item->GetSaleItemParamConst().num_per_group)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_ITEM_NUM_PER_GROUP_CHANGED);
		this->SendOriginUserRedoUpshelveResp(msg->plat_type, msg->server_id, msg->role_id, msg->sale_index, crossgameprotocal::HiddenGameTradeMarketRedoUpshelveResp::RESULT_ITEM_CONFIG_ERROR);
		return;
	}

	if (sale_item->GetSaleItemParamConst().gold_price > TradeMarketManager::GetTradeMaxPrice(item_base, &sale_item->GetSaleItemParamConst().item_wrapper) ||
		sale_item->GetSaleItemParamConst().gold_price < TradeMarketManager::GetTradeMinPrice(item_base, &sale_item->GetSaleItemParamConst().item_wrapper))
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_PRICE_NOT_IN_RANGE);
		this->SendOriginUserRedoUpshelveResp(msg->plat_type, msg->server_id, msg->role_id, msg->sale_index, crossgameprotocal::HiddenGameTradeMarketRedoUpshelveResp::RESULT_PRICE_NOT_IN_RANGE);
		return;
	}

	int errnum = 0;
	if (!this->CanSellItem(msg->role_id, sale_item->GetSaleItemParamConst(), &errnum))
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errnum);
		return;
	}

	if (!info->OnItemRedoAddOnShelve(msg->sale_index, msg->price))
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_REDO_ADD_ITEM_ON_SHELVE_FAILED);
		this->SendOriginUserRedoUpshelveResp(msg->plat_type, msg->server_id, msg->role_id, msg->sale_index, crossgameprotocal::HiddenGameTradeMarketRedoUpshelveResp::RESULT_REDO_ADD_FAILED);
		return;
	}
	else
	{
		this->SendShelveChangeNotice(msg->role_id, msg->sale_index, Protocol::SCTradeMarketUserShelveData::REASON_NOTIFY_CHANGE);
		this->SendOriginUserRedoUpshelveResp(msg->plat_type, msg->server_id, msg->role_id, msg->sale_index, crossgameprotocal::HiddenGameTradeMarketRedoUpshelveResp::RESULT_SUCC);
	}
}

void TradeMarketManager::OnOriginUserRedoUpshelveResp(crossgameprotocal::HiddenGameTradeMarketRedoUpshelveResp* msg)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	gamelog::g_log_trade_market.printf(LL_INFO, "OnOriginUserRedoUpshelveResp role[%d] sale_index[%d] result[%d]", msg->role_id, msg->sale_index, msg->result);
}

bool TradeMarketManager::OnOriginUserCashOutReq(crossgameprotocal::GameHiddenTradeMarketCashOutReq* msg)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return false;
	}

	UserTradeInfo* user_trade_info = this->GetUserTradeInfo(msg->role_id);
	if (NULL == user_trade_info)
	{
		this->SendOriginUserCashOutResp(msg->plat_type, msg->server_id, msg->role_id, 0, crossgameprotocal::HiddenGameTradeMarketCashOutResp::RESULT_ITEM_NOT_FOUND);
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_CASH_OUT_ITEM_NOT_FOUND);
		return false;
	}

	const UserSaleItem* sale_item = user_trade_info->GetSaleItem(msg->sale_index);
	if (NULL == sale_item)
	{
		this->SendOriginUserCashOutResp(msg->plat_type, msg->server_id, msg->role_id, 0, crossgameprotocal::HiddenGameTradeMarketCashOutResp::RESULT_ITEM_NOT_FOUND);
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_CASH_OUT_ITEM_NOT_FOUND);
		return false;
	}

	if (sale_item->GetSaleItemParamConst().sold_num <= 0)
	{
		this->SendOriginUserCashOutResp(msg->plat_type, msg->server_id, msg->role_id, 0, crossgameprotocal::HiddenGameTradeMarketCashOutResp::RESULT_NO_SOLD_NUM);
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_ITEM_CAN_NOT_CASH_OUT);
		return false;
	}

	long long add_gold = 0;
	if (!user_trade_info->OnCashOutItem(msg->sale_index, &add_gold))
	{
		this->SendOriginUserCashOutResp(msg->plat_type, msg->server_id, msg->role_id, 0, crossgameprotocal::HiddenGameTradeMarketCashOutResp::RESULT_CASH_OUT_FAILED);
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_ITEM_CASH_OUT_FAILED);
		return false;
	}

	UniqueServerID usid(msg->plat_type, msg->server_id);
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(usid);
	if (NULL != ramgr && ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_REDUCE_TAX_RATE))
	{
		const RandActivityReduceTaxRateConfig  * config = static_cast<const RandActivityReduceTaxRateConfig  *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_REDUCE_TAX_RATE));
		if (NULL != config)
		{
			if (add_gold > 0)
			{
				long long tax = static_cast<long long>(1.0 * add_gold * config->GetRateFromReduceTaxRateSectionCfg(ramgr) / TRADE_MARKET_PERCENT_NUM);
				tax = GetMax(tax, 1LL);
				add_gold = GetMax(add_gold - tax, 1LL);
			}
		}
	}
	else
	{
		if (add_gold > 0)
		{
			long long tax = static_cast<long long>(1.0 * add_gold * TRADEMARKET_CFG.GetConstantCfg().tax_rate / TRADE_MARKET_PERCENT_NUM);
			tax = GetMax(tax, 1LL);
			add_gold = GetMax(add_gold - tax, 1LL);
		}
	}

	this->SendOriginUserCashOutResp(msg->plat_type, msg->server_id, msg->role_id, add_gold, crossgameprotocal::HiddenGameTradeMarketCashOutResp::RESULT_SUCC);

	this->SendShelveChangeNotice(msg->role_id, msg->sale_index, Protocol::SCTradeMarketUserShelveData::REASON_NOTIFY_CHANGE);

	return true;
}

bool TradeMarketManager::OnOriginUserCashOutResp(crossgameprotocal::HiddenGameTradeMarketCashOutResp* msg)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return false;
	}

	if (msg->result == crossgameprotocal::HiddenGameTradeMarketCashOutResp::RESULT_SUCC)
	{
		this->GiveResourceToUser(IntToUid(msg->role_id), NULL, msg->add_gold, 0, 0, __FUNCTION__, false);
		gamelog::g_log_trade_market.printf(LL_INFO, "OnOriginUserCashOutResp SUCC, role[%d] add_gold[%d]", msg->role_id, msg->add_gold);
	}
	else
	{
		gamelog::g_log_trade_market.printf(LL_INFO, "OnOriginUserCashOutResp ERROR, role[%d] result[%d]", msg->role_id, msg->result);
	}
	
	return true;
}

bool TradeMarketManager::OnOriginUserAdvertiseReq(crossgameprotocal::GameHiddenTradeMarketAdvertiseReq* msg)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return false;
	}

	UserTradeInfo* info = this->GetUserTradeInfo(msg->role_id);
	if (NULL == info)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_UNKNOWN_ERROR);
		return false;
	}

	const UserSaleItem* usi = info->GetSaleItem(msg->sale_index);
	if (NULL == usi)
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_UNKNOWN_ERROR);
		return false;
	}
	const ItemBase* item = ITEMPOOL->GetItem(usi->GetSaleItemParamConst().item_wrapper.item_id);
	if (NULL == item) return false;

	if (!usi->IsAnnouncing() && !usi->IsOnSale())
	{
		this->SendNoticeNumToOriginRole(msg->role_id, errornum::EN_TRADE_MARKET_NOT_ANOUNCE_OR_SALE);
		return false;
	}

	short service_fee_rate = TRADEMARKET_CFG.GetConstantCfg().advertise_fee_rate;
	long long total_price = 1LL * usi->GetSaleItemParamConst().gold_price * usi->GetSaleItemParamConst().item_wrapper.num;
	long long service_fee = static_cast<long long>(1.0 * total_price * service_fee_rate / TRADE_MARKET_PERCENT_NUM);
	service_fee = GetMax(static_cast<long long>(TRADEMARKET_CFG.GetConstantCfg().gold_advertise_fee_min), service_fee);
	service_fee = GetMin(static_cast<long long>(TRADEMARKET_CFG.GetConstantCfg().gold_advertise_fee_max), service_fee);

	crossgameprotocal::HiddenGameTradeMarketAdvertiseResp resp;
	resp.sale_index = msg->sale_index;
	resp.role_id = msg->role_id;
	resp.gold_cost = service_fee;

	resp.item_id = item->GetItemId();
	F_STRNCPY(resp.item_name, item->GetItemName(), sizeof(resp.item_name));
	resp.price = usi->GetSaleItemParamConst().gold_price;
	resp.put_on_shelve_time = usi->GetSaleItemParamConst().put_on_shelve_time;
	resp.sale_index = msg->sale_index;
	resp.unique_key = usi->GetSaleItemParamConst().unique_key;

	return InternalComm::Instance().SendToGameThroughCross(msg->plat_type, msg->server_id, (const char*)&resp, sizeof(resp));
}

bool TradeMarketManager::OnOriginUserAdvertiseResp(crossgameprotocal::HiddenGameTradeMarketAdvertiseResp* msg)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return false;
	}

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(msg->role_id));
	if (NULL == role)
	{
		return false;
	}

	if (!role->GetRoleModuleManager()->GetMoney()->GoldMoreThan(msg->gold_cost))
	{
		role->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return false;
	}

	if (!role->GetRoleModuleManager()->GetMoney()->UseGold(msg->gold_cost, __FUNCTION__))
	{
		role->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return false;
	}

	crossgameprotocal::GameHiddenTradeMarketAdvertiseConfirmBoardcast boardcast;
	boardcast.role_id = msg->role_id;
	boardcast.plat_type = LocalConfig::Instance().GetPlatType();
	boardcast.server_id = LocalConfig::Instance().GetMergeServerId();

	boardcast.item_id = msg->item_id;
	F_STRNCPY(boardcast.item_name, msg->item_name, sizeof(boardcast.item_name));
	boardcast.price = msg->price;
	boardcast.put_on_shelve_time = msg->put_on_shelve_time;
	boardcast.sale_index = msg->sale_index;
	boardcast.unique_key = msg->unique_key;

	
	return InternalComm::Instance().SendToHiddenThroughCross((const char*)&boardcast, sizeof(boardcast));;
}

bool TradeMarketManager::OnOriginUserAdvertiseConfirmBoardcast(crossgameprotocal::GameHiddenTradeMarketAdvertiseConfirmBoardcast* msg)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return false;
	}

	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_trade_market_advertise,
		msg->item_name, msg->role_id, msg->sale_index,
		msg->put_on_shelve_time, msg->price, msg->item_id, msg->unique_key, msg->put_on_shelve_time);
	if (length > 0)
	{
		Protocol::SCSystemMsg sm;
		int sendlen = 0;
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
		{
			InternalComm::Instance().SendToGameAllUserThroughCross(msg->plat_type, msg->server_id, (const char*)&sm, sendlen);
			World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
		}
	}

	return true;
}

bool TradeMarketManager::OnOriginUserQueryItemRecommendPriceReq(crossgameprotocal::GameHiddenQueryItemRecommendPrice* msg)
{
	const ItemBase* item = ITEMPOOL->GetItem(msg->item_wrapper.item_id);
	if (NULL == item)
	{
		return false;
	}

	int min_price = 0;
	int max_price = 0;
	int recommend_price = 0;
	TradeMarketManager::QueryItemTradePrice(item, &msg->item_wrapper, &min_price, &max_price, &recommend_price);

	Protocol::SCQueryItemRecommendPriceResp resp;
	resp.reserve_ch = msg->reserve_ch;
	resp.reserve_sh = msg->reserve_sh;
	resp.is_from_pet = msg->is_from_pet;
	resp.index = msg->index;
	resp.column = msg->column;
	resp.min_price = min_price;
	resp.max_price = max_price;
	resp.price = recommend_price;

	return InternalComm::Instance().SendToGameUserThroughCross(msg->role_id, (const char*)&resp, sizeof(resp));
}

void TradeMarketManager::SendOriginUserRequestUpshelveResp(int plat_type, int server_id, int role_id, int result, const ItemDataWrapper& wrapper, int service_fee, int trade_credit, long long order_id)
{
	crossgameprotocal::HiddenGameTradeMarketRequestUpshelveResp resp;
	resp.role_id = role_id;
	resp.result = result;
	resp.service_fee = service_fee;
	resp.trade_credit = trade_credit;
	resp.item_wrapper = wrapper;
	resp.order_id = order_id;

	InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, (const char*)&resp, sizeof(resp));
}

void TradeMarketManager::SendOriginUserRedoUpshelveResp(int plat_type, int server_id, int role_id, int sale_index, int result)
{
	crossgameprotocal::HiddenGameTradeMarketRedoUpshelveResp resp;
	resp.role_id = role_id;
	resp.sale_index = sale_index;
	resp.result = result;

	InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, (const char*)&resp, sizeof(resp));
}

void TradeMarketManager::SendOriginUserCashOutResp(int plat_type, int server_id, int role_id, int add_gold, int result)
{
	crossgameprotocal::HiddenGameTradeMarketCashOutResp resp;
	resp.add_gold = add_gold;
	resp.result = result;
	resp.role_id = role_id;

	InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, (const char*)&resp, sizeof(resp));
}

void TradeMarketManager::SendOriginUserConfirmBuyErrorResp(int plat_type, int server_id, int role_id, int seller_uid, int sale_index, int buy_num, ItemID buy_item_id, UniqueKeyStr unique_key, long long order_id, int error_num, const char* error_msg)
{
	crossgameprotocal::HiddenGameTradeMarketRequestBuyConfirmErrorResp resp;
	resp.role_id = role_id;
	resp.seller_uid = seller_uid;
	resp.sale_index = sale_index;
	resp.buy_num = buy_num;
	resp.buy_item_id = buy_item_id;
	resp.reserve_sh = 0;
	F_STRNCPY(resp.unique_key_str, unique_key, sizeof(resp.unique_key_str));
	F_STRNCPY(resp.error_msg, error_msg, sizeof(resp.error_msg));
	resp.error_num = error_num;
	resp.order_id = order_id;

	InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, (const char*)&resp, sizeof(resp));
}

bool TradeMarketManager::AddItemOnSystemShelve(SaleItemParam& param, std::string& error_msg)
{
	const ItemBase* item = ITEMPOOL->GetItem(param.item_wrapper.item_id);
	if (NULL == item)
	{
		error_msg = "Item Not Exist";
		return false;
	}

	if (!item->CanTrade())
	{
		error_msg = "Item Can Not Trade";
		return false;
	}

	if (param.gold_price < TradeMarketManager::GetTradeMinPrice(item, &param.item_wrapper))
	{
		error_msg = "Item Min Price Error";
		return false;
	}

	if (param.gold_price > TradeMarketManager::GetTradeMaxPrice(item, &param.item_wrapper))
	{
		error_msg = "Item Max Price Error";
		return false;
	}

	if (param.item_wrapper.num % item->GetTradeNumPerGroup() != 0)
	{
		error_msg = "Item Num Per Group Error";
		return false;
	}

	if (item->GetItemType() >= I_TYPE_NET_VALUE_ITEM_BEGIN)
	{
		// 净值物品
		if (0 == param.item_wrapper.has_param)
		{
			error_msg = "Net Value Item Error";
			return false;
		}
	}

	if (0 == param.item_wrapper.invalid_time)
	{
		param.item_wrapper.invalid_time = item->CalInvalidTime();
	}
	else
	{
		param.item_wrapper.invalid_time = GetMin(param.item_wrapper.invalid_time, item->CalInvalidTime());
	}

	param.cur_status = SALE_ITEM_STATUS_ON_SHELVE;
	param.is_need_announcement = TradeMarketConfig::Instance().IsNeedAnnounce(item);
	param.item_search_type = item->GetTradeSearchType();
	param.num_per_group = item->GetTradeNumPerGroup();

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	param.put_on_shelve_time = now;

	//获取unique_key
	param.unique_key = TradeMarketManager::Instance().GetUniqueItemKey();
	if (0 == param.unique_key)
	{
		error_msg = "Unique Key Error";
		return false;
	}

	if (0 == param.item_wrapper.item_unique_id)
	{
		UniqueIDGenerator::Instance()->GenUniqueID(&param.item_wrapper.item_unique_id);
	}

#ifdef _WIN32
	param.next_status_time = now + 5; // 方便调试
#else
	param.next_status_time = now + EngineAdapter::Instance().NextHourInterval(0, 0);
#endif	

	if (!m_sys_trade_info.OnAddSaleItem(param.unique_key, SYSTEM_TRADE_UID, "Alice", param, false))
	{
		error_msg = "Add On System Shelve Error";
		return false;
	}
	else
	{
#ifdef TRACK_ITEM_FLAG
		gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG TradeMarketManager::AddItemOnSystemShelve item_id[%d] item_unique_id[%lld] unique_key[%lld]",
			param.item_wrapper.item_id, param.item_wrapper.item_unique_id, param.unique_key);
#endif
	}

	return true;
}

bool TradeMarketManager::SendSearchResult(Role* role, int count, SaleItemBase** item_list, int page, int total_page, int order_type, bool is_search_announce, int cursor_index)
{
	Protocol::SCTradeMarketSearchResult result;
	result.item_count = 0;

	this->MakeSearchResult(&result, count, item_list, page, total_page, order_type, is_search_announce, cursor_index);

	return EngineAdapter::Instance().NetSendSerializeMsg(role->GetNetId(), &result);
}

bool TradeMarketManager::SendSearchPetForQuickBuyResult(Role* role, short order_type, int count, SaleItemBase** item_list)
{
	Protocol::SCTradeMarketSearchPetForQuickBuy resp;
	this->AssembleSearchPetForQuickBuyResult(&resp, order_type, count, item_list);

	return EngineAdapter::Instance().NetSendSerializeMsg(role->GetNetId(), &resp);
}

void TradeMarketManager::AssembleSearchPetForQuickBuyResult(Protocol::SCTradeMarketSearchPetForQuickBuy* result, short order_type, int count, SaleItemBase** item_list)
{
	result->order_type = static_cast<short>(order_type);
	result->item_count = 0;
	for (int i = 0; i < count && result->item_count < SEARCH_PET_QUICK_BUY_ITEM_NUM; ++i)
	{
		if (NULL != item_list[i])
		{
			int gold_price = item_list[i]->GetSaleItemParamConst().gold_price;
			int immortal_price = item_list[i]->GetSaleItemParamConst().immortal_coin_price;
			if (gold_price > 0)
			{
				result->item_list[result->item_count].price = gold_price;
				result->item_list[result->item_count].price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD;
			}
			else
			{
				result->item_list[result->item_count].price = immortal_price;
				result->item_list[result->item_count].price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_IMMORTAL_COIN;
			}
			result->item_list[result->item_count].cur_item_status = static_cast<short>(item_list[i]->GetSaleItemParamConst().cur_status);
			result->item_list[result->item_count].next_status_time = item_list[i]->GetSaleItemParamConst().next_status_time;

			result->item_list[result->item_count].item_detail.item_id = item_list[i]->GetSaleItemParamConst().item_wrapper.item_id;
			result->item_list[result->item_count].item_detail.item_num = item_list[i]->GetSaleItemParamConst().item_wrapper.num;
			result->item_list[result->item_count].item_detail.invalid_time = item_list[i]->GetSaleItemParamConst().item_wrapper.invalid_time;
			result->item_list[result->item_count].item_detail.param_length = 0;
			const ItemBase* item = ITEMPOOL->GetItem(item_list[i]->GetSaleItemParamConst().item_wrapper.item_id);
			if (NULL != item)
			{
				int param_length = NetValueItemParam::GetDataLengthByItemType(item->GetItemType());
				result->item_list[result->item_count].item_detail.param_length = param_length;
				memcpy(result->item_list[result->item_count].item_detail.data, item_list[i]->GetSaleItemParamConst().item_wrapper.param_data, param_length);
			}

			result->item_list[result->item_count].seller_uid = item_list[i]->GetSellerUid();
			result->item_list[result->item_count].sale_index = item_list[i]->GetSaleItemParamConst().sale_index;
			result->item_list[result->item_count].upshelve_time = item_list[i]->GetSaleItemParamConst().put_on_shelve_time;
			F_STRNCPY(result->item_list[result->item_count].unique_key_str, item_list[i]->GetUniqueKeyStr(), sizeof(UniqueKeyStr));

			result->item_count += 1;
		}
	}
}

UserTradeInfo* TradeMarketManager::CreateUserTradeInfo(int seller_uid)
{
	UserTradeInfoMap::iterator it = m_user_trade_info_map.find(seller_uid);
	if (it == m_user_trade_info_map.end())
	{
		UserTradeInfo* info = new UserTradeInfo(seller_uid);
		it = m_user_trade_info_map.insert(UserTradeInfoMap::value_type(seller_uid, info)).first;
	}

	return it->second;
}

void TradeMarketManager::MakeShelveData(int uid, Protocol::SCTradeMarketUserShelveData* msg)
{
	msg->item_count = 0;
	msg->reason = Protocol::SCTradeMarketUserShelveData::REASON_NORMAL;

	UserTradeInfo* user_trade_info = this->GetUserTradeInfo(uid);
	if (NULL != user_trade_info)
	{
		int count = 0;
		for (int idx = 0; idx < USER_SALE_ITEM_MAX_NUM; ++idx)
		{
			const UserSaleItem* si = user_trade_info->GetSaleItem(idx);
			if (NULL == si) continue;

			const SaleItemParam& sip = si->GetSaleItemParamConst();
			msg->item_list[count].sale_index = idx;
			msg->item_list[count].cur_status = sip.cur_status;
			msg->item_list[count].next_status_time = sip.next_status_time;
			if (sip.gold_price > 0)
			{
				msg->item_list[count].price = sip.gold_price;
				msg->item_list[count].price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD;
			}
			else
			{
				msg->item_list[count].price = sip.immortal_coin_price;
				msg->item_list[count].price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_IMMORTAL_COIN;
			}
			msg->item_list[count].sold_num = sip.sold_num;

			msg->item_list[count].item_detail.item_id = sip.item_wrapper.item_id;
			msg->item_list[count].item_detail.item_num = sip.item_wrapper.num;
			msg->item_list[count].item_detail.invalid_time = sip.item_wrapper.invalid_time;
			msg->item_list[count].item_detail.param_length = 0;
			const ItemBase* item = ITEMPOOL->GetItem(sip.item_wrapper.item_id);
			if (NULL != item)
			{
				int param_length = NetValueItemParam::GetDataLengthByItemType(item->GetItemType());
				msg->item_list[count].item_detail.param_length = param_length;
				memcpy(msg->item_list[count].item_detail.data, sip.item_wrapper.param_data, param_length);
			}

			++count;
		}
		msg->item_count = count;

	}
}

void TradeMarketManager::MakeSearchResult(Protocol::SCTradeMarketSearchResult* result, int count, SaleItemBase** item_list, int page, int total_page, int order_type, bool is_search_announce, int cursor_index)
{
	if (NULL == result || NULL == item_list) return;

	result->item_count = 0;
	result->cursor_index = cursor_index;
	result->is_announce = is_search_announce ? 1 : 0;
	result->cur_page = static_cast<short>(page);
	result->total_page = static_cast<short>(total_page);
	result->order_type = static_cast<short>(order_type);
	for (int i = 0; i < count && result->item_count < SEARCH_SALE_ITEM_PER_PAGE; ++i)
	{
		if (NULL != item_list[i])
		{
			int gold_price = item_list[i]->GetSaleItemParamConst().gold_price;
			int immortal_price = item_list[i]->GetSaleItemParamConst().immortal_coin_price;
			if (gold_price > 0)
			{
				result->item_list[result->item_count].price = gold_price;
				result->item_list[result->item_count].price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD;
			}
			else
			{
				result->item_list[result->item_count].price = immortal_price;
				result->item_list[result->item_count].price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_IMMORTAL_COIN;
			}
			result->item_list[result->item_count].cur_item_status = static_cast<short>(item_list[i]->GetSaleItemParamConst().cur_status);
			result->item_list[result->item_count].next_status_time = item_list[i]->GetSaleItemParamConst().next_status_time;

			result->item_list[result->item_count].item_detail.item_id = item_list[i]->GetSaleItemParamConst().item_wrapper.item_id;
			result->item_list[result->item_count].item_detail.item_num = item_list[i]->GetSaleItemParamConst().item_wrapper.num;
			result->item_list[result->item_count].item_detail.invalid_time = item_list[i]->GetSaleItemParamConst().item_wrapper.invalid_time;
			result->item_list[result->item_count].item_detail.param_length = 0;
			const ItemBase* item = ITEMPOOL->GetItem(item_list[i]->GetSaleItemParamConst().item_wrapper.item_id);
			if (NULL != item)
			{
				int param_length = NetValueItemParam::GetDataLengthByItemType(item->GetItemType());
				result->item_list[result->item_count].item_detail.param_length = param_length;
				memcpy(result->item_list[result->item_count].item_detail.data, item_list[i]->GetSaleItemParamConst().item_wrapper.param_data, param_length);
			}

			result->item_list[result->item_count].seller_uid = item_list[i]->GetSellerUid();
			result->item_list[result->item_count].sale_index = item_list[i]->GetSaleItemParamConst().sale_index;
			result->item_list[result->item_count].upshelve_time = item_list[i]->GetSaleItemParamConst().put_on_shelve_time;
			F_STRNCPY(result->item_list[result->item_count].unique_key_str, item_list[i]->GetUniqueKeyStr(), sizeof(UniqueKeyStr));

			result->item_count += 1;
		}
	}
}

void TradeMarketManager::MakeUserTradeRecordData(int role_id, Protocol::SCTradeMarketGetRoleTransactionRecord* data)
{
	UserTradeInfo* info = this->GetUserTradeInfo(role_id);
	if (NULL == info) return;

	const RoleTransactionRecordParam& record_param = info->GetRoleRecord();
	data->total_buy_gold = record_param.total_buy_gold;
	data->total_sold_gold = record_param.total_sold_gold;
	
	const UserTradeRecord* buy_record = info->GetBuyRecord();
	if (NULL != buy_record)
	{
		buy_record->AssembleRecordsForMsg(ARRAY_ITEM_COUNT(data->buy_list), data->buy_list, &data->buy_record_num);
	}

	const UserTradeRecord* sold_record = info->GetSoldRecord();
	if (NULL != sold_record)
	{
		sold_record->AssembleRecordsForMsg(ARRAY_ITEM_COUNT(data->sold_list), data->sold_list, &data->sold_record_num);
	}
}

void TradeMarketManager::MakeUserFocusListData(int role_id, Protocol::SCTradeMarketGetFocusList* data)
{
	data->sale_count = 0;
	data->announce_count = 0;

	UserTradeInfo* info = this->GetUserTradeInfo(role_id);
	if (NULL == info)
	{
		return;
	}

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	info->UpdateFocusItemList(now);

	for (int i = 0; i < TRADE_MARKET_USER_FOCUS_MAX_NUM; ++i)
	{
		const UserSaleItem* usi = info->GetFocusItem(i);
		if (NULL == usi) continue;
		if (usi->GetSaleItemParamConst().cur_status != SALE_ITEM_STATUS_ANNOUNCEMENT &&
			usi->GetSaleItemParamConst().cur_status != SALE_ITEM_STATUS_ON_SALE) continue;

		Protocol::FocusItemInfo* focus_list = NULL;
		short* list_count = NULL;
		if (SALE_ITEM_STATUS_ON_SALE == usi->GetSaleItemParamConst().cur_status)
		{
			if (data->sale_count >= SEARCH_SALE_ITEM_PER_PAGE) continue;

			focus_list = data->sale_focus_list;
			list_count = &data->sale_count;
		}
		else
		{
			if (data->announce_count >= SEARCH_SALE_ITEM_PER_PAGE) continue;

			focus_list = data->announce_focus_list;
			list_count = &data->announce_count;
		}

		focus_list[*list_count].next_status_time = usi->GetSaleItemParamConst().next_status_time;
		focus_list[*list_count].cur_item_status = usi->GetSaleItemParamConst().cur_status;
		focus_list[*list_count].price_type = usi->GetSaleItemParamConst().gold_price > 0 ? ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD : ItemNamespace::ITEM_TRADE_MONEY_TYPE_IMMORTAL_COIN;
		focus_list[*list_count].price = usi->GetSaleItemParamConst().gold_price > 0 ? usi->GetSaleItemParamConst().gold_price : usi->GetSaleItemParamConst().immortal_coin_price;

		focus_list[*list_count].item_detail.item_id = usi->GetSaleItemParamConst().item_wrapper.item_id;
		focus_list[*list_count].item_detail.item_num = usi->GetSaleItemParamConst().item_wrapper.num;
		focus_list[*list_count].item_detail.invalid_time = usi->GetSaleItemParamConst().item_wrapper.invalid_time;
		focus_list[*list_count].item_detail.param_length = 0;
		const ItemBase* item = ITEMPOOL->GetItem(usi->GetSaleItemParamConst().item_wrapper.item_id);
		if (NULL != item)
		{
			int param_length = NetValueItemParam::GetDataLengthByItemType(item->GetItemType());
			focus_list[*list_count].item_detail.param_length = param_length;
			memcpy(focus_list[*list_count].item_detail.data, usi->GetSaleItemParamConst().item_wrapper.param_data, param_length);
		}

		focus_list[*list_count].seller_uid = usi->GetSellerUid();
		focus_list[*list_count].sale_index = usi->GetSaleItemParamConst().sale_index;
		focus_list[*list_count].focus_index = i;
		focus_list[*list_count].upshelve_time = usi->GetSaleItemParamConst().put_on_shelve_time;
		F_STRNCPY(focus_list[*list_count].unique_key_str, usi->GetUniqueKeyStr(), sizeof(UniqueKeyStr));
		(*list_count) += 1;
	}
}

void TradeMarketManager::MakeFocusItemInfoData(const UserSaleItem* seller_usi, int focus_index, Protocol::SCTradeMarketGetFocusItemInfo* data)
{
	data->info.cur_item_status = seller_usi->GetSaleItemParamConst().cur_status;
	data->info.focus_index = focus_index;

	data->info.item_detail.item_id = seller_usi->GetSaleItemParamConst().item_wrapper.item_id;
	data->info.item_detail.item_num = seller_usi->GetSaleItemParamConst().item_wrapper.num;
	data->info.item_detail.invalid_time = seller_usi->GetSaleItemParamConst().item_wrapper.invalid_time;
	data->info.item_detail.param_length = 0;
	const ItemBase* item = ITEMPOOL->GetItem(seller_usi->GetSaleItemParamConst().item_wrapper.item_id);
	if (NULL != item)
	{
		int param_length = NetValueItemParam::GetDataLengthByItemType(item->GetItemType());
		data->info.item_detail.param_length = param_length;
		memcpy(data->info.item_detail.data, seller_usi->GetSaleItemParamConst().item_wrapper.param_data, param_length);
	}

	data->info.next_status_time = seller_usi->GetSaleItemParamConst().next_status_time;
	data->info.price = seller_usi->GetSaleItemParamConst().gold_price;
	data->info.price_type = seller_usi->GetSaleItemParamConst().gold_price > 0 ? ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD : ItemNamespace::ITEM_TRADE_MONEY_TYPE_IMMORTAL_COIN;
	data->info.sale_index = seller_usi->GetSaleItemParamConst().sale_index;
	data->info.seller_uid = seller_usi->GetSellerUid();
	data->info.upshelve_time = seller_usi->GetSaleItemParamConst().put_on_shelve_time;
	F_STRNCPY(data->info.unique_key_str, seller_usi->GetUniqueKeyStr(), sizeof(UniqueKeyStr));
}

bool TradeMarketManager::IsSystemSeller(int seller_uid)
{
	return SYSTEM_TRADE_UID == seller_uid;
}

void TradeMarketManager::DoSystemBuy()
{
	std::vector<long long> unique_key_vec;
	m_sys_trade_info.GetSystemBuyList(&unique_key_vec);

	for (size_t i = 0; i < unique_key_vec.size(); ++i)
	{
		long long unique_key = unique_key_vec[i];
		SystemTradeItemInfo* stii = m_sys_trade_info.GetTradeItemInfo(unique_key);
		if (NULL == stii)
		{
			continue;
		}
		const SaleItemParam& param = stii->sale_item_ptr->GetSaleItemParamConst();
		const ItemBase* item = ITEMPOOL->GetItem(param.item_wrapper.item_id);
		if (NULL == item)
		{
			continue;
		}

		int buy_num = param.item_wrapper.num;
		int buy_group_num = param.item_wrapper.num / param.num_per_group;
		int single_price = param.gold_price;

		if (m_sys_trade_info.OnItemSold(unique_key, stii->sale_item_ptr->GetSaleItemParamConst().item_wrapper.num, SYSTEM_TRADE_UID, "SystemSelfBuy", "System"))
		{	
			int price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD;
			int param_length = NetValueItemParam::GetDataLengthByItemType(item->GetItemType());
			m_transaction_record.AddTransactionRecord(param.item_wrapper.item_id, param.item_wrapper.num, price_type, single_price, param_length, param.item_wrapper.param_data);
		
			gamelog::g_log_trade_market.printf(LL_INFO, "TradeMarketManager::DoSystemBuy SUCC, buyer[system] unique_key[%lld], item[%d] cur_num[%d] buy_num[%d] buy_group_num[%d]",
				unique_key, param.item_wrapper.item_id, buy_num, buy_num, buy_group_num);

#ifdef TRACK_ITEM_FLAG
			gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG TradeMarketManager::DoSystemBuy system by from system, item_id[%d] item_unique_id[%lld] unique_key[%lld]",
				param.item_wrapper.item_id,
				param.item_wrapper.item_unique_id,
				param.unique_key);
#endif
		}
		else
		{
			gamelog::g_log_trade_market.printf(LL_ERROR, "TradeMarketManager::DoSystemBuy FAIL, buyer[system] unique_key[%lld], item[%d] cur_num[%d] buy_num[%d] buy_group_num[%d]",
				unique_key, param.item_wrapper.item_id, buy_num, buy_num, buy_group_num);
		}
	}
}

void TradeMarketManager::DoSystemOffshelve()
{
	std::vector<long long> unique_key_vec;
	m_sys_trade_info.GetSystemOffshelveList(&unique_key_vec);

	for (size_t i = 0; i < unique_key_vec.size(); ++i)
	{
		long long unique_key = unique_key_vec[i];
		SystemTradeItemInfo* stii = m_sys_trade_info.GetTradeItemInfo(unique_key);
		if (NULL == stii)
		{
			continue;
		}
		const SaleItemParam& param = stii->sale_item_ptr->GetSaleItemParamConst();
		const ItemBase* item = ITEMPOOL->GetItem(param.item_wrapper.item_id);
		if (NULL == item)
		{
			continue;
		}

#ifdef TRACK_ITEM_FLAG
		gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG DoSystemOffshelve system_auto_offshelve item[%d] item_unique_id[%lld] unique_key[%lld]",
			param.item_wrapper.item_id, param.item_wrapper.item_unique_id, param.unique_key);
#endif

		if (!m_sys_trade_info.OnRemoveSaleItem(unique_key, __FUNCTION__))
		{
			gamelog::g_log_trade_market.printf(LL_WARNING, "DoSystemOffshelve FAILED unique_key[%lld]", unique_key);
		}
	}
}

bool TradeMarketManager::CanSellItem(int uid, const SaleItemParam& param, int* errnum)
{
	if (!this->CheckRAGodBeaseAdventCanSellItem(uid, param, errnum)) return false;

	return true;
}

bool TradeMarketManager::CheckRAGodBeaseAdventCanSellItem(int uid, const SaleItemParam& param, int* errnum)
{
	int plat_type = LocalConfig::Instance().GetPlatType();
	UniqueServerID usid(plat_type, IntToUid(uid).db_index);
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(usid);
	if (NULL == ramgr) return true;

	RandActivityGodBeastAdvent* activity = (RandActivityGodBeastAdvent*)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT);
	if (NULL == activity) return true;

	if (!activity->IsCurCycleExchangeItem(param.item_wrapper.item_id)) return true;

	unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
	unsigned int next_hour_timestamp = now + EngineAdapter::Instance().NextHourInterval(0, 0);
	if (activity->CanSell(next_hour_timestamp)) return true;

	if (NULL != errnum) *errnum = errornum::EN_TRADEMARKET_UPSHELVE_TIME_LIMIT;

	return false;
}

void TradeMarketManager::LogDeal(ItemID item_id, int price, int sold_num, int seller_uid, const char* seller_name, const char* buyer_name, int buyer_uid, const char* buyer_pname)
{
	int storage = 0;
	int high_price = 0;
	int low_price = MAX_INT;
	this->GetLogParam(item_id, &storage, &low_price, &high_price);

	std::string seller_uid_str = STRING_SPRINTF("%d", seller_uid);
	LOG_QUICK12(gamelog::g_log_quick, LOG_TYPE_TRADE_MARKET_DEAL, buyer_uid, buyer_name, buyer_pname, item_id, price, seller_name, seller_uid_str.c_str(), sold_num, storage, low_price, high_price, 0);
}

void TradeMarketManager::GetLogParam(ItemID item_id, ARG_OUT int* storage_num, ARG_OUT int* low_price, ARG_OUT int* high_price)
{
	m_sale_item_index.GetStorageInfo(item_id, storage_num, low_price, high_price);
}

int TradeMarketManager::GetItemStorage(ItemID item_id)
{
	int storage_num = 0;
	int low_price = 0;
	int high_price = 0;
	
	m_sale_item_index.GetStorageInfo(item_id, &storage_num, &low_price, &high_price);
	return storage_num;
}

void TradeMarketManager::SendSystemNotice(bool is_announce, SaleItemBase* sib)
{
	const ItemBase* item = ITEMPOOL->GetItem(sib->GetSaleItemParamConst().item_wrapper.item_id);
	if (NULL == item)
	{
		return;
	}

	const SaleItemParam& param = sib->GetSaleItemParamConst();

	if (is_announce)
	{
		//发公告
		int price = param.gold_price > 0 ? param.gold_price : param.immortal_coin_price;
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), 
			gamestring::g_trademarket_announce_notice,
			item->GetItemName(), sib->GetSellerUid(), param.sale_index,
			param.put_on_shelve_time, price, item->GetItemId(), 
			param.unique_key, param.put_on_shelve_time);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
		}
	}
	else
	{
		//发公告
		int price = param.gold_price > 0 ? param.gold_price : param.immortal_coin_price;
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
			gamestring::g_trademarket_onsale_notice,
			item->GetItemName(), sib->GetSellerUid(), param.sale_index,
			param.put_on_shelve_time, price, item->GetItemId(),
			param.unique_key, param.put_on_shelve_time);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
		}
	}
}

void TradeMarketManager::GmGetMerchandiseNum(int big_type, bool is_announce)
{
	static int search_type_list[TRADE_ITEM_SEARCH_TYPE_MAX] = { 0 };
	int search_type_num = TradeMarketConfig::Instance().GetSmallSearchTypeList(big_type, search_type_list, TRADE_ITEM_SEARCH_TYPE_MAX);

	for (int i = 0; i < search_type_num; ++i)
	{		
		int num = m_sale_item_index.GetMerchandiseNum(search_type_list[i], is_announce);
		printf("GmGetMerchandiseNum search_type[%d] merchant_num[%d]\n", search_type_list[i], num);
	}
}

void TradeMarketManager::GetHistoryTopPriceItemList(int max_list_size, ARG_OUT int* num, ARG_OUT ItemDataWrapper* item_list)
{
	m_history_top_price.GetTopPriceItemList(max_list_size, num, item_list);
}

void TradeMarketManager::InitHistoryTopPrice(const HistoryTopPriceParam& param)
{
	m_history_top_price.Init(param);
}

void TradeMarketManager::GetHistoryTopPriceInitParam(ARG_OUT HistoryTopPriceParam& param)
{
	m_history_top_price.GetInitParam(param);
}

int TradeMarketManager::GetTradeMinPrice(const ItemBase* item, const ItemDataWrapper* wrapper)
{
	const std::vector<int>& price_vec = item->GetTradeMinPrice(wrapper);
	if (price_vec.empty()) return MAX_INT;

	int server_open_days = World::GetInstWorld()->GetOpenServerDays_0(LocalConfig::Instance().GetUniqueServerID());
	int seq = TradeMarketConfig::Instance().GetTimeFluctuationSeq(server_open_days);
	
	assert(seq >= 0 && !price_vec.empty());
	if (seq < 0 || price_vec.empty())
	{
		return MAX_INT;
	}

	if (seq >= price_vec.size())
	{
		seq = price_vec.size() - 1;
	}

	return price_vec[seq];
}

int TradeMarketManager::GetTradeMaxPrice(const ItemBase* item, const ItemDataWrapper* wrapper)
{
	const std::vector<int>& price_vec = item->GetTradeMaxPrice(wrapper);
	if (price_vec.empty()) return MAX_INT;

	int server_open_days = World::GetInstWorld()->GetOpenServerDays_0(LocalConfig::Instance().GetUniqueServerID());
	int seq = TradeMarketConfig::Instance().GetTimeFluctuationSeq(server_open_days);

	assert(seq >= 0 && seq < price_vec.size());
	if (seq < 0 || seq >= price_vec.size())
	{
		return 0;
	}

	return price_vec[seq];
}

int TradeMarketManager::GetTradeRecommendPrice(const ItemBase* item, const ItemDataWrapper* wrapper)
{
	const std::vector<int>& price_vec = item->GetTradeRecommendPrice(wrapper);
	if (price_vec.empty()) return MAX_INT;

	int server_open_days = World::GetInstWorld()->GetOpenServerDays_0(LocalConfig::Instance().GetUniqueServerID());
	int seq = TradeMarketConfig::Instance().GetTimeFluctuationSeq(server_open_days);

	assert(seq >= 0 && seq < price_vec.size());
	if (seq < 0 || seq >= price_vec.size())
	{
		return 0;
	}

	return price_vec[seq];
}

void TradeMarketManager::QueryItemTradePrice(const ItemBase* item, const ItemDataWrapper* wrapper, ARG_OUT int* min_price, ARG_OUT int* max_price, ARG_OUT int* recommend_price)
{
	if (NULL != min_price) *min_price = GetTradeMinPrice(item, wrapper);
	if (NULL != max_price) *max_price = GetTradeMaxPrice(item, wrapper);
	if (NULL != recommend_price) *recommend_price = GetTradeRecommendPrice(item, wrapper);
}

bool TradeMarketManager::ItemRecover(int role_id, ItemID item_id, ARG_OUT int* recover_num)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return false;

	UserTradeInfo* uti = this->GetUserTradeInfo(role_id);
	if (NULL == uti) return false;

	int num = uti->ItemRecover(role_id, item_id);
	if (num <= 0) return false;

	if (NULL != recover_num) *recover_num = num;

	return true;
}

bool TradeMarketManager::CheckSystemSaleItemPet(NetValueItemParam& param, std::string& error_msg)
{
	char errmsgBuff[200] = { 0 };

	const PetCfg* cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(param.pet_param.pet_id);
	if (NULL == cfg)
	{
		SNPRINTF(errmsgBuff, sizeof(errmsgBuff), "Pet id[%d] Not Found", param.pet_param.pet_id);
		error_msg = errmsgBuff;
		return false;
	}

	// 写入名字
	if (0 == strcmp(param.pet_param.name, ""))
	{
		F_STRNCPY(param.pet_param.name, cfg->name, sizeof(param.pet_param.name));
	}

	// 检查被动技能
	std::set<int> possible_passive_list = LOGIC_CONFIG->GetPetConfig()->GetPossiblePassiveList(cfg->passive_group_id_vec, cfg->passive_group_id_lowest_vec);
	for (int i = 0; i < ARRAY_ITEM_COUNT(param.pet_param.passive_list); ++i)
	{
		int passive_id = param.pet_param.passive_list[i].passive_skill_id;
		if (passive_id > 0)
		{
			if (NULL == SkillPool::Instance().GetPassiveSkillCfg(passive_id))
			{
				SNPRINTF(errmsgBuff, sizeof(errmsgBuff), "Pet Passive id[%d] Not Found", passive_id);
				error_msg = errmsgBuff;
				return false;
			}
		}
	}

	// 检查资质
	{
		if (cfg->quality == PET_QUALITY_GOD)
		{
			// 神兽要满资质
			for (int i = 0; i < ARRAY_ITEM_COUNT(cfg->growth); ++i)
			{
				if (param.pet_param.growth[i] != cfg->growth[i])
				{
					error_msg = "GodPet Growth Must Full";
					return false;
				}
			}
		}
		else
		{
			int attr_dec_max = LOGIC_CONFIG->GetPetConfig()->GetAttrDecNumMax();
			for (int i = 0; i < ARRAY_ITEM_COUNT(cfg->growth); ++i)
			{
				if (param.pet_param.growth[i] < cfg->growth[i] - attr_dec_max || param.pet_param.growth[i] > cfg->growth[i])
				{
					SNPRINTF(errmsgBuff, sizeof(errmsgBuff), "Pet Growth Error i[%d] value[%d]  max[%d] min[%d]",
						i, param.pet_param.growth[i], cfg->growth[i], cfg->growth[i] - attr_dec_max);
					error_msg = errmsgBuff;
					return false;
				}
			}
		}
	}

	return true;
}

bool TradeMarketManager::CheckSystemSaleItemEquip(const Equipment* equip, NetValueItemParam& param, std::string& error_msg)
{
	char errmsgBuff[200] = { 0 };

	param.equip_param.has_random_net_value = 1;
	param.equip_param.is_treasure = 1;
	NameConfig::Instance()->GetRandomName(rand() % SEX_MAX, param.equip_param.creator_name);

	if (param.equip_param.attr_num < 0 || param.equip_param.attr_num > ARRAY_ITEM_COUNT(param.equip_param.attr_list))
	{
		SNPRINTF(errmsgBuff, sizeof(errmsgBuff), "Equip Attr Num Error num[%d] max_num[%d]",
			param.equip_param.attr_num, ARRAY_ITEM_COUNT(param.equip_param.attr_list));
		error_msg = errmsgBuff;
		return false;
	}

	if (param.equip_param.inc_num < 0 || param.equip_param.inc_num > ARRAY_ITEM_COUNT(param.equip_param.attr_extra_inc))
	{
		SNPRINTF(errmsgBuff, sizeof(errmsgBuff), "Equip Extra Attr Num Error num[%d] max_num[%d]",
			param.equip_param.inc_num, ARRAY_ITEM_COUNT(param.equip_param.attr_extra_inc));
		error_msg = errmsgBuff;
		return false;
	}

	if (param.equip_param.sp_id > 0)
	{
		const SpecialEffectCfg* spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(param.equip_param.sp_id);
		if (NULL == spe_cfg)
		{
			SNPRINTF(errmsgBuff, sizeof(errmsgBuff), "SPID Error spid[%d]",
				param.equip_param.sp_id);
			error_msg = errmsgBuff;
			return false;
		}

		int random_group = equip->GetSpecialEffectRandomGroup();
		if (!BattleSpecialEffectPool::GetInstance().CheckSpidMatchGroupid(param.equip_param.sp_id, random_group))
		{
			SNPRINTF(errmsgBuff, sizeof(errmsgBuff), "SPID[%d] NOT MATCH RANDOM GROUP[%d]",
				param.equip_param.sp_id, random_group);
			error_msg = errmsgBuff;
			return false;
		}
	}

	for (int i = 0; i < param.equip_param.attr_num; ++i)
	{
		if (!equip->CheckEquipmentAttrValid(param.equip_param.attr_list[i].attr_type, param.equip_param.attr_list[i].attr_value))
		{
			SNPRINTF(errmsgBuff, sizeof(errmsgBuff), "Equip Attr Error type[%d] value[%d]",
				param.equip_param.attr_list[i].attr_type, param.equip_param.attr_list[i].attr_value);
			error_msg = errmsgBuff;
			return false;
		}
	}

	int extra_num = 0;
	for (int i = 0; i < ARRAY_ITEM_COUNT(param.equip_param.attr_extra_inc); ++i)
	{
		if (1 == param.equip_param.attr_extra_inc[i].un_lock)
		{
			if (!equip->CheckEquipmentAttrValid(param.equip_param.attr_extra_inc[i].attr_type, param.equip_param.attr_extra_inc[i].attr_value))
			{
				SNPRINTF(errmsgBuff, sizeof(errmsgBuff), "Equip Extra Attr Error type[%d] value[%d]",
					param.equip_param.attr_extra_inc[i].attr_type, param.equip_param.attr_extra_inc[i].attr_value);
				error_msg = errmsgBuff;
				return false;
			}
			extra_num += 1;
		}
		else
		{
			break;
		}
	}
	if (extra_num > param.equip_param.inc_num)
	{
		SNPRINTF(errmsgBuff, sizeof(errmsgBuff), "Equip Extra Attr Num Error, extra_num[%d] param.inc_num[%d]",
			extra_num, param.equip_param.inc_num);
		error_msg = errmsgBuff;
		return false;
	}

	return true;
}

bool TradeMarketManager::CheckSystemSaleItemJewelry(const Jewelry* jewelry, NetValueItemParam& param, std::string& error_msg)
{
	char errmsgBuff[200] = { 0 };

	param.jewelry_param.has_random_net_value = 1;
	param.jewelry_param.is_treasure = 1;
	NameConfig::Instance()->GetRandomName(rand() % SEX_MAX, param.jewelry_param.creator_name);

	if (param.jewelry_param.attr_num < 0 || param.jewelry_param.attr_num > ARRAY_ITEM_COUNT(param.jewelry_param.attr_list))
	{
		SNPRINTF(errmsgBuff, sizeof(errmsgBuff), "Jewelry Attr Num Error num[%d] max_num[%d]",
			param.jewelry_param.attr_num, ARRAY_ITEM_COUNT(param.jewelry_param.attr_list));
		error_msg = errmsgBuff;
		return false;
	}

	if (param.jewelry_param.inc_num < 0 || param.jewelry_param.inc_num > ARRAY_ITEM_COUNT(param.jewelry_param.attr_extra_inc))
	{
		SNPRINTF(errmsgBuff, sizeof(errmsgBuff), "Jewelry Extra Inc Num Error[%d] max_num[%d]",
			param.jewelry_param.inc_num, ARRAY_ITEM_COUNT(param.jewelry_param.attr_extra_inc));
		error_msg = errmsgBuff;
		return false;
	}

	for (int i = 0; i < param.jewelry_param.attr_num; ++i)
	{
		if (!jewelry->CheckJewelryAttrValid(param.jewelry_param.attr_list[i].attr_type, param.jewelry_param.attr_list[i].attr_value))
		{
			SNPRINTF(errmsgBuff, sizeof(errmsgBuff), "Jewelry Attr Error type[%d] value[%d]",
				param.jewelry_param.attr_list[i].attr_type, param.jewelry_param.attr_list[i].attr_value);
			error_msg = errmsgBuff;
			return false;
		}
	}

	int extra_num = 0;
	for (int i = 0; i < ARRAY_ITEM_COUNT(param.jewelry_param.attr_extra_inc); ++i)
	{
		if (1 == param.jewelry_param.attr_extra_inc[i].un_lock)
		{
			if (!jewelry->CheckJewelryAttrValid(param.jewelry_param.attr_extra_inc[i].attr_type, param.jewelry_param.attr_extra_inc[i].attr_value))
			{
				SNPRINTF(errmsgBuff, sizeof(errmsgBuff), "Jewelry Extra Attr Error type[%d] value[%d]",
					param.jewelry_param.attr_extra_inc[i].attr_type, param.jewelry_param.attr_extra_inc[i].attr_value);
				error_msg = errmsgBuff;
				return false;
			}
			extra_num += 1;
		}
		else
		{
			break;
		}
	}
	if (extra_num > param.jewelry_param.inc_num)
	{
		SNPRINTF(errmsgBuff, sizeof(errmsgBuff), "Jewelry Extra Num Error, extra_num[%d] param.inc_num[%d]",
			extra_num, param.jewelry_param.inc_num);
		error_msg = errmsgBuff;
		return false;
	}

	return true;
}

bool TradeMarketManager::AddRecycleRule(const RecycleRuleParam& param)
{
	return m_system_recycle.AddRecycleRule(param, false);
}

bool TradeMarketManager::RemoveRecycleRule(long long rule_id)
{
	return m_system_recycle.RemoveRecycleRule(rule_id);
}

void TradeMarketManager::DoRecycle()
{
	UserTradeInfoMap::iterator it = m_user_trade_info_map.begin();
	for (; it != m_user_trade_info_map.end(); ++it)
	{
		UserTradeInfo* uti = it->second;
		for (int sale_idx = 0; sale_idx < USER_SALE_ITEM_MAX_NUM; ++sale_idx)
		{
			const UserSaleItem* usi = uti->GetSaleItem(sale_idx);
			if (NULL == usi || !usi->IsOnSale()) continue;

			long long rule_id = 0;
			static NetValueItemParam param;
			param.Reset();
			param.SetData(usi->GetSaleItemParamConst().item_wrapper.param_data);
			if (!m_system_recycle.IsMatchRule(usi->GetSaleItemParamConst().item_wrapper.item_id,
				usi->GetSaleItemParamConst().gold_price, param, &rule_id))
			{
				continue;
			}

			ItemID item_id = usi->GetSaleItemParamConst().item_wrapper.item_id;
			const ItemBase* item = ITEMPOOL->GetItem(item_id);
			if (NULL == item) continue;
			
			int buy_num = 1;
			int price_type = ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD;
			int single_price = usi->GetSaleItemParamConst().gold_price;

			if (uti->OnItemSold(sale_idx, 1, SYSTEM_TRADE_UID, "SystemBuy", "SystemBuy"))
			{
#ifdef TRACK_ITEM_FLAG
				gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG TradeMarketManager::DoRecycle seller[%d,%s] item_id[%d] item_unique_id[%lld] unique_key[%lld]",
					usi->GetSellerUid(), usi->GetSellerName(), 
					usi->GetSaleItemParamConst().item_wrapper.item_id,
					usi->GetSaleItemParamConst().item_wrapper.item_unique_id,
					usi->GetSaleItemParamConst().unique_key);
#endif

				m_system_recycle.OnRecycle(rule_id);

				int param_length = NetValueItemParam::GetDataLengthByItemType(item->GetItemType());
				m_transaction_record.AddTransactionRecord(item_id, buy_num, price_type, single_price, param_length, usi->GetSaleItemParamConst().item_wrapper.param_data);
				uti->AddSoldRecord(item_id, buy_num, price_type, single_price, usi->GetSaleItemParamConst().item_wrapper);
			
				this->SendShelveChangeNotice(uti->GetRoleID(), sale_idx, Protocol::SCTradeMarketUserShelveData::REASON_NOTIFY_CHANGE);

				EventHandler::Instance().OnTradeMarketSaleItem(uti->GetRoleID(), item_id, false);
			}
		}
	}
}

void TradeMarketManager::OnItemSold(ItemID item_id, int price, int sold_num, const ItemDataWrapper& item_wrapper, int seller_uid, const char* seller_name, const char* buyer_name, int buyer_uid, const char* buyer_pname)
{
	m_history_top_price.OnItemSold(price, item_wrapper);

	this->LogDeal(item_id, price, sold_num, seller_uid, seller_name, buyer_name, buyer_uid, buyer_pname);
}

