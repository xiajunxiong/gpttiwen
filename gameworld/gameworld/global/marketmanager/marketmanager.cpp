#include "marketmanager.hpp"
#include "internalcomm.h"
#include "engineadapter.h"
#include "gamelog.h"
#include <set>

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/localconfig.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"

#include "protocol/msgmarket.hpp"
#include "global/rmibackobjdef.h"
#include "global/cross/crossmanager.h"
#include "global/topicmanager/topicmanager.hpp"
#include "global/colosseum/colosseummanager.hpp"

#include "config/logicconfigmanager.hpp"

#include "item/money.h"
#include "item/knapsack.h"
#include "obj/character/role.h"

#include "other/prestige/prestige.hpp"
#include "other/prestige/prestigeconfig.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/impl/roleactivityfortunemaze.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/flyupsystem/flyupsystem.hpp"
#include "other/event/eventhandler.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "global/datasynccheck/datasynccheck.hpp"

MarketManager & MarketManager::Instance()
{
	static MarketManager market_manager;
	return market_manager;
}

void MarketManager::Update(unsigned long interval, time_t now_second)
{
	if (now_second >= m_next_save_db_timestamp && this->IsDirty())
	{
		this->Save();
		m_next_save_db_timestamp = now_second + MAX_MARKET_DATA_SAVE_DB_TIME;
	}
	//屏蔽重置逻辑
	/*if (0 == next_refresh_timestamp || now_second >= next_refresh_timestamp)
	{
		m_cross_gather_tran_record_list.clear();
		m_disconnect_cross_add_record_list.clear();
		this->SendAllInfo(NULL);
		this->GetNextRefreshTimestamp();
	}*/
}

void MarketManager::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if(old_dayid == now_dayid) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->OnHiddenDayChange();
	}
	else
	{
		this->OnGameDayChange();
	}
}

void MarketManager::OnHiddenDayChange()
{
	for (std::map<ItemID, MarketItemInfo>::iterator it = m_cross_gather_tran_record_list.begin(); it != m_cross_gather_tran_record_list.end(); ++it)
	{
		if(!it->second.IsVaild()) continue;

		it->second.yesterday_price = it->second.cur_price;
		it->second.transaction_num = 0;
		this->SetDirty(it->first);
	}
	//同步变化信息到所有游戏服
	crossgameprotocal::HiddenGameMarketAllTransactionRecordSynReq hgmatrsr;
	hgmatrsr.op_type = crossgameprotocal::HiddenGameMarketAllTransactionRecordSynReq::OP_TYPE_HIDDEN_RECV_GAME_INFO_SYN;
	hgmatrsr.count = 0;
	for (std::map<ItemID, MarketItemInfo>::const_iterator it = m_cross_gather_tran_record_list.begin(); it != m_cross_gather_tran_record_list.end() &&
		hgmatrsr.count < ARRAY_ITEM_COUNT(hgmatrsr.item_list); ++it)
	{
		if (it->second.IsVaild())
		{
			hgmatrsr.item_list[hgmatrsr.count++] = it->second;
		}
	}
	InternalComm::Instance().SendSerializeMsgToGameThroughCross(&hgmatrsr);
	this->SendAllInfo(NULL);
}

void MarketManager::OnGameDayChange()
{
	m_disconnect_cross_add_record_list.clear();	//跨天则该部分丢失

	//不同步跨服,等跨服同步回来.默认重置,防止跨服挂了或没连接上跨服
	for (std::map<ItemID, MarketItemInfo>::iterator it = m_cross_gather_tran_record_list.begin(); it != m_cross_gather_tran_record_list.end(); ++it)
	{
		if (!it->second.IsVaild()) continue;

		it->second.yesterday_price = it->second.cur_price;
		it->second.transaction_num = 0;
		this->SetDirty(it->first);
	}
	this->SendAllInfo(NULL);
}

void MarketManager::OnServerStart()
{
	this->LoadMarketAllItemData(0);
}

void MarketManager::OnServerStop()
{
	this->Save();
}

bool MarketManager::LoadMarketAllItemData(long long id_from)
{
	RMIInitMarketItemDataBackObjectImpl * impl = new RMIInitMarketItemDataBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitMarketItemDataAsyn(id_from, impl);
}

void RMIInitMarketItemDataBackObjectImpl::InitMarketItemDataRet(int ret, const MarketManagerParam& list_param)
{
	if (ret < 0)
	{
		printf("%s Failed \n", __FUNCTION__);
	}
	else
	{
		MarketManager & market_manager = MarketManager::Instance();

		market_manager.Init(list_param);
		market_manager.LoadMarketAllItemDataSucc();
		printf("%s:%d\n", __FUNCTION__, ret);
	}
}

void MarketManager::Init(const MarketManagerParam & param)
{
	for (int i = 0; i < param.count && i < ARRAY_LENGTH(param.market_list); ++i)
	{
		const MarketItemInfo & market_item_info = param.market_list[i].market_info;
		if (!market_item_info.IsVaild())
		{
			gamelog::g_log_market.printf(LL_CRITICAL, "[%s] item_id[%d]", __FUNCTION__, market_item_info.item_id);
			continue;
		}
		m_cross_gather_tran_record_list[market_item_info.item_id] = market_item_info;
		m_dirty_mark[market_item_info.item_id] = false;
		m_old_state[market_item_info.item_id] = true;
	}
	m_load_flag = true;
}

void MarketManager::GetInitParam(MarketManagerParam * param)
{
	if (!m_load_flag || !this->IsDirty())
	{
		return;
	}

	param->count = 0;
	for (std::map<ItemID, MarketItemInfo>::iterator it = m_cross_gather_tran_record_list.begin(); it != m_cross_gather_tran_record_list.end(); ++it)
	{
		if (m_dirty_mark[it->first])
		{
			if (!it->second.IsVaild())
			{
				if (m_old_state[it->first]) continue;

				param->market_list[param->count].change_state = structcommon::CS_DELETE;
				param->market_list[param->count].market_info.item_id = it->first;
				param->count++;
			}
			else
			{
				param->market_list[param->count].market_info = it->second;
				param->market_list[param->count].change_state = m_old_state[it->first] ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
				param->count++;
			}
		}
	}
}

void MarketManager::ClearDirtyMark(MarketManagerParam * param)
{
	if(!this->IsDirty()) return;

	for (int i = 0; i < param->count && i < ARRAY_LENGTH(param->market_list); i++)
	{
		MarketManagerParam::MarketItemSaveEntry & data = param->market_list[i];
		if (structcommon::CS_DELETE != data.change_state)
		{
			m_old_state[data.market_info.item_id] = true;
			m_dirty_mark[data.market_info.item_id] = false;
		}
		else
		{
			m_dirty_mark.erase(data.market_info.item_id);
			m_old_state.erase(data.market_info.item_id);
			m_cross_gather_tran_record_list.erase(data.market_info.item_id);
		}
		data.change_state = structcommon::CS_NONE;
	}

	m_change_flag = false;
}

void MarketManager::OnServerLoadDataFinish()
{
	this->CheckAllItemData();
}

void MarketManager::OnReloadConfig()
{	
	this->CheckAllItemData();
}

bool MarketManager::OnBuyItem(Role * role, int item_id, int num)
{
	if(NULL == role || num <= 0) return false;

	const MarketOtherCfg & market_other_cfg = LOGIC_CONFIG->GetShopConfig()->GetMarketOtherCfg();
	const MarketItemCfg * market_item_cfg = LOGIC_CONFIG->GetShopConfig()->GetMarketItemCfg(item_id);
	if (NULL == market_item_cfg)
	{
		return false;
	}
	if (CONVERT_REQUIRE_TYPE_ROLE_LEVEL == market_item_cfg->require_type)
	{
		if (role->GetLevel() < market_item_cfg->require_value)
		{
			role->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
			return false;
		}
	}
	else if (CONVERT_REQUIRE_TYPE_VILLAGE_PRESTIGE == market_item_cfg->require_type)
	{
		int village_id = market_item_cfg->require_value;
		const PrestigeVillageCfg* village_cfg = LOGIC_CONFIG->GetPrestigeConfig()->GetVillageCfg(village_id);
		if (NULL == village_cfg)
		{
			role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return false;
		}

		static const int PRESTIGE_REVERENCE = 3; // 声望-崇敬
		if (!role->GetRoleModuleManager()->GetPrestige()->CheckPrestige(village_id, PRESTIGE_REVERENCE))
		{
			role->NoticeNum(errornum::EN_PRESTIGE_NOT_ENOUGH);
			return false;
		}
	}
	else if (CONVERT_REQUIRE_TYPE_COLOSSEUM == market_item_cfg->require_type)
	{
		int rank = 0;
		bool is_in_pool = false;
		ColosseumManager::Instance().GetUserGladiator(role->GetUserId(), &rank, &is_in_pool);

		if (!is_in_pool && rank >= market_item_cfg->require_value)
		{
			role->NoticeNum(errornum::EN_ITEM_REQUIRE_COLOSSEUM);
			return false;
		}
	}
	else if (CONVERT_REQUIRE_TYPE_CLOUD_ARENA_SEASON_HIGHEST_DAN_ID == market_item_cfg->require_type)
	{
		int dan_id = role->GetRoleModuleManager()->GetCommonData().cloud_arena_season_highest_dan_id;
		if (dan_id < market_item_cfg->require_value)
		{
			role->NoticeNum(errornum::EN_ITEM_REQUIRE_CLOUD_ARENA_DAN);
			return false;
		}
	}
	else if (CONVERT_REQUIRE_TYPE_CLOUD_ARENA_SEASON_HIGHEST_RANK == market_item_cfg->require_type)
	{
		int rank = role->GetRoleModuleManager()->GetCommonData().cloud_arena_season_highest_rank;
		if (rank >= market_item_cfg->require_value) // 因为服务端rank是从0开始，配置从1开始，所以用>=
		{
			role->NoticeNum(errornum::EN_ITEM_REQUIRE_CLOUD_ARENA_RANK);
			return false;
		}
	}

	if (market_item_cfg->feisheng_count > role->GetRoleModuleManager()->GetFlyUpSystem()->GetFlyUpFlag())
	{
		role->NoticeNum(errornum::EN_SHOP_BUY_ITEM_FAILED_BY_SOAR_COUNT_NOT_ENOUGH);
		return false;
	}

	if (!market_item_cfg->is_show)
	{
		role->NoticeNum(errornum::EN_TRADE_MARKET_ITEM_CAN_NOT_BUY);
		return false;
	}

	int price = market_item_cfg->price;
	std::map<ItemID, MarketItemInfo>::iterator it = m_cross_gather_tran_record_list.find(item_id);
	if (it != m_cross_gather_tran_record_list.end())
	{
		price = it->second.cur_price;
	}

	long long price_count = static_cast<long long >(1LL * price * num);
	if(price_count <= 0) return false;

	ItemConfigData buy_item(market_item_cfg->item.item_id, market_item_cfg->item.is_bind, market_item_cfg->item.num * num);
	role->GetRoleModuleManager()->GetKnapsack()->OnCheckNorexConvertItem(buy_item);

	if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPut2(buy_item, false, false))
	{
		role->GetRoleModuleManager()->GetKnapsack()->SendErrorNotice();
		return false;
	}

	switch (market_item_cfg->price_type)
	{
		case MONEY_TYPE_GOLD:
		{
			if (!role->GetRoleModuleManager()->GetMoney()->UseGold(price_count, __FUNCTION__))
			{
				role->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return false;
			}
		}
		break;
		case MONEY_TYPE_IMMORTAL_COIN:
		{
			if (!role->GetRoleModuleManager()->GetMoney()->UseImmortalCoin(price_count, __FUNCTION__))
			{
				role->NoticeNum(errornum::EN_IMMORTAL_COIN_NOT_ENOUGH);
				return false;
			}
		}
		break;
		case MONEY_TYPE_COIN_BIND:
		{
			if (!role->GetRoleModuleManager()->GetMoney()->UseCoinBind(price_count, __FUNCTION__))
			{
				role->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return false;
			}
		}
		break;
		default:
		if (!role->GetRoleModuleManager()->GetMoney()->UseOtherCoin(price_count, market_item_cfg->price_type, __FUNCTION__))
		{
			role->NoticeNum(errornum::EN_OTHER_MONEY_NOT_ENOUGH);
			return false;
		}
		break;
	}
	
	//已经消耗了货币,打个log
	ROLE_LOG_QUICK10(LOG_TYPE_MARKET, role, buy_item.item_id, buy_item.num, __FUNCTION__, NULL, market_item_cfg->price_type, price_count, 
		market_item_cfg->conver_type, num, 1);
	
	if (!role->GetRoleModuleManager()->GetKnapsack()->Put(buy_item, PUT_REASON_MARKET))
	{
		role->GetRoleModuleManager()->GetKnapsack()->SendMail(&buy_item, 1, SNED_MAIL_TYPE_DEFAULT, true);
	}

	EventHandler::Instance().OnMarketBuyItem(role, item_id, num, price_count, market_item_cfg->price_type);
	const ItemBase* itembase = ITEMPOOL->GetItem(buy_item.item_id);
	if (NULL != itembase)
	{
		if (itembase->GetItemSubType() == ItemNamespace::ITEM_SUB_TYPE_GEM)
		{
			RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
			if (NULL != fortune_maze)
			{
				fortune_maze->BuyStone(num);
			}
		}
	}
	if (it != m_cross_gather_tran_record_list.end())
	{
		it->second.transaction_num += num;
		it->second.transaction_count += num;
	}
	else
	{
		MarketItemInfo market_info;
		market_info.item_id = item_id;
		market_info.yesterday_price = market_item_cfg->price;
		market_info.transaction_num += num;
		market_info.transaction_count += num;
		market_info.version = market_item_cfg->version;
		m_cross_gather_tran_record_list[item_id] = market_info;
		m_old_state[item_id] = false;
	}


	MarketItemInfo & market_info = m_cross_gather_tran_record_list[item_id];
	this->SetNewPrice(market_item_cfg, market_other_cfg, market_info);
	this->SetDirty(item_id);

	this->SendSingleInfo(role, market_info);
	gamelog::g_log_market.printf(LL_INFO, "%s succ ! role[%d, %s] buy_item[%d,%d,%d] price[%d,%lld] ", __FUNCTION__, role->GetUID(), role->GetName(),
		buy_item.item_id, buy_item.num, buy_item.is_bind, market_item_cfg->price_type, price_count);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		//同步到所有连接上隐藏服的游戏服
		this->OnHiddenGameBuyItemRecordSyn(market_info, LocalConfig::Instance().GetMergeServerId(), market_info.cur_price != price);
	}
	else
	{
		if (CrossManager::Instance().IsCrossServerConnected())
		{
			//同步该交易记录到跨服,然后再由跨服转发至其他服务器(不包括玩家所在服)
			this->OnGameHiddenBuyItemRecordSyn(TransactionRecordInfo(item_id, num));
		}
		else
		{
			m_disconnect_cross_add_record_list[item_id] += num;
		}
	}

	return true;
}

void MarketManager::OnMarketItemPurchased(int item_id, int num)
{
	const MarketItemCfg * market_item_cfg = LOGIC_CONFIG->GetShopConfig()->GetMarketItemCfg(item_id);
	if (NULL == market_item_cfg)
	{
		return;
	}
	const MarketOtherCfg & market_other_cfg = LOGIC_CONFIG->GetShopConfig()->GetMarketOtherCfg();
	int price = market_item_cfg->price;
	std::map<ItemID, MarketItemInfo>::iterator it = m_cross_gather_tran_record_list.find(item_id);
	if (it != m_cross_gather_tran_record_list.end())
	{
		price = it->second.cur_price;
		it->second.transaction_num += num;
		it->second.transaction_count += num;
	}
	else
	{
		MarketItemInfo market_info;
		market_info.item_id = item_id;
		market_info.yesterday_price = market_item_cfg->price;
		market_info.transaction_num += num;
		market_info.transaction_count += num;
		market_info.version = market_item_cfg->version;
		m_cross_gather_tran_record_list[item_id] = market_info;
		m_old_state[item_id] = false;
	}

	MarketItemInfo & market_info = m_cross_gather_tran_record_list[item_id];
	this->SetNewPrice(market_item_cfg, market_other_cfg, market_info);
	this->SetDirty(item_id);


//	this->SendSingleInfo(NULL, item_id);
	gamelog::g_log_market.printf(LL_INFO, "OnMarketItemPurchased item_id[%d] num[%d]", item_id, num);
	
	if (CrossConfig::Instance().IsHiddenServer())
	{
		//同步到所有连接上隐藏服的游戏服
		this->OnHiddenGameBuyItemRecordSyn(market_info, LocalConfig::Instance().GetMergeServerId(), market_info.cur_price != price);
	}
	else
	{
		if (CrossManager::Instance().IsCrossServerConnected())
		{
			//同步该交易记录到跨服,然后再由跨服转发至其他服务器(不包括玩家所在服)
			this->OnGameHiddenBuyItemRecordSyn(TransactionRecordInfo(item_id, num));
		}
		else
		{
			m_disconnect_cross_add_record_list[item_id] += num;
		}
	}
}

void MarketManager::OnSaleItem(Role * role, int column, int index, int num)
{
	if (NULL == role || num <= 0) return;

	const ItemGridData * item_grid_data = role->GetRoleModuleManager()->GetKnapsack()->GetItemGridData2(column, index);
	if (NULL == item_grid_data || item_grid_data->num < num)
	{
		return;
	}

	const MarketOtherCfg & market_other_cfg = LOGIC_CONFIG->GetShopConfig()->GetMarketOtherCfg();
	const MarketItemCfg * market_item_cfg = LOGIC_CONFIG->GetShopConfig()->GetMarketItemCfg(item_grid_data->item_id);
	if (NULL == market_item_cfg)
	{
		return;
	}

	int price = market_item_cfg->price;
	std::map<ItemID, MarketItemInfo>::iterator it = m_cross_gather_tran_record_list.find(item_grid_data->item_id);
	if (it != m_cross_gather_tran_record_list.end())
	{
		price = it->second.cur_price;
	}
	//此处价格是购买价格需要再衰减一下
	price = (int)((1LL * market_other_cfg.sell_price_scale * price) / MarketItemCfg::BASE_NUM);
	if (price > market_item_cfg->max_price)
	{
		price = market_item_cfg->max_price;
	}

	long long price_count = static_cast<long long>(1LL * price * num);
	if (price_count <= 0) return;

	ItemID item_id = item_grid_data->item_id;
	if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItemByIndex(column, index, num, __FUNCTION__))
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	switch (market_item_cfg->price_type)
	{
		case MONEY_TYPE_GOLD:
		{
			role->GetRoleModuleManager()->GetMoney()->AddGold(price_count, __FUNCTION__);
		}
		break;
		case MONEY_TYPE_IMMORTAL_COIN:
		{
			role->GetRoleModuleManager()->GetMoney()->AddImmortalCoin(price_count, __FUNCTION__);
		}
		break;
		case MONEY_TYPE_COIN_BIND:
		{
			role->GetRoleModuleManager()->GetMoney()->AddCoinBind(price_count, __FUNCTION__);
		}
		break;
	default:
		role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(price_count, market_item_cfg->price_type, __FUNCTION__);
		break;
	}

	int old_price = market_item_cfg->price;
	if (it != m_cross_gather_tran_record_list.end())
	{
		old_price = it->second.cur_price;
		it->second.transaction_num -= num;
		it->second.transaction_count -= num;
	}
	else
	{
		MarketItemInfo market_info;
		market_info.item_id = item_id;
		market_info.yesterday_price = market_item_cfg->price;
		market_info.transaction_num -= num;
		market_info.transaction_count -= num;
		market_info.version = market_item_cfg->version;
		m_cross_gather_tran_record_list[item_id] = market_info;
		m_old_state[item_id] = false;
	}

	MarketItemInfo & market_info = m_cross_gather_tran_record_list[item_id];
	this->SetNewPrice(market_item_cfg, market_other_cfg, market_info);
	this->SetDirty(item_id);

	this->SendSingleInfo(role, market_info);
	gamelog::g_log_market.printf(LL_INFO, "%s succ ! role[%d, %s] sale_item[%d,%d] price[%d,%lld] ", __FUNCTION__, role->GetUID(), role->GetName(),
		item_id, num, market_item_cfg->price_type, price_count);
	if (CrossConfig::Instance().IsHiddenServer())
	{
		//同步到所有连接上隐藏服的游戏服
		this->OnHiddenGameBuyItemRecordSyn(market_info, LocalConfig::Instance().GetMergeServerId(), market_info.cur_price != old_price);
	}
	else
	{
		if (CrossManager::Instance().IsCrossServerConnected())
		{
			//同步该交易记录到跨服,然后再由跨服转发至其他服务器(不包括玩家所在服)
			this->OnGameHiddenBuyItemRecordSyn(TransactionRecordInfo(item_id, -num));
		}
		else
		{
			m_disconnect_cross_add_record_list[item_id] -= num;
		}
	}

}

bool MarketManager::GetItemPrice(int item_id, int num, ARG_OUT int* price_type, ARG_OUT int* total_price)
{
	const MarketItemCfg * market_item_cfg = LOGIC_CONFIG->GetShopConfig()->GetMarketItemCfg(item_id);
	if (NULL == market_item_cfg)
	{
		return false;
	}

	int cur_price = market_item_cfg->price;
	std::map<ItemID, MarketItemInfo>::iterator it = m_cross_gather_tran_record_list.find(item_id);
	if (it != m_cross_gather_tran_record_list.end())
	{
		cur_price = it->second.cur_price;
	}

	if (NULL != price_type) *price_type = market_item_cfg->price_type;
	if (NULL != total_price) *total_price = cur_price * num;		//此处获取的是购买价格,而不是售卖

	return true;
}

int MarketManager::GetItemPriceFloat(int business_amount, int float_range, const MarketOtherCfg & market_other_cfg, int transaction_count, bool is_sale)
{
	int record_price_range = (transaction_count / business_amount) * float_range;	//满足除才算一份
	if (record_price_range > 0)
	{
		if (record_price_range > market_other_cfg.float_limit_max)
		{
			record_price_range = market_other_cfg.float_limit_max;
		}
	}
	else
	{
		if (record_price_range * -1 > market_other_cfg.float_limit_min)
		{
			record_price_range = market_other_cfg.float_limit_min * -1;
		}
	}
	if (is_sale)
	{
		return market_other_cfg.sell_price_scale + record_price_range;
	}

	return MarketItemCfg::BASE_NUM + record_price_range;
}

void MarketManager::OnSaleFuShi(Role * role, int index)
{
	if (NULL == role) return;

	const ItemGridData * item_grid_data = role->GetRoleModuleManager()->GetHolyEquipManager()->GetFuShiGridDataKnapsack(index);
	if (NULL == item_grid_data) return;

	const MarketOtherCfg & market_other_cfg = LOGIC_CONFIG->GetShopConfig()->GetMarketOtherCfg();
	const MarketItemCfg * market_item_cfg = LOGIC_CONFIG->GetShopConfig()->GetMarketItemCfg(item_grid_data->item_id);
	if (NULL == market_item_cfg)
	{
		return;
	}

	int price = market_item_cfg->price;
	std::map<ItemID, MarketItemInfo>::iterator it = m_cross_gather_tran_record_list.find(item_grid_data->item_id);
	if (it != m_cross_gather_tran_record_list.end())
	{
		price = it->second.cur_price;
	}
	//此处价格是购买价格需要再衰减一下
	price = (int)((1LL * market_other_cfg.sell_price_scale * price) / MarketItemCfg::BASE_NUM);
	if (price > market_item_cfg->max_price)
	{
		price = market_item_cfg->max_price;
	}

	long long price_count = static_cast<long long>(1LL * price * 1);
	if (price_count <= 0) return;

	ItemID item_id = item_grid_data->item_id;
	if (!role->GetRoleModuleManager()->GetHolyEquipManager()->ConsumeFuShiGrid(index, HOLY_EQUIP_SEND_REASON_SALE_FU_SHI, __FUNCTION__))
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	switch (market_item_cfg->price_type)
	{
	case MONEY_TYPE_GOLD:
	{
		role->GetRoleModuleManager()->GetMoney()->AddGold(price_count, __FUNCTION__);
	}
	break;
	case MONEY_TYPE_IMMORTAL_COIN:
	{
		role->GetRoleModuleManager()->GetMoney()->AddImmortalCoin(price_count, __FUNCTION__);
	}
	break;
	case MONEY_TYPE_COIN_BIND:
	{
		role->GetRoleModuleManager()->GetMoney()->AddCoinBind(price_count, __FUNCTION__);
	}
	break;
	default:
		role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(price_count, market_item_cfg->price_type, __FUNCTION__);
		break;
	}

	int old_price = market_item_cfg->price;
	if (it != m_cross_gather_tran_record_list.end())
	{
		old_price = it->second.cur_price;
		it->second.transaction_num -= 1;
		it->second.transaction_count -= 1;
	}
	else
	{
		MarketItemInfo market_info;
		market_info.item_id = item_id;
		market_info.yesterday_price = market_item_cfg->price;
		market_info.transaction_num -= 1;
		market_info.transaction_count -= 1;
		market_info.version = market_item_cfg->version;
		m_cross_gather_tran_record_list[item_id] = market_info;
		m_old_state[item_id] = false;
	}

	MarketItemInfo & market_info = m_cross_gather_tran_record_list[item_id];
	this->SetNewPrice(market_item_cfg, market_other_cfg, market_info);
	this->SetDirty(item_id);

	this->SendSingleInfo(role, market_info);
	gamelog::g_log_market.printf(LL_INFO, "%s succ ! role[%d, %s] sale_item[%d,%d] price[%d,%lld] ", __FUNCTION__, role->GetUID(), role->GetName(),
		item_id, 1, market_item_cfg->price_type, price_count);
	if (CrossConfig::Instance().IsHiddenServer())
	{
		//同步到所有连接上隐藏服的游戏服
		this->OnHiddenGameBuyItemRecordSyn(market_info, LocalConfig::Instance().GetMergeServerId(), market_info.cur_price != old_price);
	}
	else
	{
		if (CrossManager::Instance().IsCrossServerConnected())
		{
			//同步该交易记录到跨服,然后再由跨服转发至其他服务器(不包括玩家所在服)
			this->OnGameHiddenBuyItemRecordSyn(TransactionRecordInfo(item_id, -1));
		}
		else
		{
			m_disconnect_cross_add_record_list[item_id] -= 1;
		}
	}
}

void MarketManager::OnConnectCrossSucc()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameMarketAllTransactionRecordSynReq hgmatrsr;
		hgmatrsr.op_type = crossgameprotocal::HiddenGameMarketAllTransactionRecordSynReq::OP_TYPE_HIDDEN_CONNECT_CROSS;
		hgmatrsr.count = 0;

		InternalComm::Instance().SendSerializeMsgToGameThroughCross(&hgmatrsr);
	}
	else
	{
		this->OnGameHiddenAllRecordSyn();
	}
}

void MarketManager::OnDisconnectFromCrossServer()
{
	if (CrossConfig::Instance().IsHiddenServer()) return;		//因为断开连接后,开跨服不会说单开,而是直接隐藏服重开
}

void MarketManager::OnGameHiddenBuyItemRecordSyn(TransactionRecordInfo record)
{
	crossgameprotocal::GameHiddenMarketBuyItemRecordSynReq ghmbirsr;
	ghmbirsr.server_id = LocalConfig::Instance().GetMergeServerId();
	ghmbirsr.tran_record = record;

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghmbirsr, sizeof(ghmbirsr));
}

void MarketManager::OnHiddenReceGameBuyItemRecord(crossgameprotocal::GameHiddenMarketBuyItemRecordSynReq * msg)
{
	if(NULL == msg) return;

	const MarketItemCfg * market_item_cfg = LOGIC_CONFIG->GetShopConfig()->GetMarketItemCfg(msg->tran_record.item_id);
	if (NULL == market_item_cfg)
	{
		return;
	}
	const MarketOtherCfg & market_other_cfg = LOGIC_CONFIG->GetShopConfig()->GetMarketOtherCfg();
	int price = market_item_cfg->price;
	std::map<ItemID, MarketItemInfo>::iterator it = m_cross_gather_tran_record_list.find(msg->tran_record.item_id);
	if (it != m_cross_gather_tran_record_list.end())
	{
		price = it->second.cur_price;
		it->second.transaction_num += msg->tran_record.transaction_num;
		it->second.transaction_count += msg->tran_record.transaction_num;
	}
	else
	{
		MarketItemInfo market_info;
		market_info.item_id = msg->tran_record.item_id;
		market_info.yesterday_price = market_item_cfg->price;
		market_info.transaction_num += msg->tran_record.transaction_num;
		market_info.transaction_count += msg->tran_record.transaction_num;
		market_info.version = market_item_cfg->version;
		m_cross_gather_tran_record_list[market_info.item_id] = market_info;
		m_old_state[market_info.item_id] = false;
	}
	MarketItemInfo & market_info = m_cross_gather_tran_record_list[msg->tran_record.item_id];
	this->SetNewPrice(market_item_cfg, market_other_cfg, market_info);
	this->SetDirty(market_info.item_id);

	this->OnHiddenGameBuyItemRecordSyn(market_info, msg->server_id, market_info.cur_price != price);
	if (market_info.cur_price != price)
	{
		this->SendSingleInfo(NULL, market_info);
	}
}

void MarketManager::OnHiddenGameBuyItemRecordSyn(const MarketItemInfo & item_info, int server_id, bool is_has_change_price)
{
	crossgameprotocal::HiddenGameMarketBuyItemRecordSynReq hgmbirsr;
	hgmbirsr.server_id = server_id;
	hgmbirsr.is_has_change_price = is_has_change_price ? 1 : 0;
	hgmbirsr.item_info = item_info;

	InternalComm::Instance().SendToAllGameThroughCross(&hgmbirsr, sizeof(hgmbirsr));
}

void MarketManager::OnGameRecvHiddenBuyItemRecord(crossgameprotocal::HiddenGameMarketBuyItemRecordSynReq * msg)
{
	if(NULL == msg || msg->server_id == LocalConfig::Instance().GetMergeServerId()) return;		//本服同步的已经先加过了.不能重复

	m_cross_gather_tran_record_list[msg->item_info.item_id] = msg->item_info;
	if (0 != msg->is_has_change_price)
	{
		this->SendSingleInfo(NULL, msg->item_info);
	}
}

void MarketManager::OnGameHiddenAllRecordSyn()
{
	crossgameprotocal::GameHiddenMarketAllTransactionRecordSynReq ghmatrsr;
	ghmatrsr.usid = LocalConfig::Instance().GetUniqueServerID();
	ghmatrsr.add_count = 0;
	for (std::map<ItemID, int>::const_iterator it = m_disconnect_cross_add_record_list.begin(); it != m_disconnect_cross_add_record_list.end() && 
		ghmatrsr.add_count < MAX_MARKET_ITEM_NUM; ++it)
	{
		if (0 != it->second)
		{
			ghmatrsr.disc_cross_add_record_list[ghmatrsr.add_count++] = TransactionRecordInfo(it->first, it->second);
		}
	}

	InternalComm::Instance().SendSerializeMsgToHiddenThroughCross(&ghmatrsr);

	DataSyncCheck::Instance().AddCheck(crossgameprotocal::DATA_SYNC_CHECK_TYPE_MARKET);
}

void MarketManager::OnGameRecvHiddenAllRecord(crossgameprotocal::HiddenGameMarketAllTransactionRecordSynReq & msg)
{
	std::set<ItemID> item_list;
	for (int i = 0; i < msg.count && i < MAX_MARKET_ITEM_NUM; i++)
	{
		m_cross_gather_tran_record_list[msg.item_list[i].item_id] = msg.item_list[i];
		this->SetDirty(msg.item_list[i].item_id);
		item_list.insert(msg.item_list[i].item_id);
	}

	for (std::map<ItemID, MarketItemInfo>::iterator it = m_cross_gather_tran_record_list.begin(); it != m_cross_gather_tran_record_list.end(); ++it)
	{
		if (item_list.end() == item_list.find(it->first))		//没找到的则默认删除
		{
			m_dirty_mark[it->first] = true;
			m_old_state[it->first] = false;
			it->second.Reset();
			m_change_flag = true;
		}
	}
	if (this->IsDirty())
	{
		this->Save();
		m_next_save_db_timestamp = (unsigned int)(EngineAdapter::Instance().Time() + MAX_MARKET_DATA_SAVE_DB_TIME);
	}

	m_disconnect_cross_add_record_list.clear();
	this->SendAllInfo(NULL);
}

void MarketManager::OnHiddenRecvGameAllRecord(crossgameprotocal::GameHiddenMarketAllTransactionRecordSynReq & msg)
{
	const MarketOtherCfg & market_other_cfg = LOGIC_CONFIG->GetShopConfig()->GetMarketOtherCfg();
	for (int i = 0; i < msg.add_count && i < MAX_MARKET_ITEM_NUM; i++)
	{
		TransactionRecordInfo & info = msg.disc_cross_add_record_list[i];
		const MarketItemCfg * market_item_cfg = LOGIC_CONFIG->GetShopConfig()->GetMarketItemCfg(info.item_id);
		if(NULL == market_item_cfg) continue;

		std::map<ItemID, MarketItemInfo>::iterator it = m_cross_gather_tran_record_list.find(info.item_id);
		if (it != m_cross_gather_tran_record_list.end())
		{
			it->second.transaction_num += info.transaction_num;
			it->second.transaction_count += info.transaction_num;
		}
		else
		{
			MarketItemInfo market_info;
			market_info.item_id = info.item_id;
			market_info.yesterday_price = market_item_cfg->price;
			market_info.transaction_num += info.transaction_num;
			market_info.transaction_count += info.transaction_num;
			market_info.version = market_item_cfg->version;
			m_cross_gather_tran_record_list[market_info.item_id] = market_info;
			m_old_state[market_info.item_id] = false;
		}
		MarketItemInfo & market_info = m_cross_gather_tran_record_list[info.item_id];
		this->SetNewPrice(market_item_cfg, market_other_cfg, market_info);
		this->SetDirty(market_info.item_id);
	}

	crossgameprotocal::HiddenGameMarketAllTransactionRecordSynReq hgmatrsr;
	hgmatrsr.op_type = crossgameprotocal::HiddenGameMarketAllTransactionRecordSynReq::OP_TYPE_HIDDEN_RECV_GAME_INFO_SYN;
	hgmatrsr.count = 0;
	for (std::map<ItemID, MarketItemInfo>::const_iterator it = m_cross_gather_tran_record_list.begin(); it != m_cross_gather_tran_record_list.end() && 
		hgmatrsr.count < MAX_MARKET_ITEM_NUM; ++it)
	{
		if (it->second.IsVaild())
		{
			hgmatrsr.item_list[hgmatrsr.count++] = it->second;
		}
	}
	InternalComm::Instance().SendSerializeMsgToGameThroughCross(&hgmatrsr);
	this->SendAllInfo(NULL);
}

void MarketManager::SendAllInfo(Role * role)
{
	Protocol::SCMarketAllInfo info;
	info.count = 0;
	for (std::map<ItemID, MarketItemInfo>::const_iterator it = m_cross_gather_tran_record_list.begin(); it != m_cross_gather_tran_record_list.end() && info.count < MAX_MARKET_ITEM_NUM; ++it)
	{
		if (it->second.IsVaild())
		{
			info.item_list[info.count++] = it->second;
		}
	}

	//缩减下发长度
	int length = sizeof(info) - (MAX_MARKET_ITEM_NUM - info.count) * sizeof(MarketItemSimpleInfo);
	if (NULL != role)
	{
		role->GetRoleModuleManager()->NetSend(&info, length);
	}
	else
	{
		TopicManager::Instance()->OnTopicUpdate(TOPIC_TYPE_MARKET, (const char*)&info, length);
	}
}

void MarketManager::SendSingleInfo(Role * role, MarketItemInfo & item_info)
{
	Protocol::SCMarketSingleInfo info;
	info.item_info = item_info;

	if (NULL != role)
	{
		role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
	}
	else
	{
		TopicManager::Instance()->OnTopicUpdate(TOPIC_TYPE_MARKET, (const char*)&info, sizeof(info));
	}
}

MarketManager::MarketManager() : next_refresh_timestamp(0), m_change_flag(false), m_load_flag(false), m_next_save_db_timestamp(0)
{
	m_cross_gather_tran_record_list.clear();
	m_disconnect_cross_add_record_list.clear();
	m_dirty_mark.clear();
	m_old_state.clear();
}

MarketManager::~MarketManager()
{
}

void MarketManager::GetNextRefreshTimestamp()
{
	const tm *local_time = EngineAdapter::Instance().LocalTime();
	const MarketOtherCfg & market_other_cfg = LOGIC_CONFIG->GetShopConfig()->GetMarketOtherCfg();
	int hour = 0;
	for (int i = 1; i * market_other_cfg.float_time_reset <= 24; i++)
	{
		hour = i * market_other_cfg.float_time_reset;
		if (local_time->tm_hour < hour)
		{
			break;
		}
	}
	if (hour >= 24)
	{
		hour = 0;
	}

	next_refresh_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().NextDayInterval(hour, 0, 0) + EngineAdapter::Instance().Time());
}

void MarketManager::SetNewPrice(const MarketItemCfg * market_item_cfg, const MarketOtherCfg & market_other_cfg, MarketItemInfo & market_info)
{
	if(NULL == market_item_cfg) return;

	int price_range = MarketItemCfg::BASE_NUM;
	price_range = this->GetItemPriceFloat(market_item_cfg->business_amount, market_item_cfg->float_range, market_other_cfg, market_info.transaction_num, false);

	int cur_price = static_cast<int>(1LL * market_info.yesterday_price * price_range / MarketItemCfg::BASE_NUM);	//需要用昨天价格来计算,该浮动是当天的
	if (cur_price < market_item_cfg->min_price)
	{
		cur_price = market_item_cfg->min_price;
	}
	else if (cur_price > market_item_cfg->max_price)
	{
		cur_price = market_item_cfg->max_price;
	}
	market_info.cur_price = cur_price;
}

void MarketManager::CheckAllItemData()
{
	bool is_change = false;	
	for (std::map<ItemID, MarketItemInfo>::iterator it = m_cross_gather_tran_record_list.begin(); it != m_cross_gather_tran_record_list.end(); ++it)
	{
		bool is_price_invaild = false;
		const MarketItemCfg * market_item_cfg = LOGIC_CONFIG->GetShopConfig()->GetMarketItemCfg(it->first);
		if (NULL == market_item_cfg)	//找不到对应配置则删除记录
		{
			m_dirty_mark[it->first] = true;
			m_old_state[it->first] = false;
			it->second.Reset();
			m_change_flag = true;
			is_change = true;
			continue;
		}
		else if (it->second.version != market_item_cfg->version)	//版号变更则重置价格
		{
			it->second.cur_price = market_item_cfg->price;
			it->second.yesterday_price = it->second.cur_price;
			it->second.transaction_num = 0;
			it->second.transaction_count = 0;
			it->second.version = market_item_cfg->version;
			m_dirty_mark[it->first] = true;
			m_old_state[it->first] = m_old_state[it->first] ? true : false;
			m_change_flag = true; 
			is_change = true;
			continue;
		}
		else if (it->second.cur_price < market_item_cfg->min_price)
		{
			it->second.cur_price = market_item_cfg->min_price;
			is_price_invaild = true;
		}
		else if (it->second.cur_price > market_item_cfg->max_price)
		{
			it->second.cur_price = market_item_cfg->max_price;
			is_price_invaild = true;
		}

		if (is_price_invaild)
		{
			it->second.yesterday_price = it->second.cur_price;
			m_dirty_mark[it->first] = true;
			m_old_state[it->first] = m_old_state[it->first] ? true : false;
			m_change_flag = true;
			is_change = true;
			continue;
		}
	}
	//需要把发生价格变化的发送给订阅的玩家,暂时全部发送
	if (is_change)
	{
		this->SendAllInfo(NULL);
	}
	if (this->IsDirty())
	{
		this->Save();
		m_next_save_db_timestamp = (unsigned int)(EngineAdapter::Instance().Time() + MAX_MARKET_DATA_SAVE_DB_TIME);
	}
}

void MarketManager::Save()
{
	if (!this->IsLoadFinish()) return;

	static MarketManagerParam market_param;
	this->GetInitParam(&market_param);

	RMISaveMarketItemDataBackObjectImpl *backobj = new RMISaveMarketItemDataBackObjectImpl();
	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

	bool ret = ac.SaveMarketItemDataAsyn(market_param, backobj);
	if (ret)
	{
		this->ClearDirtyMark(&market_param);
	}
}

void MarketManager::LoadMarketAllItemDataSucc()
{
	if (this->IsLoadFinish())
	{
		m_next_save_db_timestamp = (unsigned int)(EngineAdapter::Instance().Time() + MAX_MARKET_DATA_SAVE_DB_TIME);
	}
}

void MarketManager::SetDirty(ItemID item_id)
{
	m_dirty_mark[item_id] = true;
	m_old_state[item_id] = m_old_state[item_id] ? true : false;
	m_change_flag = true;
}
