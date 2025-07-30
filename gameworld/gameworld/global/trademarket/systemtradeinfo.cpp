#include "systemtradeinfo.hpp"
#include "systemsaleitem.hpp"
#include "item/itembase.h"
#include "trademarketmanager.hpp"
#include "gamelog.h"
#include "engineadapter.h"
#include "internalcomm.h"
#include "trademarketconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/pet/petconfig.hpp"

SystemTradeInfo::SystemTradeInfo() : m_is_change(false), m_next_save_db_time(0u), m_next_check_status_time(0u)
{

}

SystemTradeInfo::~SystemTradeInfo()
{
	SystemTradeItemMap::iterator it = m_trade_item_map.begin();
	for (; it != m_trade_item_map.end(); ++it)
	{
		SystemTradeItemInfo& info = it->second;
		if (NULL != info.sale_item_ptr)
		{
			delete info.sale_item_ptr;
			info.sale_item_ptr = NULL;
		}
	}
}

void SystemTradeInfo::Update(time_t now)
{
	unsigned int now_time = static_cast<unsigned int>(now);
	if (now_time > m_next_save_db_time && this->IsChanged())
	{
		if (this->SaveDB())
		{
			m_next_save_db_time = now_time + USER_TRADE_INFO_SAVE_DB_INTERVAL_S;
		}
	}

	if (now_time > m_next_check_status_time)
	{
		m_next_check_status_time = now_time + USER_TRADE_INFO_CHECK_STATUS_INTERVAL_S;

		this->UpdateSaleItemList(static_cast<unsigned int>(now));
	}
}

void SystemTradeInfo::UpdateSaleItemList(unsigned int now)
{
	SystemTradeItemMap::iterator it = m_trade_item_map.begin();
	for (; it != m_trade_item_map.end(); ++it)
	{
		SystemTradeItemInfo& info = it->second;
		if (NULL == info.sale_item_ptr) continue;

		SaleItemParam& param = info.sale_item_ptr->GetSaleItemParam();
		if (param.cur_status <= SALE_ITEM_STATUS_INVALID || param.cur_status >= SALE_ITEM_STATUS_MAX) continue;
		if (param.cur_status >= SALE_ITEM_STATUS_OFF_SHELVE) continue;
		if (0 == param.next_status_time) continue;

		const ItemBase* item = ITEMPOOL->GetItem(param.item_wrapper.item_id);
		if (NULL == item || item->GetTradeNumPerGroup() != param.num_per_group
			|| !item->CanTrade())
		{
			// 找不到了物品，或group配置修改，则立刻下架
			int prev_status = param.cur_status;
			param.next_status_time = 0u;
			param.cur_status = SALE_ITEM_STATUS_OFF_SHELVE;
			if (SALE_ITEM_STATUS_ON_SALE == prev_status)
			{
				TradeMarketManager::Instance().GetSaleItemIndex().OnRemoveItemOnSale(info.sale_item_ptr);
			}
			else if (SALE_ITEM_STATUS_ANNOUNCEMENT == prev_status)
			{
				TradeMarketManager::Instance().GetSaleItemIndex().OnRemoveItemOnAnnounce(info.sale_item_ptr);
			}
			info.is_dirty = true;
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
						TradeMarketManager::Instance().GetSaleItemIndex().OnAddItemOnAnnounce(info.sale_item_ptr);
						TradeMarketManager::Instance().SendSystemNotice(true, info.sale_item_ptr);
					}
					else
					{
						param.next_status_time = now + TRADE_ITEM_ON_SALE_TIME_S;
						param.cur_status = SALE_ITEM_STATUS_ON_SALE;
						if (param.system_buy_minute_num > 0)
						{
							param.system_buy_timestamp = now + 60 * param.system_buy_minute_num;
						}
						if (param.system_offshelve_minute_num > 0)
						{
							param.system_offshelve_timestamp = now + 60 * param.system_offshelve_minute_num;
						}
						
						// 正式上架，把货品加到货品索引中
						TradeMarketManager::Instance().GetSaleItemIndex().OnAddItemOnSale(info.sale_item_ptr);
					}
				}
				break;

			case SALE_ITEM_STATUS_ANNOUNCEMENT:
				{
					param.next_status_time = now + TRADE_ITEM_ON_SALE_TIME_S;
					param.cur_status = SALE_ITEM_STATUS_ON_SALE;

					if (param.system_buy_minute_num > 0)
					{
						param.system_buy_timestamp = now + 60 * param.system_buy_minute_num;
					}
					if (param.system_offshelve_minute_num > 0)
					{
						param.system_offshelve_timestamp = now + 60 * param.system_offshelve_minute_num;
					}

					// 公示转为上架
					TradeMarketManager::Instance().GetSaleItemIndex().OnRemoveItemOnAnnounce(info.sale_item_ptr);
					TradeMarketManager::Instance().GetSaleItemIndex().OnAddItemOnSale(info.sale_item_ptr);
					TradeMarketManager::Instance().SendSystemNotice(false, info.sale_item_ptr);
				}
				break;

			case SALE_ITEM_STATUS_ON_SALE:
				{
					param.next_status_time = 0u;
					param.cur_status = SALE_ITEM_STATUS_OFF_SHELVE;

					TradeMarketManager::Instance().GetSaleItemIndex().OnRemoveItemOnSale(info.sale_item_ptr);
				}
				break;

			default:
				{
					continue;
				}
				break;
			}

			info.is_dirty = true;
			m_is_change = true;
		}
	}
}

bool SystemTradeInfo::InitSaleItem(const TradeMarketData::SaleItem &saleitem)
{
	const SystemTradeItemInfo* info = this->GetTradeItemInfo(saleitem.salitem_param.unique_key);
	if (NULL != info)
	{
		gamelog::g_log_serious_error.printf(LL_CRITICAL, "SystemTradeInfo::InitSaleItem unique_key duplicate[%lld], item[%d,%d]",
			saleitem.salitem_param.unique_key, saleitem.salitem_param.item_wrapper.item_id,
			saleitem.salitem_param.item_wrapper.num);
		return false;
	}

	if (SALE_ITEM_STATUS_OFF_SHELVE == saleitem.salitem_param.cur_status)
	{
		gamelog::g_log_trade_market.printf(LL_INFO, "SystemTradeInfo::InitSaleItem Already OffShelve, unique_key[%lld]", saleitem.unique_key);
		return false;
	}

	if (!this->OnAddSaleItem(saleitem.unique_key, saleitem.seller_uid, saleitem.seller_name, saleitem.salitem_param, true))
	{
		gamelog::g_log_serious_error.printf(LL_CRITICAL, "SystemTradeInfo::InitSaleItem calls OnAddSaleItem failed, unique_key[%lld], item[%d,%d]",
			saleitem.salitem_param.unique_key, saleitem.salitem_param.item_wrapper.item_id,
			saleitem.salitem_param.item_wrapper.num);
		return false;
	}

	info = this->GetTradeItemInfo(saleitem.salitem_param.unique_key);
	if (info != NULL)
	{
		// 正式上架，把货品加到货品索引中
		if (SALE_ITEM_STATUS_ON_SALE == saleitem.salitem_param.cur_status)
		{
			TradeMarketManager::Instance().GetSaleItemIndex().OnAddItemOnSale(info->sale_item_ptr);
		}
		else if (SALE_ITEM_STATUS_ANNOUNCEMENT == saleitem.salitem_param.cur_status)
		{
			TradeMarketManager::Instance().GetSaleItemIndex().OnAddItemOnAnnounce(info->sale_item_ptr);
		}
	}
	
	return true;
}

bool SystemTradeInfo::SaveDB()
{
	this->RemoveOffShelveItems();

	TradeMarketData system_sale_data;
	system_sale_data.count = 0;
	system_sale_data.is_system_shelve = true;
	this->GetChangeSaleItemList(&system_sale_data);

	if (system_sale_data.count <= 0) return true;

	RMISaveTradeMarketDataBackObjectImpl *impl = new RMISaveTradeMarketDataBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	bool ret = ac.SaveTradeMarketDataAsync(system_sale_data, impl);
	if (ret)
	{
		this->ClearDirtyFlag(system_sale_data);
	}

	return ret;
}

bool SystemTradeInfo::OnAddSaleItem(long long unique_key, int seller_uid, const char* seller_name, const SaleItemParam& param, bool is_init)
{
	const SystemTradeItemInfo* stii = this->GetTradeItemInfo(unique_key);
	if (NULL != stii) return false;

	SystemSaleItem* sys_sale_item = new SystemSaleItem();
	sys_sale_item->Init(seller_uid, seller_name, param, false);
	sys_sale_item->GetSaleItemParam().unique_key = unique_key;

	SystemTradeItemInfo info;
	if (!is_init)
	{
		info.is_dirty = true;
		info.old_state = false;
		m_is_change = true;
	}
	else
	{
		info.old_state = true;
		info.is_dirty = false;

		// 检查宠物资质数据
		if (info.sale_item_ptr != NULL)
		{
			const ItemBase* item = ITEMPOOL->GetItem(info.sale_item_ptr->GetSaleItemParam().item_wrapper.item_id);
			if (NULL != item && I_TYPE_PET == item->GetItemType())
			{
				NetValueItemParam temp_param;
				temp_param.SetData(info.sale_item_ptr->GetSaleItemParam().item_wrapper.param_data);
				if (temp_param.pet_param.CheckGrowthVersion(info.sale_item_ptr->GetSaleItemParam().item_wrapper.item_unique_id, __FUNCTION__))
				{
					temp_param.GetData(info.sale_item_ptr->GetSaleItemParam().item_wrapper.param_data);
					info.is_dirty = true;
				}

				if (temp_param.pet_param.CheckModifyVersion())
				{
					temp_param.GetData(info.sale_item_ptr->GetSaleItemParam().item_wrapper.param_data);
					info.is_dirty = true;
				}
			}
			else if (NULL != item && I_TYPE_MEDAL == item->GetItemType())
			{
				NetValueItemParam temp_param;
				temp_param.SetData(info.sale_item_ptr->GetSaleItemParam().item_wrapper.param_data);
				if (temp_param.medal_param.CheckModifyVersion())
				{
					temp_param.GetData(info.sale_item_ptr->GetSaleItemParam().item_wrapper.param_data);
					info.is_dirty = true;
				}
			}
			else if (NULL != item && I_TYPE_EQUIPMENT == item->GetItemType())
			{
				NetValueItemParam temp_param;
				temp_param.SetData(info.sale_item_ptr->GetSaleItemParam().item_wrapper.param_data);
				if (temp_param.equip_param.CheckModifyVersion())
				{
					temp_param.GetData(info.sale_item_ptr->GetSaleItemParam().item_wrapper.param_data);
					info.is_dirty = true;
				}
			}
			else if (NULL != item && I_TYPE_JEWELRY == item->GetItemType())
			{
				NetValueItemParam temp_param;
				temp_param.SetData(info.sale_item_ptr->GetSaleItemParam().item_wrapper.param_data);
				if (temp_param.jewelry_param.CheckModifyVersion())
				{
					temp_param.GetData(info.sale_item_ptr->GetSaleItemParam().item_wrapper.param_data);
					info.is_dirty = true;
				}
			}
		}	

		const ItemBase* item = ITEMPOOL->GetItem(sys_sale_item->GetSaleItemParamConst().item_wrapper.item_id);
		if (NULL == item || item->GetTradeNumPerGroup() != sys_sale_item->GetSaleItemParamConst().num_per_group
			|| !item->CanTrade() 
			|| sys_sale_item->GetSaleItemParamConst().gold_price > TradeMarketManager::GetTradeMaxPrice(item, &sys_sale_item->GetSaleItemParamConst().item_wrapper)
			|| sys_sale_item->GetSaleItemParamConst().gold_price < TradeMarketManager::GetTradeMinPrice(item, &sys_sale_item->GetSaleItemParamConst().item_wrapper))
		{
			if (NULL == item)
			{
				gamelog::g_log_trade_market.printf(LL_INFO, "SystemTradeInfo::OnAddSaleItem Item Null Error is_init[%s] role[%d,%s] item_id[%d] unique_key[%lld]",
					is_init ? "true" : "false", seller_uid, seller_name, param.item_wrapper.item_id, param.unique_key);
			}
			else if (item->GetTradeNumPerGroup() != sys_sale_item->GetSaleItemParamConst().num_per_group)
			{
				gamelog::g_log_trade_market.printf(LL_INFO, "SystemTradeInfo::OnAddSaleItem NumPerGroup Error is_init[%s] role[%d,%s] item_id[%d] num_per_group_cfg[%d] num_per_group_actual[%d] unique_key[%lld]",
					is_init ? "true" : "false", seller_uid, seller_name, param.item_wrapper.item_id, item->GetTradeNumPerGroup(), param.num_per_group, param.unique_key);
			}
			else if (!item->CanTrade())
			{
				gamelog::g_log_trade_market.printf(LL_INFO, "SystemTradeInfo::OnAddSaleItem Item Cannot Trade Error is_init[%s] role[%d,%s] item_id[%d] num_per_group_cfg[%d] num_per_group_actual[%d] unique_key[%lld]",
					is_init ? "true" : "false", seller_uid, seller_name, param.item_wrapper.item_id, item->GetTradeNumPerGroup(), param.num_per_group, param.unique_key);
			}
			else if (sys_sale_item->GetSaleItemParamConst().gold_price > TradeMarketManager::GetTradeMaxPrice(item, &param.item_wrapper) ||
				sys_sale_item->GetSaleItemParamConst().gold_price < TradeMarketManager::GetTradeMinPrice(item, &param.item_wrapper))
			{
				gamelog::g_log_trade_market.printf(LL_INFO, "SystemTradeInfo::OnAddSaleItem Item Price Range Error is_init[%s] role[%d,%s] item_id[%d] num_per_group_cfg[%d] num_per_group_actual[%d] unique_key[%lld]",
					is_init ? "true" : "false", seller_uid, seller_name, param.item_wrapper.item_id, item->GetTradeNumPerGroup(), param.num_per_group, param.unique_key);
			}

			// 没有这个物品了，或group配置修改了，则强行下架
			sys_sale_item->GetSaleItemParam().next_status_time = 0u;
			sys_sale_item->GetSaleItemParam().cur_status = SALE_ITEM_STATUS_OFF_SHELVE;
			info.is_dirty = true;
		}
	}
	info.sale_item_ptr = sys_sale_item;
	
	m_trade_item_map.insert(SystemTradeItemMap::value_type(unique_key, info));

	gamelog::g_log_trade_market.printf(LL_INFO, "SystemTradeInfo::OnAddSaleItem is_init[%s] unique_key[%lld] cur_status[%d] gold_price[%d] immortal_price[%d] item_id[%d] num[%d] num_per_group[%d] item_search_type[%d] item_param[%s]",
		is_init ? "true" : "false", unique_key, param.cur_status, param.gold_price, param.immortal_coin_price,
		param.item_wrapper.item_id, param.item_wrapper.num, param.num_per_group, param.item_search_type,
		NetValueItemParam::GetParamLog(param.item_wrapper.item_id, param.item_wrapper.param_data));

	return true;
}

bool SystemTradeInfo::OnRemoveSaleItem(long long unique_key, const char* reason)
{
	SystemTradeItemInfo* stii = this->GetTradeItemInfo(unique_key);
	if (NULL == stii || NULL == stii->sale_item_ptr) return false;

	{
		//log
		SaleItemParam& param = stii->sale_item_ptr->GetSaleItemParam();
		gamelog::g_log_trade_market.printf(LL_INFO, "SystemTradeInfo::OnRemoveSaleItem reason[%s] unique_key[%lld] "
			"gold_price[%d] immortal_price[%d] item_id[%d] num[%d] num_per_group[%d] item_search_type[%d] item_param[%s]",
			reason, param.unique_key, param.gold_price, 
			param.immortal_coin_price, param.item_wrapper.item_id, 
			param.item_wrapper.num, param.num_per_group, 
			param.item_search_type,
			NetValueItemParam::GetParamLog(param.item_wrapper.item_id, param.item_wrapper.param_data));
	}

	//!< 从货品索引中删除
	if (SALE_ITEM_STATUS_ON_SALE == stii->sale_item_ptr->GetSaleItemParamConst().cur_status)
	{
		TradeMarketManager::Instance().GetSaleItemIndex().OnRemoveItemOnSale(stii->sale_item_ptr);
	}

	if (SALE_ITEM_STATUS_ANNOUNCEMENT == stii->sale_item_ptr->GetSaleItemParamConst().cur_status)
	{
		TradeMarketManager::Instance().GetSaleItemIndex().OnRemoveItemOnAnnounce(stii->sale_item_ptr);
	}


	delete stii->sale_item_ptr;
	stii->sale_item_ptr = NULL;
	stii->is_dirty = true;
	m_is_change = true;

	return true;
}

bool SystemTradeInfo::OnItemSold(long long unique_key, short sold_num, int buyer_uid, const char* buyer_name, const char* buyer_pname)
{
	SystemTradeItemInfo* stii = this->GetTradeItemInfo(unique_key);
	if (NULL == stii) return false;

	ItemID item_id = stii->sale_item_ptr->GetSaleItemParam().item_wrapper.item_id;

	stii->sale_item_ptr->GetSaleItemParam().sold_num += sold_num;
	stii->sale_item_ptr->GetSaleItemParam().item_wrapper.num -= sold_num;

	if (stii->sale_item_ptr->GetSaleItemParam().item_wrapper.num <= 0)
	{
		//卖完就下架
		stii->sale_item_ptr->GetSaleItemParam().cur_status = SALE_ITEM_STATUS_OFF_SHELVE;
		stii->sale_item_ptr->GetSaleItemParam().next_status_time = 0u;

		//!< 从货品索引中删除
		TradeMarketManager::Instance().GetSaleItemIndex().OnRemoveItemOnSale(stii->sale_item_ptr);
	}
	stii->is_dirty = true;
	m_is_change = true;

	gamelog::g_log_trade_market.printf(LL_INFO, "SystemTradeInfo::OnItemSold buyer[%d,%s] unique_key[%lld] item[%d,%d] sold_num[%d] total_sold_num[%d] num_per_group[%d], item_param[%s]",
		buyer_uid, buyer_name, unique_key,
		stii->sale_item_ptr->GetSaleItemParamConst().item_wrapper.item_id,
		stii->sale_item_ptr->GetSaleItemParamConst().item_wrapper.num, sold_num, stii->sale_item_ptr->GetSaleItemParamConst().sold_num,
		stii->sale_item_ptr->GetSaleItemParamConst().num_per_group,
		NetValueItemParam::GetParamLog(stii->sale_item_ptr->GetSaleItemParamConst().item_wrapper.item_id, stii->sale_item_ptr->GetSaleItemParamConst().item_wrapper.param_data));

	TradeMarketManager::Instance().OnItemSold(item_id, stii->sale_item_ptr->GetSaleItemParam().gold_price, sold_num, stii->sale_item_ptr->GetSaleItemParamConst().item_wrapper, 0, stii->sale_item_ptr->GetSellerName(), buyer_name, buyer_uid, buyer_pname);
	return true;
}

SystemTradeItemInfo* SystemTradeInfo::GetTradeItemInfo(long long unique_key)
{
	SystemTradeItemMap::iterator it = m_trade_item_map.find(unique_key);
	if (it != m_trade_item_map.end())
	{
		return &it->second;
	}
	return NULL;
}

void SystemTradeInfo::GetSystemBuyList(ARG_OUT std::vector<long long>* unique_key_vec)
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	SystemTradeItemMap::iterator it = m_trade_item_map.begin();
	for (; it != m_trade_item_map.end(); ++it)
	{
		SystemTradeItemInfo& info = it->second;
		if (NULL == info.sale_item_ptr) continue;

		SaleItemParam& param = info.sale_item_ptr->GetSaleItemParam();
		if (param.cur_status != SALE_ITEM_STATUS_ON_SALE) continue;

		if (param.system_buy_timestamp > 0 && now > param.system_buy_timestamp)
		{
			unique_key_vec->push_back(param.unique_key);
		}
	}
}

void SystemTradeInfo::GetSystemOffshelveList(ARG_OUT std::vector<long long>* unique_key_vec)
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	SystemTradeItemMap::iterator it = m_trade_item_map.begin();
	for (; it != m_trade_item_map.end(); ++it)
	{
		SystemTradeItemInfo& info = it->second;
		if (NULL == info.sale_item_ptr) continue;

		SaleItemParam& param = info.sale_item_ptr->GetSaleItemParam();
		if (param.cur_status != SALE_ITEM_STATUS_ON_SALE) continue;

		if (param.system_offshelve_timestamp > 0 && now > param.system_offshelve_timestamp)
		{
			unique_key_vec->push_back(param.unique_key);
		}
	}
}

void SystemTradeInfo::GetChangeSaleItemList(ARG_OUT TradeMarketData* data)
{
	if (!m_is_change)
	{
		data->count = 0;
		return;
	}

	int count = 0;
	SystemTradeItemMap::iterator it = m_trade_item_map.begin();
	for (; it != m_trade_item_map.end() && count < TradeMarketData::MAX_SALE_ITEM_COUNT; ++it)
	{
		SystemTradeItemInfo& info = it->second;
		if (!info.is_dirty) continue;

		if (info.IsValid())
		{
			//update or insert
			data->sale_list[count].change_state = (info.old_state ? structcommon::CS_UPDATE : structcommon::CS_INSERT);
			data->sale_list[count].owner_uid = info.sale_item_ptr->GetSellerUid();
			data->sale_list[count].unique_key = it->first;
			data->sale_list[count].seller_uid = info.sale_item_ptr->GetSellerUid();
			F_STRNCPY(data->sale_list[count].seller_name, info.sale_item_ptr->GetSellerName(), sizeof(GameName));
			data->sale_list[count].salitem_param = info.sale_item_ptr->GetSaleItemParam();
		}
		else
		{
			//delete
			if (!info.old_state) continue;

			data->sale_list[count].change_state = structcommon::CS_DELETE;
			data->sale_list[count].unique_key = it->first;
		}

		++count;
	}

	data->count = count;

	return;

}

void SystemTradeInfo::ClearDirtyFlag(const TradeMarketData& data)
{
	for (int i = 0; i < data.count; ++i)
	{
		long long unique_key = data.sale_list[i].unique_key;
		SystemTradeItemInfo* info = this->GetTradeItemInfo(unique_key);
		if (NULL == info) continue;

		if (info->IsValid())
		{
			info->is_dirty = false;
			info->old_state = true;
		}
		else
		{
			m_trade_item_map.erase(unique_key);
		}
	}
}

void SystemTradeInfo::RemoveOffShelveItems()
{
	std::vector<long long> offshelve_list;
	offshelve_list.reserve(m_trade_item_map.size());
	SystemTradeItemMap::iterator it = m_trade_item_map.begin();
	for (; it != m_trade_item_map.end(); ++it)
	{
		SystemSaleItem* ssi = it->second.sale_item_ptr;
		if (NULL != ssi && ssi->IsOffShelve())
		{
			offshelve_list.push_back(it->first);
		}
	}
	
	for (size_t i = 0; i < offshelve_list.size(); ++i)
	{
		this->OnRemoveSaleItem(offshelve_list[i], __FUNCTION__);
	}
}

