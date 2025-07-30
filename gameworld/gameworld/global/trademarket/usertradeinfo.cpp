#include "usertradeinfo.hpp"
#include "gamelog.h"
#include "usersaleitem.hpp"
#include "engineadapter.h"
#include "global/rmibackobjdef.h"
#include "internalcomm.h"
#include "trademarketmanager.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "protocol/msgtrademarket.h"
#include "item/itembase.h"
#include "trademarketconfig.hpp"
#include "other/event/eventhandler.hpp"
#include "other/pet/petconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "usertraderecord.hpp"
#include "global/usercache/usercache.hpp"

UserTradeInfo::UserTradeInfo(int role_id) : m_role_id(role_id), m_is_change(false), 
	m_next_check_status_time(0u), m_next_save_db_time(0u), m_focus_is_change(false), 
	m_is_record_change(false), m_next_save_record_time(0u), m_record_old_state(false),
	m_sold_record(NULL), m_buy_record(NULL), m_next_check_afk_clear_trade_record_timestamp(0u)
{
	memset(m_old_state, 0, sizeof(m_old_state));
	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));
	memset(m_sale_list, 0, sizeof(m_sale_list));

	memset(m_focus_old_state, 0, sizeof(m_focus_old_state));
	memset(m_focus_dirty_mark, 0, sizeof(m_focus_dirty_mark));
	memset(m_focus_list, 0, sizeof(m_focus_list));

	unsigned int now_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	m_next_save_db_time = now_time + USER_TRADE_INFO_SAVE_DB_INTERVAL_S;
	m_next_check_status_time = now_time + USER_TRADE_INFO_CHECK_STATUS_INTERVAL_S;
	m_next_save_record_time = now_time + USER_TRADE_INFO_SAVE_RECORD_INTERVAL_S;

	m_next_check_afk_clear_trade_record_timestamp = now_time + 5;
}

UserTradeInfo::~UserTradeInfo()
{
	for (int i = 0; i < USER_SALE_ITEM_MAX_NUM; ++i)
	{
		if (NULL != m_sale_list[i])
		{
			delete m_sale_list[i];
			m_sale_list[i] = NULL;
		}
	}

	for (int i = 0; i < TRADE_MARKET_USER_FOCUS_MAX_NUM; ++i)
	{
		if (NULL != m_focus_list[i])
		{
			delete m_focus_list[i];
			m_focus_list[i] = NULL;
		}
	}

	if (NULL != m_sold_record)
	{
		delete m_sold_record;
		m_sold_record = NULL;
	}

	if (NULL != m_buy_record)
	{
		delete m_buy_record;
		m_buy_record = NULL;
	}
}

void UserTradeInfo::Update(time_t now)
{
	unsigned int now_time = static_cast<unsigned int>(now);
	if (now_time > m_next_save_db_time && this->IsChanged())
	{
		if (this->SaveDB())
		{
			m_next_save_db_time = now_time + USER_TRADE_INFO_SAVE_DB_INTERVAL_S;
			this->ClearDirtyMark();
		}
	}

	if (now_time > m_next_check_status_time)
	{
		m_next_check_status_time = now_time + USER_TRADE_INFO_CHECK_STATUS_INTERVAL_S;
		
		this->UpdateSaleItemList(static_cast<unsigned int>(now));
	}

	if (now_time > m_next_save_record_time)
	{
		if (this->SaveRecord())
		{
			m_next_save_record_time = now_time + USER_TRADE_INFO_SAVE_RECORD_INTERVAL_S;
			m_is_record_change = false;
			m_record_old_state = true; // 由于不会删除， 在保存后可直接改为true
		}
	}

	if (now > m_next_check_afk_clear_trade_record_timestamp)
	{
		this->CheckUserAFKClearTradeRecord(now);
		m_next_check_afk_clear_trade_record_timestamp = now + RandomNum(60, 100);
	}
	
}

void UserTradeInfo::UpdateSaleItemList(unsigned int now)
{
	bool is_true = false;
	for (int i = 0; i < USER_SALE_ITEM_MAX_NUM; ++i)
	{
		if (NULL == m_sale_list[i]) continue;

		is_true = false;
		SaleItemParam& param = m_sale_list[i]->GetSaleItemParam();
		if (param.cur_status <= SALE_ITEM_STATUS_INVALID || param.cur_status >= SALE_ITEM_STATUS_MAX) continue;
		if (param.cur_status >= SALE_ITEM_STATUS_OFF_SHELVE) continue;
		if (0 == param.next_status_time) continue;

		const ItemBase* item = ITEMPOOL->GetItem(param.item_wrapper.item_id);
		if (NULL == item || item->GetTradeNumPerGroup() != param.num_per_group)
		{
			// 找不到了物品，或group配置修改，则立刻下架
			int prev_status = param.cur_status;
			param.next_status_time = 0u;
			param.cur_status = SALE_ITEM_STATUS_OFF_SHELVE;
			if (SALE_ITEM_STATUS_ON_SALE == prev_status)
			{
				TradeMarketManager::Instance().GetSaleItemIndex().OnRemoveItemOnSale(m_sale_list[i]);
			}
			else if (SALE_ITEM_STATUS_ANNOUNCEMENT == prev_status)
			{
				TradeMarketManager::Instance().GetSaleItemIndex().OnRemoveItemOnAnnounce(m_sale_list[i]);
			}
			this->SetDirty(i);
		}

		if (now > param.next_status_time)
		{
			switch (param.cur_status)
			{
			case SALE_ITEM_STATUS_ON_SHELVE:
				{
					if (param.is_need_announcement)
					{
						param.next_status_time = now + TradeMarketConfig::Instance().GetConstantCfg().publicity_time_s;
						param.cur_status = SALE_ITEM_STATUS_ANNOUNCEMENT;

						// 开始公示，把货品加到公示索引中
						TradeMarketManager::Instance().GetSaleItemIndex().OnAddItemOnAnnounce(m_sale_list[i]);
						TradeMarketManager::Instance().SendSystemNotice(true, m_sale_list[i]);
					}
					else
					{
						param.next_status_time = now + TRADE_ITEM_ON_SALE_TIME_S;
						param.cur_status = SALE_ITEM_STATUS_ON_SALE;

						// 正式上架，把货品加到货品索引中
						TradeMarketManager::Instance().GetSaleItemIndex().OnAddItemOnSale(m_sale_list[i]);
						is_true = true;
					}
				}
				break;

			case SALE_ITEM_STATUS_ANNOUNCEMENT:
				{
					param.next_status_time = now + TRADE_ITEM_ON_SALE_TIME_S;
					param.cur_status = SALE_ITEM_STATUS_ON_SALE;

					// 公示转为上架
					TradeMarketManager::Instance().GetSaleItemIndex().OnRemoveItemOnAnnounce(m_sale_list[i]);
					TradeMarketManager::Instance().GetSaleItemIndex().OnAddItemOnSale(m_sale_list[i]);	
					TradeMarketManager::Instance().SendSystemNotice(false, m_sale_list[i]);
					is_true = true;
				}
				break;

			case SALE_ITEM_STATUS_ON_SALE:
				{
					param.next_status_time = 0u;
					param.cur_status = SALE_ITEM_STATUS_OFF_SHELVE;

					TradeMarketManager::Instance().GetSaleItemIndex().OnRemoveItemOnSale(m_sale_list[i]);

					LOG_QUICK12(gamelog::g_log_quick,
						LOG_TYPE_TRADE_MARTKET_EXPIRED, m_sale_list[i]->GetSellerUid(), m_sale_list[i]->GetSellerName(), NULL,
						param.item_wrapper.item_id, param.item_wrapper.num, NULL, NULL,
						0, param.gold_price, 0, 0, 0);
				}
				break;

			default:
				{
					continue;
				}
				break;
			}

			this->SetDirty(i);

			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(m_role_id));
			if (NULL != role)
			{
				if(is_true) EventHandler::Instance().OnTradeMarketPutawayHour(role);
			}

			TradeMarketManager::Instance().SendShelveChangeNotice(m_role_id, i, Protocol::SCTradeMarketUserShelveData::REASON_NOTIFY_CHANGE);
		}
	}
}

void UserTradeInfo::UpdateFocusItemList(unsigned int now)
{
	// 凡是状态转变、售罄、下架的，直接从关注列表中删掉

	for (int i = 0; i < TRADE_MARKET_USER_FOCUS_MAX_NUM; ++i)
	{
		if (NULL == m_focus_list[i]) continue;

		SaleItemParam& param = m_focus_list[i]->GetSaleItemParam();

		if (now > param.next_status_time)
		{
			this->OnRemoveFocusItem(i);
			continue;
		}

		unsigned int last_update_time = m_focus_list[i]->GetLastUpdateFocusTime();
		if (last_update_time + TRADE_MARKET_UPDATE_FOCUS_INTERVAL_S > now)
		{
			int seller_uid = m_focus_list[i]->GetSellerUid();
			UserTradeInfo* seller_info = TradeMarketManager::Instance().GetUserTradeInfo(seller_uid);
			if (NULL == seller_info)
			{
				this->OnRemoveFocusItem(i);
				continue;
			}
			
			int sale_index = m_focus_list[i]->GetSaleItemParamConst().sale_index;
			const UserSaleItem* seller_usi = seller_info->GetSaleItem(sale_index);
			if (NULL == seller_usi || SALE_ITEM_STATUS_OFF_SHELVE == seller_usi->GetSaleItemParamConst().cur_status)
			{
				this->OnRemoveFocusItem(i);
				continue;
			}

			if (seller_usi->GetSaleItemParamConst().cur_status != param.cur_status)
			{
				this->OnRemoveFocusItem(i);
				continue;
			}

			this->UpdateFocusItem(i, seller_usi->GetSaleItemParamConst());	
			m_focus_list[i]->OnUpdateFocus(now);
			this->SetFocusDirty(i);
		}
	}
}

bool UserTradeInfo::InitSaleItem(const TradeMarketData::SaleItem &saleitem)
{
	if (!IsIndexValid(saleitem.index))
	{
		gamelog::g_log_serious_error.printf(LL_CRITICAL, "UserTradeInfo::InitSaleItem invalid index[%d], item[%d,%d] belongs to[%d,%s], unique_key[%lld]",
			saleitem.index, saleitem.salitem_param.item_wrapper.item_id,
			saleitem.salitem_param.item_wrapper.num, saleitem.seller_uid, saleitem.seller_name, saleitem.salitem_param.unique_key);
		return false;
	}

	if (NULL != m_sale_list[saleitem.index])
	{
		gamelog::g_log_serious_error.printf(LL_CRITICAL, "UserTradeInfo::InitSaleItem index duplicate[%d], item[%d,%d] belongs to[%d,%s], unique_key[%lld]",
			saleitem.index, saleitem.salitem_param.item_wrapper.item_id,
			saleitem.salitem_param.item_wrapper.num, saleitem.seller_uid, saleitem.seller_name, saleitem.salitem_param.unique_key);
		return false;
	}

	if (!this->OnAddSaleItem(saleitem.index, saleitem.seller_uid, saleitem.seller_name, saleitem.salitem_param, true))
	{
		gamelog::g_log_serious_error.printf(LL_CRITICAL, "UserTradeInfo::InitSaleItem calls OnAddSaleItem failed, saleindex[%d], item[%d,%d] belongs to[%d,%s], unique_key[%lld]",
			saleitem.index, saleitem.salitem_param.item_wrapper.item_id,
			saleitem.salitem_param.item_wrapper.num, saleitem.seller_uid, saleitem.seller_name, saleitem.salitem_param.unique_key);
		return false;
	}

	m_old_state[saleitem.salitem_param.sale_index] = true;

	unsigned int now_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	m_next_check_status_time = now_time;
	m_next_save_db_time = now_time + USER_TRADE_INFO_SAVE_DB_INTERVAL_S;

	return true;
}

bool UserTradeInfo::InitFocusItem(const TradeMarketData::SaleItem &focusitem)
{
	if (!IsFocusIndexValid(focusitem.index))
	{
		gamelog::g_log_serious_error.printf(LL_CRITICAL, "UserTradeInfo::InitFocusItem invalid index[%d], data_owner_uid[%d], item[%d,%d] belongs to[%d,%s] at sale_index[%d], unique_key[%lld]",
			focusitem.index, focusitem.owner_uid, focusitem.salitem_param.item_wrapper.item_id,
			focusitem.salitem_param.item_wrapper.num, focusitem.seller_uid, focusitem.seller_name, focusitem.salitem_param.sale_index, focusitem.salitem_param.unique_key);
		return false;
	}

	if (NULL != m_focus_list[focusitem.index])
	{
		gamelog::g_log_serious_error.printf(LL_CRITICAL, "UserTradeInfo::InitFocusItem index duplicate[%d], data_owner_uid[%d], item[%d,%d] belongs to[%d,%s] at sale_index[%d], unique_key[%lld]",
			focusitem.index, focusitem.owner_uid, focusitem.salitem_param.item_wrapper.item_id,
			focusitem.salitem_param.item_wrapper.num, focusitem.seller_uid, focusitem.seller_name, focusitem.salitem_param.sale_index, focusitem.salitem_param.unique_key);
		return false;
	}

	if (!this->OnAddFocusItem(focusitem.index, focusitem.seller_uid, focusitem.seller_name, focusitem.salitem_param, true))
	{
		gamelog::g_log_serious_error.printf(LL_CRITICAL, "UserTradeInfo::InitFocusItem calls OnAddFocusItem failed, data_owner_uid[%d], focusindex[%d] item[%d,%d] belongs to[%d,%s] at sale_index[%d], unique_key[%lld]",
			focusitem.owner_uid, focusitem.index, focusitem.salitem_param.item_wrapper.item_id,
			focusitem.salitem_param.item_wrapper.num, focusitem.seller_uid, focusitem.seller_name, focusitem.salitem_param.sale_index, focusitem.salitem_param.unique_key);
		return false;
	}

	m_focus_old_state[focusitem.index] = true;

	unsigned int now_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	m_next_check_status_time = now_time;
	m_next_save_db_time = now_time + USER_TRADE_INFO_SAVE_DB_INTERVAL_S;

	return true;
}

bool UserTradeInfo::InitRoleRecordParam(const RoleTransactionRecordParam& param, const NewUserTradeRecordParam& sold_record, const NewUserTradeRecordParam& buy_record)
{
	if (m_record_old_state)
	{
		gamelog::g_log_trade_market.printf(LL_ERROR, "UserTradeInfo::InitRoleRecordParam role[%d]", m_role_id);

#ifdef _DEBUG
		assert(0);
#else
		return false;
#endif
	}

	m_other_param = param;
	
	if (NULL == m_sold_record)
	{
		m_sold_record = new UserTradeRecord();
	}
	m_sold_record->InitTradeRecord(sold_record);

	if (NULL == m_buy_record)
	{
		m_buy_record = new UserTradeRecord();
	}
	m_buy_record->InitTradeRecord(buy_record);

	m_record_old_state = true;

	return true;
}

bool UserTradeInfo::OnAddSaleItem(int index, int seller_uid, const char* seller_name, const SaleItemParam& param, bool is_init)
{
	int sale_index = index;
	if (!IsIndexValid(sale_index)) return false;

	if (m_sale_list[sale_index] != NULL) return false;

	UserSaleItem* user_sale_item = new UserSaleItem();
	user_sale_item->Init(seller_uid, seller_name, param, false);
	user_sale_item->GetSaleItemParam().sale_index = sale_index;

	m_sale_list[sale_index] = user_sale_item;
	if (!is_init)
	{
		this->SetDirty(sale_index);
	}
	else
	{
		const ItemBase* item = ITEMPOOL->GetItem(user_sale_item->GetSaleItemParam().item_wrapper.item_id);
		if (NULL != item && I_TYPE_PET == item->GetItemType())
		{
			// 检查宠物数据版本变更
			NetValueItemParam temp_param;
			temp_param.SetData(user_sale_item->GetSaleItemParam().item_wrapper.param_data);
			if (temp_param.pet_param.CheckGrowthVersion(user_sale_item->GetSaleItemParam().item_wrapper.item_unique_id, __FUNCTION__))
			{
				temp_param.GetData(user_sale_item->GetSaleItemParam().item_wrapper.param_data);
				this->SetDirty(index);
			}

			if (temp_param.pet_param.CheckModifyVersion())
			{
				temp_param.GetData(user_sale_item->GetSaleItemParam().item_wrapper.param_data);
				this->SetDirty(index);
			}
		}
		else if (NULL != item && I_TYPE_MEDAL == item->GetItemType())
		{
			NetValueItemParam temp_param;
			temp_param.SetData(user_sale_item->GetSaleItemParam().item_wrapper.param_data);
			if (temp_param.medal_param.CheckModifyVersion())
			{
				temp_param.GetData(user_sale_item->GetSaleItemParam().item_wrapper.param_data);
				this->SetDirty(index);
			}
		}
		else if (NULL != item && I_TYPE_EQUIPMENT == item->GetItemType())
		{
			NetValueItemParam temp_param;
			temp_param.SetData(user_sale_item->GetSaleItemParam().item_wrapper.param_data);
			if (temp_param.equip_param.CheckModifyVersion())
			{
				temp_param.GetData(user_sale_item->GetSaleItemParam().item_wrapper.param_data);
				this->SetDirty(index);
			}
		}
		else if (NULL != item && I_TYPE_JEWELRY == item->GetItemType())
		{
			NetValueItemParam temp_param;
			temp_param.SetData(user_sale_item->GetSaleItemParam().item_wrapper.param_data);
			if (temp_param.jewelry_param.CheckModifyVersion())
			{
				temp_param.GetData(user_sale_item->GetSaleItemParam().item_wrapper.param_data);
				this->SetDirty(index);
			}
		}

		if (NULL == item || item->GetTradeNumPerGroup() != user_sale_item->GetSaleItemParamConst().num_per_group
			|| !item->CanTrade() 
			|| user_sale_item->GetSaleItemParamConst().gold_price > TradeMarketManager::GetTradeMaxPrice(item, &user_sale_item->GetSaleItemParamConst().item_wrapper)
			|| user_sale_item->GetSaleItemParamConst().gold_price < TradeMarketManager::GetTradeMinPrice(item, &user_sale_item->GetSaleItemParamConst().item_wrapper))
		{
			if (NULL == item)
			{
				gamelog::g_log_trade_market.printf(LL_INFO, "OnAddSaleItem Item Null Error is_init[%s] role[%d,%s] item_id[%d] unique_key[%lld]",
					is_init ? "true" : "false", seller_uid, seller_name, param.item_wrapper.item_id, param.unique_key);
			}
			else if (item->GetTradeNumPerGroup() != user_sale_item->GetSaleItemParamConst().num_per_group)
			{
				gamelog::g_log_trade_market.printf(LL_INFO, "OnAddSaleItem NumPerGroup Error is_init[%s] role[%d,%s] item_id[%d] num_per_group_cfg[%d] num_per_group_actual[%d] unique_key[%lld]",
					is_init ? "true" : "false", seller_uid, seller_name, param.item_wrapper.item_id, item->GetTradeNumPerGroup(), param.num_per_group, param.unique_key);
			}
			else if (!item->CanTrade())
			{
				gamelog::g_log_trade_market.printf(LL_INFO, "OnAddSaleItem Item Cannot Trade Error is_init[%s] role[%d,%s] item_id[%d] num_per_group_cfg[%d] num_per_group_actual[%d] unique_key[%lld]",
					is_init ? "true" : "false", seller_uid, seller_name, param.item_wrapper.item_id, item->GetTradeNumPerGroup(), param.num_per_group, param.unique_key);
			}
			else if (user_sale_item->GetSaleItemParamConst().gold_price > TradeMarketManager::Instance().GetTradeMaxPrice(item, &param.item_wrapper) ||
				user_sale_item->GetSaleItemParamConst().gold_price < TradeMarketManager::Instance().GetTradeMaxPrice(item, &param.item_wrapper))
			{
				gamelog::g_log_trade_market.printf(LL_INFO, "OnAddSaleItem Item Price Range Error is_init[%s] role[%d,%s] item_id[%d] num_per_group_cfg[%d] num_per_group_actual[%d] unique_key[%lld]",
					is_init ? "true" : "false", seller_uid, seller_name, param.item_wrapper.item_id, item->GetTradeNumPerGroup(), param.num_per_group, param.unique_key);
			}

			// 没有这个物品了，或group配置修改了，则强行下架
			user_sale_item->GetSaleItemParam().next_status_time = 0u;
			user_sale_item->GetSaleItemParam().cur_status = SALE_ITEM_STATUS_OFF_SHELVE;
			this->SetDirty(sale_index);
		}
	}

	gamelog::g_log_trade_market.printf(LL_INFO, "UserTradeInfo::OnAddSaleItem is_init[%s] role[%d,%s] sale_index[%d] cur_status[%d] gold_price[%d] immortal_price[%d] item_id[%d] num[%d] num_per_group[%d] item_search_type[%d] unique_key[%lld] item_param[%s]",
		is_init ? "true" : "false", seller_uid, seller_name, param.sale_index, param.cur_status, param.gold_price, param.immortal_coin_price,
		param.item_wrapper.item_id, param.item_wrapper.num, param.num_per_group, param.item_search_type, param.unique_key,
		NetValueItemParam::GetParamLog(param.item_wrapper.item_id, param.item_wrapper.param_data));

	if (is_init)
	{
		// 正式上架，把货品加到货品索引中
		if (SALE_ITEM_STATUS_ON_SALE == user_sale_item->GetSaleItemParam().cur_status)
		{
			TradeMarketManager::Instance().GetSaleItemIndex().OnAddItemOnSale(m_sale_list[sale_index]);
		}
		else if (SALE_ITEM_STATUS_ANNOUNCEMENT == user_sale_item->GetSaleItemParam().cur_status)
		{
			TradeMarketManager::Instance().GetSaleItemIndex().OnAddItemOnAnnounce(m_sale_list[sale_index]);
		}
	}

	return true;
}

bool UserTradeInfo::OnAddFocusItem(int index, int seller_uid, const char* seller_name, const SaleItemParam& param, bool is_init)
{
	if (!IsFocusIndexValid(index)) return false;

	if (m_focus_list[index] != NULL) return false;

	UserSaleItem* user_sale_item = new UserSaleItem();
	user_sale_item->Init(seller_uid, seller_name, param, true);

	m_focus_list[index] = user_sale_item;
	if (!is_init)
	{
		this->SetFocusDirty(index);
	}

	gamelog::g_log_trade_market.printf(LL_INFO, "UserTradeInfo::OnAddFocusItem is_init[%s] owner[%d] focus_index[%d] seller[%d,%s] "
		"gold_price[%d] immortal_price[%d] item_id[%d] num[%d] num_per_group[%d] unique_key[%lld]", 
		is_init ? "true" : "false", m_role_id, index, seller_uid, seller_name,
		param.gold_price, param.immortal_coin_price, param.item_wrapper.item_id, param.item_wrapper.num, param.num_per_group, param.unique_key);
	return true;
}

bool UserTradeInfo::OnRemoveSaleItem(int sale_index, const char* reason)
{
	if (!IsIndexValid(sale_index)) return false;

	if (NULL == m_sale_list[sale_index]) return false;

	{
		//log
		SaleItemParam& param = m_sale_list[sale_index]->GetSaleItemParam();
		gamelog::g_log_trade_market.printf(LL_INFO, "UserTradeInfo::OnRemoveSaleItem reason[%s] role[%d,%s] sale_index[%d] "
		"gold_price[%d] immortal_price[%d] item_id[%d] num[%d] num_per_group[%d] item_search_type[%d] unique_key[%lld] item_param[%s]", 
			reason, m_sale_list[sale_index]->GetSellerUid(), m_sale_list[sale_index]->GetSellerName(), param.sale_index,
			param.gold_price, param.immortal_coin_price, param.item_wrapper.item_id, param.item_wrapper.num, param.num_per_group, param.item_search_type, param.unique_key,
			NetValueItemParam::GetParamLog(param.item_wrapper.item_id, param.item_wrapper.param_data));
	}

	//!< 从货品索引中删除
	if (SALE_ITEM_STATUS_ON_SALE == m_sale_list[sale_index]->GetSaleItemParamConst().cur_status)
	{
		TradeMarketManager::Instance().GetSaleItemIndex().OnRemoveItemOnSale(m_sale_list[sale_index]);
	}

	if (SALE_ITEM_STATUS_ANNOUNCEMENT == m_sale_list[sale_index]->GetSaleItemParamConst().cur_status)
	{
		TradeMarketManager::Instance().GetSaleItemIndex().OnRemoveItemOnAnnounce(m_sale_list[sale_index]);
	}

	delete m_sale_list[sale_index];
	m_sale_list[sale_index] = NULL;

	this->SetDirty(sale_index);

	return true;
}

bool UserTradeInfo::OnRemoveFocusItem(int focus_index)
{
	if (!IsFocusIndexValid(focus_index)) return false;

	if (NULL == m_focus_list[focus_index]) return false;

	{
		//log
		SaleItemParam& param = m_focus_list[focus_index]->GetSaleItemParam();
		gamelog::g_log_trade_market.printf(LL_INFO, "UserTradeInfo::OnRemoveFocusItem role_id[%d] index[%d] seller[%d,%s] sale_index[%d] \
		gold_price[%d] immortal_price[%d] item_id[%d] num[%d] num_per_group[%d] unique_key[%lld]", m_role_id, focus_index,
			m_focus_list[focus_index]->GetSellerUid(), m_focus_list[focus_index]->GetSellerName(), param.sale_index,
			param.gold_price, param.immortal_coin_price, param.item_wrapper.item_id, param.item_wrapper.num, param.num_per_group, param.unique_key);
	}

	delete m_focus_list[focus_index];
	m_focus_list[focus_index] = NULL;

	this->SetFocusDirty(focus_index);

	return true;
}

bool UserTradeInfo::OnCashOutItem(int sale_index, long long* add_gold)
{
	if (!IsIndexValid(sale_index)) return false;

	if (NULL == add_gold) return false;

	UserSaleItem* saleitem = m_sale_list[sale_index];
	if (NULL == saleitem) return false;

	if (saleitem->GetSaleItemParam().sold_num <= 0) return false;

	int num_per_group = saleitem->GetSaleItemParam().num_per_group;
	if (0 == num_per_group || saleitem->GetSaleItemParam().sold_num < num_per_group)
	{
		const ItemBase* item_base = ITEMPOOL->GetItem(saleitem->GetSaleItemParam().item_wrapper.item_id);
		if (NULL == item_base || saleitem->GetSaleItemParam().sold_num % item_base->GetTradeNumPerGroup() != 0)
		{
#ifdef _DEBUG
			assert(0);
#endif
			return false;
		}
		num_per_group = item_base->GetTradeNumPerGroup();
	}
	int sold_group_num = saleitem->GetSaleItemParam().sold_num / num_per_group;
	if (sold_group_num <= 0) return false;
	
	SaleItemParam& p = saleitem->GetSaleItemParam();
	*add_gold = 1LL * p.gold_price * sold_group_num;

	int sold_num = p.sold_num;
	p.sold_num = 0;

	this->SetDirty(sale_index);

	gamelog::g_log_trade_market.printf(LL_INFO, "UserTradeInfo::OnCashOutItem item from[%d,%s], sold_num[%d], sold_group_num[%d], remain_num[%d], add_gold[%lld], unique_key[%lld]",
		saleitem->GetSellerUid(), saleitem->GetSellerName(), sold_num, sold_group_num, p.item_wrapper.num, *add_gold, p.unique_key);

	if (p.item_wrapper.num <= 0)
	{
		this->OnRemoveSaleItem(sale_index, __FUNCTION__);
	}

	return true;
}

bool UserTradeInfo::OnItemSold(int sale_index, short sold_num, int buyer_uid, const char* buyer_name, const char* buyer_pname)
{
	if (!IsIndexValid(sale_index)) return false;

	UserSaleItem* saleitem = m_sale_list[sale_index];
	if (NULL == saleitem) return false;

	ItemID item_id = saleitem->GetSaleItemParam().item_wrapper.item_id;

	saleitem->GetSaleItemParam().sold_num += sold_num;
	saleitem->GetSaleItemParam().item_wrapper.num -= sold_num;

	this->SetDirty(sale_index);
	if (saleitem->GetSaleItemParam().item_wrapper.num <= 0)
	{
		//卖完就下架
		saleitem->GetSaleItemParam().cur_status = SALE_ITEM_STATUS_OFF_SHELVE;
		saleitem->GetSaleItemParam().next_status_time = 0u;

		//!< 从货品索引中删除
		TradeMarketManager::Instance().GetSaleItemIndex().OnRemoveItemOnSale(m_sale_list[sale_index]);
	}

	gamelog::g_log_trade_market.printf(LL_INFO, "UserTradeInfo::OnItemSold buyer[%d,%s] seller[%d,%s] sale_index[%d] item[%d,%d] sold_num[%d] total_sold_num[%d] num_per_group[%d] unique_key[%lld], item_param[%s]",
		buyer_uid, buyer_name,
		saleitem->GetSellerUid(), saleitem->GetSellerName(), sale_index, saleitem->GetSaleItemParamConst().item_wrapper.item_id,
		saleitem->GetSaleItemParamConst().item_wrapper.num, sold_num, saleitem->GetSaleItemParamConst().sold_num, 
		saleitem->GetSaleItemParamConst().num_per_group, saleitem->GetSaleItemParamConst().unique_key,
		NetValueItemParam::GetParamLog(saleitem->GetSaleItemParamConst().item_wrapper.item_id, saleitem->GetSaleItemParamConst().item_wrapper.param_data));

	TradeMarketManager::Instance().OnItemSold(item_id, saleitem->GetSaleItemParam().gold_price, sold_num, saleitem->GetSaleItemParamConst().item_wrapper, saleitem->GetSellerUid(), saleitem->GetSellerName(), buyer_name, buyer_uid, buyer_pname);
	return true;
}

bool UserTradeInfo::OnItemRedoAddOnShelve(int sale_index, int price)
{
	if (!IsIndexValid(sale_index)) return false;

	UserSaleItem* saleitem = m_sale_list[sale_index];
	if (NULL == saleitem) return false;

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	saleitem->GetSaleItemParam().cur_status = SALE_ITEM_STATUS_ON_SHELVE;
	saleitem->GetSaleItemParam().put_on_shelve_time = now;
	saleitem->GetSaleItemParam().next_status_time = now + EngineAdapter::Instance().NextHourInterval(0, 0);
	if (saleitem->GetSaleItemParam().gold_price > 0)
	{
		saleitem->GetSaleItemParam().gold_price = price;
	}
	else
	{
		saleitem->GetSaleItemParam().immortal_coin_price = price;
	}

	this->SetDirty(sale_index);

	gamelog::g_log_trade_market.printf(LL_INFO, "OnItemRedoAddOnShelve role[%d,%s] item[%d] price[%d] unique_key[%lld]",
		m_role_id, saleitem->GetSellerName(), saleitem->GetSaleItemParam().item_wrapper.item_id, price, saleitem->GetSaleItemParam().unique_key);

#ifdef TRACK_ITEM_FLAG
	gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG UserTradeInfo::OnItemRedoAddOnShelve role[%d,%s] item[%d] item_unique_id[%lld] unique_key[%lld]",
		saleitem->GetSellerUid(), saleitem->GetSellerName(), saleitem->GetSaleItemParamConst().item_wrapper.item_id, saleitem->GetSaleItemParamConst().item_wrapper.item_unique_id, 
		saleitem->GetSaleItemParamConst().unique_key);
#endif

	return true;
}

bool UserTradeInfo::IsItemAlreadyOnShelve(ItemID item_id)
{
	for (int i = 0; i < USER_SALE_ITEM_MAX_NUM; ++i)
	{
		if (NULL != m_sale_list[i])
		{
			if (item_id == m_sale_list[i]->GetSaleItemParam().item_wrapper.item_id)
			{
				return true;
			}
		}
	}
	return false;
}

bool UserTradeInfo::GetEmptyIndex(int* out_sale_index)
{
	for (int i = 0; i < USER_SALE_ITEM_MAX_NUM; ++i)
	{
		if (NULL == m_sale_list[i])
		{
			if (NULL != out_sale_index)
			{
				*out_sale_index = i;
			}
			return true;
		}
	}
	return false;
}

const UserSaleItem* UserTradeInfo::GetSaleItem(int sale_index)
{
	if (sale_index < 0 || sale_index >= USER_SALE_ITEM_MAX_NUM) return NULL;

	return m_sale_list[sale_index];
}

const UserSaleItem* UserTradeInfo::GetFocusItem(int focus_index)
{
	if (focus_index < 0 || focus_index >= TRADE_MARKET_USER_FOCUS_MAX_NUM) return NULL;

	return m_focus_list[focus_index];
}

void UserTradeInfo::UpdateFocusItem(int focus_index, const SaleItemParam& data)
{
	if (focus_index < 0 || focus_index >= TRADE_MARKET_USER_FOCUS_MAX_NUM) return;

	SaleItemParam& param = m_focus_list[focus_index]->GetSaleItemParam();
	param.cur_status = data.cur_status;
	param.next_status_time = data.next_status_time;
	param.sold_num = data.sold_num;
	param.item_wrapper.num = data.item_wrapper.num;
}

bool UserTradeInfo::GetEmptyFocusIndex(int* out_focus_index)
{
	for (int i = 0; i < TRADE_MARKET_USER_FOCUS_MAX_NUM; ++i)
	{
		if (NULL == m_focus_list[i])
		{
			if (NULL != out_focus_index)
			{
				*out_focus_index = i;
			}
			return true;
		}
	}
	return false;
}

bool UserTradeInfo::IsIndexValid(int sale_index)
{
	return sale_index >= 0 && sale_index < USER_SALE_ITEM_MAX_NUM;
}

bool UserTradeInfo::IsFocusIndexValid(int focus_index)
{
	return focus_index >= 0 && focus_index < TRADE_MARKET_USER_FOCUS_MAX_NUM;
}

void UserTradeInfo::SetDirty(int sale_index)
{
	// 由外部调用者确保sale_index合法性
	m_dirty_mark[sale_index] = true;
	m_is_change = true;
}

void UserTradeInfo::SetFocusDirty(int focus_index)
{
	m_focus_dirty_mark[focus_index] = true;
	m_focus_is_change = true;
}

void UserTradeInfo::ClearDirtyMark()
{
	for (int i = 0; i < USER_SALE_ITEM_MAX_NUM; ++i)
	{
		m_dirty_mark[i] = false;
		m_old_state[i] = (NULL != m_sale_list[i]);
	}
	m_is_change = false;

	for (int i = 0; i < TRADE_MARKET_USER_FOCUS_MAX_NUM; ++i)
	{
		m_focus_dirty_mark[i] = false;
		m_focus_old_state[i] = (NULL != m_focus_list[i]);
	}
	m_focus_is_change = false;
}

void UserTradeInfo::AddSoldRecord(ItemID item_id, short sold_num, short price_type, int price, const ItemDataWrapper& wrapper)
{
	if (NULL == m_sold_record)
	{
		m_sold_record = new UserTradeRecord();
	}
	m_sold_record->AddRecord(item_id, sold_num, price_type, price, wrapper);

	m_other_param.total_sold_gold += price * sold_num;

	m_is_record_change = true;
}

void UserTradeInfo::AddBuyRecord(ItemID item_id, short buy_num, short price_type, int price, const ItemDataWrapper& wrapper)
{
	if (NULL == m_buy_record)
	{
		m_buy_record = new UserTradeRecord();
	}
	m_buy_record->AddRecord(item_id, buy_num, price_type, price, wrapper);

	m_other_param.total_buy_gold += price * buy_num;

	m_is_record_change = true;
}

void UserTradeInfo::GMSetNextStatusTime()
{
	for (int i = 0; i < USER_SALE_ITEM_MAX_NUM; ++i)
	{
		if (NULL == m_sale_list[i]) continue;

		SaleItemParam& param = m_sale_list[i]->GetSaleItemParam();
		if (param.cur_status <= SALE_ITEM_STATUS_INVALID || param.cur_status >= SALE_ITEM_STATUS_MAX) continue;
		if (param.cur_status >= SALE_ITEM_STATUS_OFF_SHELVE) continue;
		if (0 == param.next_status_time) continue;

		param.next_status_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	}
}

int UserTradeInfo::ItemRecover(int role_id, ItemID item_id)
{
	int temp_recover_num = 0;
	for (int i = 0; i < USER_SALE_ITEM_MAX_NUM; ++i)
	{
		UserSaleItem* usi = this->MutableSaleItem(i);
		if (NULL == usi) continue;

		SaleItemParam& param = usi->GetSaleItemParam();
		if (item_id != param.item_wrapper.item_id) continue;

		if (param.item_wrapper.num > 0)
		{
			gamelog::g_log_trade_market.printf(LL_INFO, "UserTradeInfo::ItemRecover__1 role[%d] item_id[%d] num[%d] unique_id[%lld]",
				this->GetRoleID(), item_id, param.item_wrapper.num, param.item_wrapper.item_unique_id);

			temp_recover_num += param.item_wrapper.num;
			param.item_wrapper.num = 0;
		}

		if (param.sold_num <= 0)
		{
			// 没有卖出过就直接Remove
			this->OnRemoveSaleItem(i, __FUNCTION__);
			TradeMarketManager::Instance().SendShelveChangeNotice(role_id, i, Protocol::SCTradeMarketUserShelveData::REASON_NOTIFY_REMOVE);
		}
		else
		{
			// 卖出过就改为下架状态
			if (param.cur_status == SALE_ITEM_STATUS_ON_SALE)
			{
				TradeMarketManager::Instance().GetSaleItemIndex().OnRemoveItemOnSale(usi);
			}
			else if (usi->GetSaleItemParam().cur_status == SALE_ITEM_STATUS_ANNOUNCEMENT)
			{
				TradeMarketManager::Instance().GetSaleItemIndex().OnRemoveItemOnAnnounce(usi);
			}

			param.cur_status = SALE_ITEM_STATUS_OFF_SHELVE;
			param.next_status_time = 0u;

			this->SetDirty(i);

			TradeMarketManager::Instance().SendShelveChangeNotice(role_id, i, Protocol::SCTradeMarketUserShelveData::REASON_NOTIFY_CHANGE);
		}		
	}

	gamelog::g_log_trade_market.printf(LL_INFO, "UserTradeInfo::ItemRecover__2 role[%d] item_id[%d] temp_recover_num[%d]",
		this->GetRoleID(), item_id, temp_recover_num);

	return temp_recover_num;
}

static TradeMarketData g_user_sale_data;
bool UserTradeInfo::SaveDB()
{
	g_user_sale_data.count = 0;
	g_user_sale_data.is_system_shelve = false;
	this->GetChangeSaleItemList(&g_user_sale_data);

	if (g_user_sale_data.count <= 0) return true;

	RMISaveTradeMarketDataBackObjectImpl *impl = new RMISaveTradeMarketDataBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.SaveTradeMarketDataAsync(g_user_sale_data, impl);
}


bool UserTradeInfo::SaveRecord()
{
	if (!m_is_record_change)
	{
		return false;
	}

	static UserTradeRecordData record_data;
	record_data.count = 1;
	record_data.record_list[0].id = 0;
	record_data.record_list[0].role_id = m_role_id;
	record_data.record_list[0].other_param = m_other_param;
	record_data.record_list[0].old_state = m_record_old_state;
	if (NULL != m_sold_record)
	{
		m_sold_record->GetTradeRecordParam(&record_data.record_list[0].sold_record);
	}
	else
	{
		record_data.record_list[0].sold_record.Reset();
	}
	if (NULL != m_buy_record)
	{
		m_buy_record->GetTradeRecordParam(&record_data.record_list[0].buy_record);
	}
	else
	{
		record_data.record_list[0].buy_record.Reset();
	}

	RMISaveUserTradeRecordBackObjectImpl* impl = new RMISaveUserTradeRecordBackObjectImpl();
	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.SaveTradeMarketUserRecordAsync(record_data, impl);

	return true;
}

void RMISaveTradeMarketDataBackObjectImpl::SaveRet(int ret)
{
	gamelog::g_log_trade_market.printf(LL_INFO, "RMISaveTradeMarketDataBackObjectImpl::SaveRet ret[%d]", ret);
}

void RMISaveUserTradeRecordBackObjectImpl::SaveRet(int ret)
{
	gamelog::g_log_trade_market.printf(LL_INFO, "RMISaveTradeMarketUserRecordBackObjectImpl::SaveRet ret[%d]", ret);
}

void RMISaveSystemTradeRecordBackObjectImpl::SaveRet(int ret)
{
	gamelog::g_log_trade_market.printf(LL_INFO, "RMISaveSystemTradeRecordBackObjectImpl::SaveRet ret[%d]", ret);
}

void UserTradeInfo::GetChangeSaleItemList(TradeMarketData* data)
{
	if (!m_is_change)
	{
		data->count = 0;
		return;
	}

	int count = 0;
	for (int i = 0; i < USER_SALE_ITEM_MAX_NUM; ++i)
	{
		if (!m_dirty_mark[i]) continue;

		if (NULL != m_sale_list[i])
		{
			//update or insert
			data->sale_list[count].change_state = (m_old_state[i] ? structcommon::CS_UPDATE : structcommon::CS_INSERT);
			
			data->sale_list[count].is_focus = 0;
			data->sale_list[count].index = i;
			data->sale_list[count].owner_uid = m_role_id;

			data->sale_list[count].seller_uid = m_sale_list[i]->GetSellerUid();
			F_STRNCPY(data->sale_list[count].seller_name, m_sale_list[i]->GetSellerName(), sizeof(GameName));
			data->sale_list[count].salitem_param = m_sale_list[i]->GetSaleItemParam();
		}
		else
		{
			//delete
			if (!m_old_state[i]) continue;

			data->sale_list[count].change_state = structcommon::CS_DELETE;
			
			data->sale_list[count].is_focus = 0;
			data->sale_list[count].index = i;
			data->sale_list[count].owner_uid = m_role_id;
		}

		++count;
	}

	for (int i = 0; i < TRADE_MARKET_USER_FOCUS_MAX_NUM; ++i)
	{
		if (!m_focus_dirty_mark[i]) continue;

		if (NULL != m_focus_list[i])
		{
			//update or insert
			data->sale_list[count].change_state = (m_focus_old_state[i] ? structcommon::CS_UPDATE : structcommon::CS_INSERT);
			
			data->sale_list[count].is_focus = 1;
			data->sale_list[count].index = i;
			data->sale_list[count].owner_uid = m_role_id;

			data->sale_list[count].seller_uid = m_focus_list[i]->GetSellerUid();
			F_STRNCPY(data->sale_list[count].seller_name, m_focus_list[i]->GetSellerName(), sizeof(GameName));
			data->sale_list[count].salitem_param = m_focus_list[i]->GetSaleItemParam();
		}
		else
		{
			//delete
			if (!m_focus_old_state[i]) continue;

			data->sale_list[count].change_state = structcommon::CS_DELETE;
			
			data->sale_list[count].is_focus = 1;
			data->sale_list[count].index = i;
			data->sale_list[count].owner_uid = m_role_id;
		}

		++count;
	}

	data->count = count;

	return;
}

void UserTradeInfo::CheckUserAFKClearTradeRecord(unsigned int now)
{
	if (NULL == m_sold_record && NULL == m_buy_record) return;

	UserCacheNode* node = UserCacheManager::Instance().GetUserNode(m_role_id, false);
	if (NULL == node) return;
	if (node->last_online_timestamp + 3 * SECOND_PER_DAY > now) return;

	gamelog::g_log_trade_market.printf(LL_INFO, "CheckUserAFKClearTradeRecord role[%d,%s] last_online_timestamp[%u]",
		node->uid, node->role_name, node->last_online_timestamp);

	// 3天以上没登录 清掉记录

	if (NULL != m_sold_record)
	{
		this->MakeSoldRecordLog();
		delete m_sold_record;
		m_sold_record = NULL;
	}

	if (NULL != m_buy_record)
	{
		this->MakeBuyRecordLog();
		delete m_buy_record;
		m_buy_record = NULL;
	}
}

void UserTradeInfo::MakeSoldRecordLog()
{
	assert(m_sold_record);
	if (NULL == m_sold_record) return;

	this->MakeRecordLog("MakeSoldRecordLog", m_sold_record->GetRecordQueue());
}

void UserTradeInfo::MakeBuyRecordLog()
{
	assert(m_buy_record);
	if (NULL == m_buy_record) return;

	this->MakeRecordLog("MakeBuyRecordLog", m_buy_record->GetRecordQueue());
}

void UserTradeInfo::MakeRecordLog(const char* from, const UserTradeRecord::RecordQueue& record_queue)
{
	gamelog::g_log_trade_market.buff_printf("MakeRecordLog role[%d] from[%s] log=[", m_role_id, from);
	UserTradeRecord::RecordQueue::const_iterator it = record_queue.begin();
	for (; it != record_queue.end(); ++it)
	{
		const TransactionInfo& info = *it;
		gamelog::g_log_trade_market.buff_printf("{item_id:%d num:%d price:%d param:%s},",
			info.item_id, info.item_num, info.price, NetValueItemParam::GetParamLog(info.item_id, info.wrapper.param_data));
	}
	gamelog::g_log_trade_market.buff_print("]");
	gamelog::g_log_trade_market.commit_buff(LL_INFO);
}

UserSaleItem* UserTradeInfo::MutableSaleItem(int sale_index)
{
	if (sale_index < 0 || sale_index >= USER_SALE_ITEM_MAX_NUM) return NULL;

	return m_sale_list[sale_index];
}
