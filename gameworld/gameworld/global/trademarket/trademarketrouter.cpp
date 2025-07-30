#include "trademarketrouter.hpp"
#include "obj/character/role.h"
#include "other/pet/pet.hpp"
#include "other/rolemodulemanager.hpp"
#include "item/knapsack.h"
#include "servercommon/errornum.h"
#include "item/itembase.h"
#include "item/itempool.h"
#include "engineadapter.h"
#include "servercommon/internalprotocal/gglobalprotocal.h"
#include "global/trademarket/trademarketmanager.hpp"
#include "global/trademarket/trademarketconfig.hpp"
#include "global/trademarket/usertradeinfo.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "other/rolecross/rolecross.hpp"
#include "internalcomm.h"
#include "world.h"
#include "other/event/eventhandler.hpp"
#include "other/trade_credit/trade_credit.hpp"
#include "global/cross/crossmanager.h"
#include "item/pet/petitem.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/pet/petconfig.hpp"
#include "gamelog.h"
#include "other/smartmountssystem/smartmountssystem.hpp"
#include "globalconfig/spidconfig.h"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivityimpl/randactivitygodbeastadvent.hpp"
#include "global/randactivity/randactivitymanager.hpp"

TradeMarketRouter& TradeMarketRouter::Instance()
{
	static TradeMarketRouter instance;
	return instance;
}

void TradeMarketRouter::OnUserLogin(const UserID& user_id)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL == role) return;

	this->QueryUserShelveInfo(role);
}

TradeMarketRouter::TradeMarketRouter()
{

}

TradeMarketRouter::~TradeMarketRouter()
{

}

void TradeMarketRouter::OnAddItemOnShelveReq(Role* role, Protocol::CSAddItemOnShelve* req)
{
	if (Role::AUTHORITY_TYPE_TEST == role->GetAuthorityType())
	{
		if (!GLOBALCONFIG->IsCanSell())
		{
			role->NoticeNum(errornum::EN_TRADE_MARKET_NEIWAN_CANNOT_SALE);
			return;
		}
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		if (!CrossManager::Instance().IsHiddenServerAlive())
		{
			role->NoticeNum(errornum::EN_CROSS_TRADE_MARKET_NOT_AVALIABLE_NOW);
			return;
		}

		if (!OrderManager::Instance().IsOpen())
		{
			role->NoticeNum(errornum::EN_ORIGIN_SERVER_TRADE_MARKET_UPSHELVE_NOT_AVAILABLE);
			return;
		}
	}

	if (req->item_num <= 0) return;

	if (req->price <= 0) return;

	static SaleItemParam saleitem_param;
	saleitem_param.Reset();

	if (1 == req->is_from_pet)
	{
		if (!role->GetRoleModuleManager()->GetPet()->GetPetGridData(req->index, &saleitem_param.item_wrapper))
		{
			role->NoticeNum(errornum::EN_PET_INDEX_ERROR);
			return;
		}

		if (1 == saleitem_param.item_wrapper.is_bind)
		{
			role->NoticeNum(errornum::EN_TRADE_MARKET_PET_BIND_CAN_NOT_TRADE);
			return;
		}

		if (role->GetRoleModuleManager()->GetPet()->IsInPlatfformBattle(saleitem_param.item_wrapper.item_unique_id, true))
		{
			return;
		}
		//先判断绑定
		if (role->GetRoleModuleManager()->GetPet()->IsBind(req->index))
		{
			role->NoticeNum(errornum::EN_PET_IS_BIND);
			return;
		}

		if (role->GetRoleModuleManager()->GetPet()->IsLock(req->index))
		{
			role->NoticeNum(errornum::EN_PET_IS_LOCK);
			return;
		}

		if (role->GetRoleModuleManager()->GetSmartMountsSystem()->IsPetDomination(saleitem_param.item_wrapper.item_unique_id))
		{
			role->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_PET_DOMINATION);
			return;
		}

		if (role->GetRoleModuleManager()->GetPet()->IsPetBenedictionVice(req->index))
		{
			role->NoticeNum(errornum::EN_PET_NOT_OP_BY_BENEDICTION);
			return;
		}

		static NetValueItemParam tmp_param;
		tmp_param.Reset();
		tmp_param.SetData(saleitem_param.item_wrapper.param_data);
		if (tmp_param.pet_param.pet_equipment_id != 0)
		{
			role->NoticeNum(errornum::EN_PET_CAN_NOT_SALE_EQUIP);
			return;
		}

		const TradePetPriceRangeCfg* price_range_cfg = TradeMarketConfig::Instance().GetPetPriceRangeCfg(tmp_param.pet_param.pet_id, tmp_param.pet_param.str_level);
		if (NULL == price_range_cfg)
		{
			role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}

		if (req->price < price_range_cfg->min_price || req->price > price_range_cfg->max_price)
		{
			role->NoticeNum(errornum::EN_TRADE_MARKET_PET_PRICE_NOT_IN_RANGE);
			return;
		}
		//还拥有宠物天赋则不能上架交易行
		if (tmp_param.pet_param.IsHasPetTalent())
		{
			role->NoticeNum(errornum::EN_PET_TALENT_NOT_RESET_NOT_TRADE);
			return;
		}
	}
	else
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->GetItemGridWrapData(req->column, req->index, &saleitem_param.item_wrapper))
		{
			role->NoticeNum(errornum::EN_KNAPSACK_LOAD_ERROR);
			return;
		}
	}
	 
	if (1 == saleitem_param.item_wrapper.is_bind)
	{
		role->NoticeNum(errornum::EN_ITEM_IS_BIND);
		return;
	}

	if (req->item_num > saleitem_param.item_wrapper.num)
	{
		role->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	const ItemBase* item = ITEMPOOL->GetItem(saleitem_param.item_wrapper.item_id);
	if (NULL == item) return;

	if (req->item_num % item->GetTradeNumPerGroup() != 0)
	{
		role->NoticeNum(errornum::EN_TRADE_MARKET_ITEM_NUM_NOT_DEVIDED_EVENLY);
		return;
	}

	if (I_TYPE_EQUIPMENT == item->GetItemType())
	{
		static NetValueItemParam tmp_param;
		tmp_param.Reset();
		tmp_param.SetData(saleitem_param.item_wrapper.param_data);
		if (0 == tmp_param.equip_param.is_treasure)
		{
			if (item->GetColor() != ItemBase::I_COLOR_ORANGE)
			{
				role->NoticeNum(errornum::EN_TRADE_MARKET_EQUIP_MUST_BE_TREASURE);
				return;
			}
		}

		if (tmp_param.equip_param.HasStone())
		{
			role->NoticeNum(errornum::EN_MARKET_SALE_EQUIP_HAS_STONE);
			return;
		}

		if (tmp_param.equip_param.effect_id > 0)
		{
			role->NoticeNum(errornum::EN_HAS_DIANHUASHI_ERR);
			return;
		}

		if (tmp_param.equip_param.refresh_effect_id > 0)
		{
			role->NoticeNum(errornum::EN_HAS_DIANHUASHI_ERR);
			return;
		}
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		// 隐藏服要判断价格，但其实后面还会再判断一次
		if (req->price < TradeMarketManager::GetTradeMinPrice(item, &saleitem_param.item_wrapper))
		{
			role->NoticeNum(errornum::EN_TRADE_MARKET_PRICE_TOO_LOW_TO_SALE);
			return;
		}

		if (req->price > TradeMarketManager::GetTradeMaxPrice(item, &saleitem_param.item_wrapper))
		{
			role->NoticeNum(errornum::EN_TRADE_MARKET_PRICE_TOO_HIGH_TO_SALE);
			return;
		}
	}

	if (!item->CanTrade())
	{
		role->NoticeNum(errornum::EN_ITEM_CAN_NOT_TRADE);
		return;
	}

	UserTradeInfo* user_trade_info = NULL;
	if (CrossConfig::Instance().IsHiddenServer())
	{
		TradeMarketManager::Instance().GetUserTradeInfo(role->GetUID());
		if (NULL != user_trade_info)
		{
			if (!user_trade_info->GetEmptyIndex(NULL))
			{
				role->NoticeNum(errornum::EN_TRADE_MARKET_SHELVE_FULL);
				return;
			}
		}
	}

	int group_num = req->item_num / item->GetTradeNumPerGroup();
	long long credit_fee = 1LL * req->price * group_num; // 信用费用是1比1
	long long service_fee = 1LL * req->price * group_num;
	if (service_fee < TRADEMARKET_CFG.GetConstantCfg().upshelve_fee_min)
	{
		service_fee = TRADEMARKET_CFG.GetConstantCfg().upshelve_fee_min;
	}
	else if (service_fee > TRADEMARKET_CFG.GetConstantCfg().upshelve_fee_max)
	{
		service_fee = TRADEMARKET_CFG.GetConstantCfg().upshelve_fee_max;
	}

	if (!role->GetRoleModuleManager()->GetMoney()->CoinBindMoreThan(service_fee))
	{
		role->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return;
	}

	if (!role->GetRoleModuleManager()->GetTradeCredit()->CreditMorThan(credit_fee))
	{
		role->NoticeNum(errornum::EN_CREDIT_NUM_NOT_ENOUGH);
		return;
	}

	if (1 == req->is_from_pet)
	{	
		{
			// 如果高于1级，则归元到1级，并更新ItemWrapper数据
			static NetValueItemParam tmp_param;
			tmp_param.Reset();
			tmp_param.SetData(saleitem_param.item_wrapper.param_data);
			
			if (tmp_param.pet_param.level > 1)
			{
				if (0 != role->GetRoleModuleManager()->GetPet()->Rebirth(req->index))
				{
					role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
					return;
				}
			}

			role->GetRoleModuleManager()->GetPet()->ClearBeforeSale(req->index);

			if (!role->GetRoleModuleManager()->GetPet()->GetPetGridData(req->index, &saleitem_param.item_wrapper))
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return;
			}
		}

		if (!role->GetRoleModuleManager()->GetPet()->OnTradeConsumePet(req->index))
		{
			return;
		}
	}
	else
	{
		if (ItemNamespace::ITEM_COLUMN_TYPE_STORAGE == req->column || ItemNamespace::ITEM_COLUMN_TYPE_TEMPORARY == req->column)
		{
			role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}

		if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItemByIndex(req->column, req->index, req->item_num, __FUNCTION__))
		{
			role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}
	}

	if (!role->GetRoleModuleManager()->GetMoney()->UseCoinBind(service_fee, __FUNCTION__))
	{
		role->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return;
	}

	if (!role->GetRoleModuleManager()->GetTradeCredit()->UseCredit(credit_fee, __FUNCTION__))
	{
		role->NoticeNum(errornum::EN_CREDIT_NUM_NOT_ENOUGH);
		return;
	}

	if (ItemNamespace::ITEM_TRADE_MONEY_TYPE_GOLD == item->GetTradeMoneyType())
	{
		saleitem_param.gold_price = req->price;
	}
	else
	{	
		saleitem_param.immortal_coin_price = req->price;
	}
	saleitem_param.cur_status = SALE_ITEM_STATUS_ON_SHELVE;
	saleitem_param.is_need_announcement = TradeMarketConfig::Instance().IsNeedAnnounce(item);
	saleitem_param.item_search_type = item->GetTradeSearchType();
	saleitem_param.item_wrapper.num = req->item_num;
	saleitem_param.num_per_group = item->GetTradeNumPerGroup();

	//获取下一个整点的时间戳
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	saleitem_param.put_on_shelve_time = now;

	//获取unique_key
	saleitem_param.unique_key = TradeMarketManager::Instance().GetUniqueItemKey();

#ifdef _WIN32
	saleitem_param.next_status_time = now + 5; // 方便调试
#else
	saleitem_param.next_status_time = now + EngineAdapter::Instance().NextHourInterval(0, 0);
#endif	

	gamelog::g_log_trade_market.printf(LL_INFO, "TradeMarketRouter::OnAddItemOnShelveReq role[%d,%s] \
		item_id[%d] num[%d] price[%d] num_per_group[%d] unique_key[%lld]",
		role->GetUID(), role->GetName(), saleitem_param.item_wrapper.item_id, saleitem_param.item_wrapper.num, 
		saleitem_param.gold_price, saleitem_param.num_per_group, saleitem_param.unique_key);

	if (!CrossConfig::Instance().IsHiddenServer()) //原服购买跨服商品 特殊处理
	{
		crossgameprotocal::GameHiddenTradeMarketRequestUpshelve upshelve_req;
		upshelve_req.plat_type = LocalConfig::Instance().GetPlatType();
		upshelve_req.server_id = LocalConfig::Instance().GetMergeServerId();

		upshelve_req.role_id = role->GetUID();
		F_STRNCPY(upshelve_req.role_name, role->GetName(), sizeof(upshelve_req.role_name));
		F_STRNCPY(upshelve_req.role_pname, role->GetPlatName(), sizeof(upshelve_req.role_pname));
		upshelve_req.gold_price = saleitem_param.gold_price;
		upshelve_req.item_wrapper = saleitem_param.item_wrapper;
		upshelve_req.service_fee = service_fee;
		upshelve_req.credit_fee = credit_fee;

		// 加入订单数据
		upshelve_req.order_id = OrderManager::Instance().RegisterOrder(TRADEMARKET_ORDER_TYPE_UPSHELVE, role->GetUID());
		OrderManager::Instance().OnOrderCostCoin(upshelve_req.order_id, service_fee);
		OrderManager::Instance().OnOrderCostTradeCredit(upshelve_req.order_id, credit_fee);
		OrderManager::Instance().OnOrderConsumeItem(upshelve_req.order_id, saleitem_param.item_wrapper);
		OrderManager::Instance().OnOrderSwitchToStatus(upshelve_req.order_id, TRADEMARKET_ORDER_STATUS_CONSUME);

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&upshelve_req, sizeof(upshelve_req));
		return;
	}
	else
	{
		bool ret = TradeMarketManager::Instance().OnAddItemOnShelveReq(role, saleitem_param, service_fee, credit_fee);
		if (ret) EventHandler::Instance().OnTradeMarketPutaway(role);
	}
	
}

void TradeMarketRouter::OnRemoveItemOnShelveReq(Role* role, Protocol::CSRemoveItemOnShelve* req)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		if (!CrossManager::Instance().IsHiddenServerAlive())
		{
			role->NoticeNum(errornum::EN_CROSS_TRADE_MARKET_NOT_AVALIABLE_NOW);
			return;
		}

		crossgameprotocal::GameHiddenTradeMarketOffshelveReq msg;
		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.server_id = LocalConfig::Instance().GetMergeServerId();
		msg.role_id = role->GetUID();
		F_STRNCPY(msg.role_name, role->GetName(), sizeof(msg.role_name));
		F_STRNCPY(msg.role_pname, role->GetPlatName(), sizeof(msg.role_pname));
		msg.sale_index = req->sale_index;

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));

		return;
	}

	TradeMarketManager::Instance().OnRemoveItemOnShelveReq(role, req->sale_index);
}

void TradeMarketRouter::OnQueryItemRecommendPriceReq(Role* role, Protocol::CSQueryItemRecommendPrice* req)
{
	static ItemDataWrapper wrapper;
	wrapper.Reset();

	if (1 == req->is_from_pet)
	{
		if (!role->GetRoleModuleManager()->GetPet()->GetPetGridData(req->index, &wrapper))
		{
			role->NoticeNum(errornum::EN_PET_INDEX_ERROR);
			return;
		}
	}
	else
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->GetItemGridWrapData(req->column, req->index, &wrapper))
		{
			role->NoticeNum(errornum::EN_KNAPSACK_LOAD_ERROR);
			return;
		}
	}

	const ItemBase* item = ITEMPOOL->GetItem(wrapper.item_id);
	if (NULL == item)
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenQueryItemRecommendPrice ghqirp;
		ghqirp.plat_type = LocalConfig::Instance().GetPlatType();
		ghqirp.server_id = LocalConfig::Instance().GetMergeServerId();
		ghqirp.role_id = role->GetUID();
		ghqirp.is_from_pet = req->is_from_pet;
		ghqirp.reserve_ch = 0;
		ghqirp.reserve_sh = 0;
		ghqirp.column = req->column;
		ghqirp.index = req->index;
		ghqirp.item_wrapper = wrapper;

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&ghqirp, sizeof(ghqirp));
	}
	else
	{
		int min_price = 0;
		int max_price = 0;
		int recommend_price = 0;
		TradeMarketManager::QueryItemTradePrice(item, &wrapper, &min_price, &max_price, &recommend_price);

		Protocol::SCQueryItemRecommendPriceResp resp;
		resp.reserve_ch = req->reserve_ch;
		resp.reserve_sh = req->reserve_sh;
		resp.is_from_pet = req->is_from_pet;
		resp.index = req->index;
		resp.column = req->column;
		resp.min_price = min_price;
		resp.max_price = max_price;
		resp.price = recommend_price;

		EngineAdapter::Instance().NetSend(role->GetNetId(), &resp, sizeof(resp));
	}
}

void TradeMarketRouter::OnCashOutReq(Role* role, Protocol::CSTradeMarketCashOut* req)
{
	if (Role::AUTHORITY_TYPE_TEST == role->GetAuthorityType())
	{
		role->NoticeNum(errornum::EN_TRADE_MARKET_NEIWAN_CANNOT_SALE);
		return;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		if (!CrossManager::Instance().IsHiddenServerAlive())
		{
			role->NoticeNum(errornum::EN_CROSS_TRADE_MARKET_NOT_AVALIABLE_NOW);
			return;
		}

		crossgameprotocal::GameHiddenTradeMarketCashOutReq msg;
		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.server_id = LocalConfig::Instance().GetMergeServerId();
		msg.role_id = role->GetUID();
		F_STRNCPY(msg.role_name, role->GetName(), sizeof(msg.role_name));
		F_STRNCPY(msg.role_pname, role->GetPlatName(), sizeof(msg.role_pname));
		msg.sale_index = req->sale_index;
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));

		return;
	}

	TradeMarketManager::Instance().OnCashOutReq(role, req->sale_index);
}

void TradeMarketRouter::OnUserShelveInfoReq(Role* role)
{
	this->QueryUserShelveInfo(role);
}

void TradeMarketRouter::OnSearchByItemIdReq(Role* role, Protocol::CSTradeMarketSearchByItemID* req)
{
	if (req->req_page < 0) return;

	const ItemBase* item = ITEMPOOL->GetItem(req->item_id);
	if (NULL == item) return;
	if (!item->CanTrade()) return;

	//if (I_TYPE_EQUIPMENT != item->GetItemType() &&
	//	I_TYPE_PET != item->GetItemType() &&
	//	req->req_page > 0)
	//{
	//	role->NoticeNum(errornum::EN_TRADE_MARKET_CANNOT_FLIP_PAGE);
	//	return;
	//}

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	//if (role->GetRoleModuleManager()->GetLastSearchTradeMarketTime() + 1 >= now)
	//{
	//	role->NoticeNum(errornum::EN_TRADE_MARKET_ITEM_SEARCH_TOO_FREQUENTLY);
	//	return;
	//}
	role->GetRoleModuleManager()->OnSearchTradeMarket(now);
	
	if (CrossConfig::Instance().IsHiddenServer())
	{
		TradeMarketManager::Instance().OnSearchByItemIDReq(role, req->item_id, req->req_page, req->is_announce > 0, req->order_type);
	}
	else
	{
		static crossgameprotocal::GameHiddenTradeMarketSearchById msg;
		msg.uuid = role->GetRoleModuleManager()->GetRoleCross()->GetOriginUUID();
		msg.item_id = req->item_id;
		msg.order_type = req->order_type;
		msg.req_page = req->req_page;
		msg.is_announce = req->is_announce;

		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.server_id = LocalConfig::Instance().GetMergeServerId();

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}
}

void TradeMarketRouter::OnSearchByItemSearchTypeReq(Role* role, Protocol::CSTradeMarketSearchByItemSearchType* req)
{
	if (req->req_page < 0) return;
	if (req->item_search_type < 0 || req->item_search_type >= TRADE_ITEM_SEARCH_TYPE_MAX) return;

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	//if (role->GetRoleModuleManager()->GetLastSearchTradeMarketTime() + 1 >= now)
	//{
	//	role->NoticeNum(errornum::EN_TRADE_MARKET_ITEM_SEARCH_TOO_FREQUENTLY);
	//	return;
	//}
	role->GetRoleModuleManager()->OnSearchTradeMarket(now);

	if (req->item_search_type < TRADE_ITEM_LEVEL_SEARCH_TYPE_NUM && req->param > 0)
	{
		if (CrossConfig::Instance().IsHiddenServer())
		{
			TradeMarketManager::Instance().OnSearchByItemTypeWithLevelReq(role, req->param, req->item_search_type, req->req_page, req->is_announce > 0, req->order_type);
		}
		else
		{
			static crossgameprotocal::GameHiddenTradeMarketSearchByTypeWithLevel msg;
			msg.uuid = role->GetRoleModuleManager()->GetRoleCross()->GetOriginUUID();
			msg.item_big_search_type = req->item_big_search_type;
			msg.item_search_type = req->item_search_type;
			msg.is_announce = req->is_announce;
			msg.level_param = req->param;
			msg.order_type = req->order_type;
			msg.req_page = req->req_page;

			msg.plat_type = LocalConfig::Instance().GetPlatType();
			msg.server_id = LocalConfig::Instance().GetMergeServerId();

			InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
		}
	}
	else if (req->item_search_type >= TRADE_MEDAL_SEARCH_TYPE_MIN && req->item_search_type <= TRADE_MEDAL_SEARCH_TYPE_MAX)
	{
		if (CrossConfig::Instance().IsHiddenServer())
		{
			short color = req->item_search_type - static_cast<short>(TRADE_MEDAL_SEARCH_TYPE_MIN) + ItemBase::I_COLOR_GREEN;
			TradeMarketManager::Instance().OnSearchMedalWithColor(role, color, req->req_page, req->is_announce, req->order_type, req->param);
		}
		else
		{
			static crossgameprotocal::GameHiddenTradeMarketSearchMedalByColor msg;
			msg.uuid = role->GetRoleModuleManager()->GetRoleCross()->GetOriginUUID();
			msg.medal_color = req->item_search_type - static_cast<short>(TRADE_MEDAL_SEARCH_TYPE_MIN) + ItemBase::I_COLOR_GREEN;
			msg.is_announce = req->is_announce;
			msg.order_type = req->order_type;
			msg.req_page = req->req_page;
			msg.prof_param = req->param;
			
			msg.plat_type = LocalConfig::Instance().GetPlatType();
			msg.server_id = LocalConfig::Instance().GetMergeServerId();

			InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
		}
	}
	else
	{
		if (CrossConfig::Instance().IsHiddenServer())
		{
			TradeMarketManager::Instance().OnSearchByItemTypeReq(role, req->item_search_type, req->req_page, req->is_announce > 0, req->order_type);
		}
		else
		{
			static crossgameprotocal::GameHiddenTradeMarketSearchByType msg;
			msg.uuid = role->GetRoleModuleManager()->GetRoleCross()->GetOriginUUID();
			msg.item_big_search_type = req->item_big_search_type;
			msg.item_search_type = req->item_search_type;
			msg.is_announce = req->is_announce;
			msg.param = req->param;
			msg.order_type = req->order_type;
			msg.req_page = req->req_page;
			
			msg.plat_type = LocalConfig::Instance().GetPlatType();
			msg.server_id = LocalConfig::Instance().GetMergeServerId();
			InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
		}
	}
}

void TradeMarketRouter::OnSearchByItemSearchBigTypeReq(Role* role, Protocol::CSTradeMarketSearchByItemSearchType* req)
{
	if (req->req_page < 0) return;
	if (req->item_big_search_type < 0 || req->item_big_search_type >= TRADE_ITEM_SEARCH_BIG_TYPE_MAX) return;

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	//if (role->GetRoleModuleManager()->GetLastSearchTradeMarketTime() + 1 >= now)
	//{
	//	role->NoticeNum(errornum::EN_TRADE_MARKET_ITEM_SEARCH_TOO_FREQUENTLY);
	//	return;
	//}
	role->GetRoleModuleManager()->OnSearchTradeMarket(now);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		TradeMarketManager::Instance().OnSearchByItemBigTypeReq(role, req->item_big_search_type, req->req_page, req->is_announce > 0, req->order_type);
	}
	else
	{
		static crossgameprotocal::GameHiddenTradeMarketSearchByBigType msg;
		msg.uuid = role->GetRoleModuleManager()->GetRoleCross()->GetOriginUUID();
		msg.item_big_search_type = req->item_big_search_type;
		msg.item_search_type = req->item_search_type;
		msg.is_announce = req->is_announce;
		msg.param = req->param;
		msg.order_type = req->order_type;
		msg.req_page = req->req_page;

		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.server_id = LocalConfig::Instance().GetMergeServerId();
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}
}

void TradeMarketRouter::OnBuyItemReq(Role* role, Protocol::CSTradeMarketBuyItem* req)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		if (!CrossManager::Instance().IsHiddenServerAlive())
		{
			role->NoticeNum(errornum::EN_CROSS_TRADE_MARKET_NOT_AVALIABLE_NOW);
			return;
		}

		if (!OrderManager::Instance().IsOpen())
		{
			role->NoticeNum(errornum::EN_ORIGIN_SERVER_TRADE_MARKET_BUY_NOT_AVAILABLE);
			return;
		}
	}

	if (req->sale_index < 0 || req->sale_index >= USER_SALE_ITEM_MAX_NUM)
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (req->buy_num <= 0)
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	ItemID buy_item_id = req->buy_item_id;
	const ItemBase* item = ITEMPOOL->GetItem(buy_item_id);
	if (NULL == item || !item->CanTrade())
	{
		role->NoticeNum(errornum::EN_ITEM_CANNOT_TRADE);
		return;
	}

	if (req->buy_num % item->GetTradeNumPerGroup() != 0)
	{
		role->NoticeNum(errornum::EN_TRADE_MARKET_ITEM_NUM_NOT_DEVIDED_EVENLY);
		return;
	}

	if (I_TYPE_PET == item->GetItemType())
	{
		if (!role->GetRoleModuleManager()->GetPet()->HaveSpace(1))
		{
			role->NoticeNum(errornum::EN_PET_COUNT_LIMIT);
			return;
		}
	}
	else
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(req->buy_item_id, req->buy_num, false))
		{
			role->GetRoleModuleManager()->GetKnapsack()->SendErrorNotice();
			return;
		}
	}

	if (req->seller_uid == role->GetUID())
	{
		//买自己的东西得有多傻啊
		/*role->NoticeNum(errornum::EN_MARKET_BUY_MY_SELLER);
		return;*/
	}
	bool ret = true;
	long long unique_key = 0LL;
	if (!ConvertUniqueKeyStrToNumber(req->unique_key_str, &unique_key))
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (!CrossConfig::Instance().IsHiddenServer()) //原服购买跨服商品 特殊处理
	{
		crossgameprotocal::GameHiddenTradeMarketRequestBuy buy_req;
		buy_req.buy_item_id = buy_item_id;
		buy_req.buy_num = req->buy_num;
		buy_req.expect_price = req->expect_price;
		buy_req.reserve_sh = 0;
		buy_req.role_id = role->GetUID();
		buy_req.sale_index = req->sale_index;
		buy_req.seller_uid = req->seller_uid;
		F_STRNCPY(buy_req.unique_key_str, req->unique_key_str, sizeof(buy_req.unique_key_str));
		buy_req.upshelve_time = req->upshelve_time;

		buy_req.plat_type = LocalConfig::Instance().GetPlatType();
		buy_req.server_id = LocalConfig::Instance().GetMergeServerId();

		long long order_id = OrderManager::Instance().RegisterOrder(TRADEMARKET_ORDER_TYPE_BUY, role->GetUID());
		buy_req.order_id = order_id;
		OrderManager::Instance().OnOrderSwitchToStatus(buy_req.order_id, TRADEMARKET_ORDER_STATUS_QUERY);

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&buy_req, sizeof(buy_req));
		return;
	}

	int out_consume_gold = 0;
	ret = TradeMarketManager::Instance().OnBuyItemReq(role, req->seller_uid, req->sale_index, req->buy_num, req->upshelve_time, I_TYPE_PET == item->GetItemType(), unique_key, &out_consume_gold);
	
	// 转到跨服后，不再允许自动购买功能

	if (ret)
	{
		EventHandler::Instance().OnTradeMarketPurchase(role, buy_item_id, out_consume_gold); 
		EventHandler::Instance().OnTradeMarketSaleItem(req->seller_uid, buy_item_id, req->seller_uid == role->GetUID());
	}
}

void TradeMarketRouter::OnGetTransactionRecordsReq(Role* role, Protocol::CSRecentTransactionRecords* req)
{
	const ItemBase* item = ITEMPOOL->GetItem(req->item_id);
	if (NULL == item) return;
	if (!item->CanTrade()) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		TradeMarketManager::Instance().OnGetTransactionRecordsReq(role, req->item_id);
	}
	else
	{
		static crossgameprotocal::GameHiddenTradeMarketGetTransactionRecord msg;
		msg.uuid = role->GetRoleModuleManager()->GetRoleCross()->GetOriginUUID();
		msg.item_id = req->item_id;
		msg.reserve_sh = 0;

		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.server_id = LocalConfig::Instance().GetMergeServerId();
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}
}

void TradeMarketRouter::OnGetFocusListReq(Role* role, Protocol::CSTradeMarketGetFocusList* req)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		TradeMarketManager::Instance().OnGetFocusListReq(role);
	}
	else
	{
		static crossgameprotocal::GameHiddenTradeMarketGetRoleFocusList msg;
		msg.uuid = role->GetRoleModuleManager()->GetRoleCross()->GetOriginUUID();

		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.server_id = LocalConfig::Instance().GetMergeServerId();
		InternalComm::Instance().SendToHiddenThroughCross( (const char*)&msg, sizeof(msg));
	}
	
}

void TradeMarketRouter::OnAddFocusReq(Role* role, Protocol::CSTradeMarketAddFocus* req)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		TradeMarketManager::Instance().OnAddFocusReq(role, req->seller_uid, req->sale_index, req->upshelve_time);
	}
	else
	{
		static crossgameprotocal::GameHiddenTradeMarketAddFocusItem msg;
		msg.uuid = role->GetRoleModuleManager()->GetRoleCross()->GetOriginUUID();
		msg.sale_index = req->sale_index;
		msg.seller_uid = req->seller_uid;
		msg.upshelve_time = req->upshelve_time;
		msg.reserve_sh = req->reserve_sh;

		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.server_id = LocalConfig::Instance().GetMergeServerId();

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}
}

void TradeMarketRouter::OnRemoveFocusReq(Role* role, Protocol::CSTradeMarketRemoveFocus* req)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		TradeMarketManager::Instance().OnRemoveFocusReq(role, req->focus_index, req->cur_page, 1 == req->is_announce);
	}
	else
	{
		static crossgameprotocal::GameHiddenTradeMarketRemoveFocusItem msg;
		msg.uuid = role->GetRoleModuleManager()->GetRoleCross()->GetOriginUUID();
		msg.cur_page = req->cur_page;
		msg.focus_index = req->focus_index;
		msg.is_announce = req->is_announce;

		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.server_id = LocalConfig::Instance().GetMergeServerId();

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}
}

void TradeMarketRouter::OnGetFocusItemInfoReq(Role* role, Protocol::CSTradeMarketGetFocusItemInfo* req)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		TradeMarketManager::Instance().OnGetFocusItemInfoReq(role, req->focus_index, req->cur_page, 1 == req->is_announce);
	}
	else
	{
		static crossgameprotocal::GameHiddenTradeMarketGetFocusItemInfo msg;
		msg.uuid = role->GetRoleModuleManager()->GetRoleCross()->GetOriginUUID();
		msg.cur_page = req->cur_page;
		msg.focus_index = req->focus_index;
		msg.is_announce = req->is_announce;

		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.server_id = LocalConfig::Instance().GetMergeServerId();

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}
}

void TradeMarketRouter::OnAdvertiseReq(Role* role, Protocol::CSTradeMarketAdvertise* req)
{
	if (NULL == role || NULL == req) return;

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		if (!CrossManager::Instance().IsHiddenServerAlive())
		{
			role->NoticeNum(errornum::EN_CROSS_TRADE_MARKET_NOT_AVALIABLE_NOW);
			return;
		}

		crossgameprotocal::GameHiddenTradeMarketAdvertiseReq msg;
		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.server_id = LocalConfig::Instance().GetMergeServerId();
		msg.role_id = role->GetUID();
		msg.sale_index = req->sale_index;
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
		
		return;
	}

	TradeMarketManager::Instance().OnAdvertiseReq(role, req->sale_index);
}

void TradeMarketRouter::OnSearchByAdvertiseReq(Role* role, Protocol::CSTradeMarketSearchAdvertise* req)
{
	const ItemBase* item = ITEMPOOL->GetItem(req->item_id);
	if (NULL == item) return;
	if (!item->CanTrade()) return;

	long long unique_key = 0;
	if (!ConvertUniqueKeyStrToNumber(req->unique_key_str, &unique_key))
	{
		return;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		if (!CrossManager::Instance().IsHiddenServerAlive())
		{
			role->NoticeNum(errornum::EN_CROSS_TRADE_MARKET_NOT_AVALIABLE_NOW);
			return;
		}
		
		crossgameprotocal::GameHiddenTradeMarketSearchAdvertiseItemReq msg;
		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.server_id = LocalConfig::Instance().GetMergeServerId();
		msg.role_id = role->GetUID();
		
		msg.item_id = req->item_id;
		msg.price = req->price;
		msg.put_on_shelve_time = req->put_on_shelve_time;
		msg.sale_index = req->sale_index;
		msg.seller_uid = req->seller_uid;
		msg.unique_key = unique_key;

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
		return;
	}	

	if (req->seller_uid != SYSTEM_TRADE_UID)
	{
		TradeMarketManager::Instance().OnSearchByAdvertiseReq(role, req->seller_uid, req->sale_index, req->put_on_shelve_time, req->price, req->item_id, item->GetItemSubType(), unique_key);
	}
	else
	{
		TradeMarketManager::Instance().OnSearchSystemByAdvertiseReq(role, req->seller_uid, req->sale_index, req->put_on_shelve_time, req->price, req->item_id, item->GetItemSubType(), unique_key);
	}
}

void TradeMarketRouter::OnGetSellingListReq(Role* role, Protocol::CSTradeMarketGetSellingList* req)
{
	const ItemBase* item = ITEMPOOL->GetItem(req->item_id);
	if (NULL == item) return;
	if (!item->CanTrade()) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		TradeMarketManager::Instance().OnGetSellingListReq(role, req->item_id, req->special_effect_type);
	}
	else
	{
		crossgameprotocal::GameHiddenTradeMarketGetSellingList list;
		list.item_id = req->item_id;
		list.special_effect_type = req->special_effect_type;
		list.uuid = role->GetRoleModuleManager()->GetRoleCross()->GetOriginUUID();

		list.plat_type = LocalConfig::Instance().GetPlatType();
		list.server_id = LocalConfig::Instance().GetMergeServerId();

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&list, sizeof(list));
	}
}

void TradeMarketRouter::OnGetRoleTransactionRecordsReq(Role* role, Protocol::CSTradeMarketGetRoleTransactionRecord* req)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		TradeMarketManager::Instance().OnGetRoleTradeRecordReq(role);
	}
	else
	{
		static crossgameprotocal::GameHiddenTradeMarketGetRoleTradeRecord msg;
		msg.uuid = role->GetRoleModuleManager()->GetRoleCross()->GetOriginUUID();

		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.server_id = LocalConfig::Instance().GetMergeServerId();

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}
}

void TradeMarketRouter::OnRedoAddItemOnShelveReq(Role* role, Protocol::CSTradeMarketRedoAddItemOnShelve* req)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		if (!CrossManager::Instance().IsHiddenServerAlive())
		{
			role->NoticeNum(errornum::EN_CROSS_TRADE_MARKET_NOT_AVALIABLE_NOW);
			return;
		}

		crossgameprotocal::GameHiddenTradeMarketRedoUpshelveReq msg;
		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.server_id = LocalConfig::Instance().GetMergeServerId();
		msg.role_id = role->GetUID();
		msg.price = req->price;
		msg.sale_index = req->sale_index;
		F_STRNCPY(msg.role_name, role->GetName(), sizeof(msg.role_name));
		F_STRNCPY(msg.role_pname, role->GetPlatName(), sizeof(msg.role_pname));

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
		return;
	}

	int ret = TradeMarketManager::Instance().OnRedoAddItemOnShelveReq(role, req->sale_index, req->price);
	if(ret) EventHandler::Instance().OnTradeMarketPutaway(role);
}

void TradeMarketRouter::OnSearchPetForQuickBuyReq(Role* role, Protocol::CSTradeMarketSearchPetForQuickBuy* req)
{
	if (req->item_id > 0)
	{
		const ItemBase* item = ITEMPOOL->GetItem(req->item_id);
		if (NULL == item || !item->CanTrade() || I_TYPE_PET != item->GetItemType())
		{
			role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
	}
	else if (req->quality <= PET_QUALITY_INVALID || req->quality >= PET_QUALITY_MAX)
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	
	ItemID pet_id = req->item_id;
	const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
	if (NULL != pet_cfg && pet_cfg->pet_mutant_times)
	{
		pet_id = pet_cfg->prototype_pet_id;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		if (!CrossManager::Instance().IsHiddenServerAlive())
		{
			role->NoticeNum(errornum::EN_CROSS_TRADE_MARKET_NOT_AVALIABLE_NOW);
			return;
		}
		
		crossgameprotocal::GameHiddenTradeMarketSearchPetForQuickBuy msg;
		msg.role_id = role->GetUID();
		msg.item_id = pet_id;
		msg.last_price = req->last_price;
		msg.last_put_on_shelve_time = req->last_put_on_shelve_time;
		msg.last_seller_uid = req->last_seller_uid;
		msg.last_str_level = req->last_str_level;
		msg.order_type = req->order_type;
		msg.quality = req->quality;

		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.server_id = LocalConfig::Instance().GetMergeServerId();

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));

		return;
	}

	//unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	//if (role->GetRoleModuleManager()->GetLastSearchTradeMarketTime() + 1 >= now)
	//{
	//	role->NoticeNum(errornum::EN_TRADE_MARKET_ITEM_SEARCH_TOO_FREQUENTLY);
	//	return;
	//}

	TradeMarketManager::Instance().OnSearchPetForQuickBuyReq(role, req->order_type, pet_id, req->quality, 
		req->last_seller_uid, req->last_price, req->last_put_on_shelve_time, req->last_str_level);
}

void TradeMarketRouter::OnGetMerchandiseNumReq(Role* role, Protocol::CSTradeMarketGetMerchandiseNum* req)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		TradeMarketManager::Instance().OnGetMerchandiseNumReq(role, req->big_search_type, req->is_announce > 0);
	}
	else
	{
		crossgameprotocal::GameHiddenTradeMarketGetMerchandiseNum msg;
		msg.big_search_type = req->big_search_type;
		msg.is_announce = req->is_announce;
		msg.uuid = role->GetRoleModuleManager()->GetRoleCross()->GetOriginUUID();

		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.server_id = LocalConfig::Instance().GetMergeServerId();
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}
}

void TradeMarketRouter::OnSearchMedalByEffectTypeAndColorReq(Role* role, Protocol::CSTradeMarketSearchMedalByEffectTypeAndColor* req)
{
	if (req->req_page < 0) return;

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	//if (role->GetRoleModuleManager()->GetLastSearchTradeMarketTime() + 1 >= now)
	//{
	//	role->NoticeNum(errornum::EN_TRADE_MARKET_ITEM_SEARCH_TOO_FREQUENTLY);
	//	return;
	//}
	role->GetRoleModuleManager()->OnSearchTradeMarket(now);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		TradeMarketManager::Instance().OnSearchMedalByEffectTypeAndColorReq(role, req->sp_type, req->color, req->req_page, req->is_announce, req->order_type);
	}
	else
	{
		static crossgameprotocal::GameHiddenTradeMarketSearchMedalByEffectTypeAndColor msg;
		msg.uuid = role->GetRoleModuleManager()->GetRoleCross()->GetOriginUUID();
		msg.sp_type = req->sp_type;
		msg.medal_color = req->color;
		msg.reserve_sh = 0;
		msg.is_announce = req->is_announce;
		msg.order_type = req->order_type;
		msg.req_page = req->req_page;

		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.server_id = LocalConfig::Instance().GetMergeServerId();

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}
}

void TradeMarketRouter::QueryUserShelveInfo(Role* role)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		TradeMarketManager::Instance().OnUserShelveDataReq(role);
	}
	else
	{
		static crossgameprotocal::GameHiddenTradeMarketUserShelveInfo info;
		info.uuid = role->GetRoleModuleManager()->GetRoleCross()->GetOriginUUID();

		info.plat_type = LocalConfig::Instance().GetPlatType();
		info.server_id = LocalConfig::Instance().GetMergeServerId();
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&info, sizeof(info));
	}
}

