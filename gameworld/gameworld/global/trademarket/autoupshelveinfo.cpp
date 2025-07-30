#include "autoupshelveinfo.hpp"
#include "engineadapter.h"
#include "trademarketmanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/pet/petconfig.hpp"
#include "gamelog.h"
#include "global/usercache/usercache.hpp"
#include "other/medal/medalconfig.hpp"

AutoUpshelveInfo::AutoUpshelveInfo() : m_is_dirty(false), m_old_state(false),
	m_invalid_flag(false), m_has_sold(false), m_firsttime_upshelve(true), m_cur_float_percent(0),
	m_next_upshelve_timestamp(0u)
{

}

AutoUpshelveInfo::~AutoUpshelveInfo()
{

}

bool AutoUpshelveInfo::Init(const AutoUpshelveParam& param, bool is_from_init)
{
	if (is_from_init)
	{
		m_is_dirty = false;
		m_old_state = true;
	}
	else
	{
		if (m_param.unique_id != 0)//被替代
		{
			m_is_dirty = true;
			m_old_state = true;
		}
		else
		{
			m_is_dirty = true;
			m_old_state = false;
		}
		
		TradeMarketManager::Instance().GetAutoUpshelve().SetIsChange(true);
	}
	m_param = param;

	m_cur_float_percent = 0;
	m_next_upshelve_timestamp = 0u;
	m_invalid_flag = false;
	m_has_sold = false;
	m_firsttime_upshelve = true;

	return true;
}

bool AutoUpshelveInfo::GetInitParam(ARG_OUT AutoUpshelveParam* param)
{
	*param = m_param;
	return true;
}

void AutoUpshelveInfo::Update(unsigned int now)
{
	if (!this->IsValid()) return;

	if (now > m_param.end_timestamp)
	{
		m_invalid_flag = true;
		m_is_dirty = true;
		TradeMarketManager::Instance().GetAutoUpshelve().SetIsChange(true);
		return;
	}
	else if (now > m_param.begin_timestamp)
	{
		if (0u == m_next_upshelve_timestamp)
		{
			m_next_upshelve_timestamp = now;
		}
	}

	if (now > m_next_upshelve_timestamp && m_next_upshelve_timestamp > 0)
	{
		// 执行上架逻辑
		this->DoSystemUpshelve();
		m_next_upshelve_timestamp = now + m_param.upshelve_interval_m * 60;
	}
}

bool AutoUpshelveInfo::GetUpshelvePrice(ARG_OUT int* price, ARG_OUT SaleItemParam* param)
{
	if (!this->IsValid()) return false;

	if (!m_firsttime_upshelve)
	{
		if (m_has_sold)
		{
			m_cur_float_percent += m_param.price_float_percent;
			m_cur_float_percent = GetMin(m_cur_float_percent, m_param.price_float_limit_percent);
		}
		else
		{
			m_cur_float_percent -= m_param.price_float_percent;
			m_cur_float_percent = GetMax(m_cur_float_percent, (short)-m_param.price_float_limit_percent);
		}

		*price = (int)(1.0 * m_param.upshelve_price * (100 + m_cur_float_percent) / 100);
	}
	else
	{
		*price = m_param.upshelve_price;
		m_firsttime_upshelve = false;
	}	
	
	const ItemBase* item = ITEMPOOL->GetItem(m_param.item_id);
	if (NULL != item)
	{
		*price = GetMin(*price, TradeMarketManager::GetTradeMaxPrice(item, &param->item_wrapper));
		*price = GetMax(*price, TradeMarketManager::GetTradeMinPrice(item, &param->item_wrapper));
	}
	return true;
}

void AutoUpshelveInfo::OnSold()
{
	m_has_sold = true;
}

void AutoUpshelveInfo::DoSystemUpshelve()
{
	std::string error_msg;

	do
	{
		int cur_item_num = TradeMarketManager::Instance().GetItemStorage(m_param.item_id);
		if (cur_item_num >= m_param.auto_upshelve_limit_num)
		{
			// 已有足够数量的货品，暂不补货
			error_msg = "Upshelve Limit Num";
			break;
		}

		if (m_param.stop_upshelve_world_level > 0 && 
			UserCacheManager::Instance().GetHighestRoleLevel() > m_param.stop_upshelve_world_level)
		{
			error_msg = "Highest Role Level > Stop Upshelve Level";

			gamelog::g_log_trade_market.printf(LL_ERROR, "AutoUpshelveInfo::DoSystemUpshelve ERROR item_id[%d] unique_id[%lld] highest_level[%d] stop_level[%d]",
				m_param.item_id, m_param.unique_id, UserCacheManager::Instance().GetHighestRoleLevel(), m_param.stop_upshelve_world_level);
			break;
		}

		if (m_param.start_upshelve_world_level > 0 && UserCacheManager::Instance().GetHighestRoleLevel() < m_param.start_upshelve_world_level)
		{
			error_msg = "Highest Role Level < Start Upshelve Level";
			break;
		}

		const ItemBase* item = ITEMPOOL->GetItem(m_param.item_id);
		if (NULL == item)
		{
			error_msg = "Get Item Failed";
			break;
		}

		static SaleItemParam sip;
		sip.Reset();

		sip.item_wrapper.item_id = m_param.item_id;
		sip.item_wrapper.num = m_param.upshelve_num;
		sip.item_wrapper.is_bind = 0;
		sip.item_wrapper.has_param = 0;
		sip.system_buy_minute_num = 0;
		sip.system_offshelve_minute_num = m_param.auto_offshelve_time_m;

		if (item->GetItemType() >= I_TYPE_NET_VALUE_ITEM_BEGIN)
		{
			sip.item_wrapper.num = 1;
			for (int i = 0; i < m_param.upshelve_num; ++i)
			{
				if (I_TYPE_PET == item->GetItemType())
				{
					static NetValueItemParam param;
					param.Reset();
					if (!LOGIC_CONFIG->GetPetConfig()->RandPetParam(m_param.item_id, 1, false, param))
					{
						error_msg = "Get Rand Pet Param Failed";
						break;
					}
					sip.item_wrapper.has_param = 1;
					param.GetData(sip.item_wrapper.param_data);
				}
				else if (I_TYPE_MEDAL == item->GetItemType())
				{
					static NetValueItemParam param;
					param.Reset();

					const MedalItem* medal = (const MedalItem*)item;
					if (!LOGIC_CONFIG->GetMedalConfig()->RandMedalParam(&param, medal->GetAttrGroup(), medal->GetSpGroup(), medal->GetInitialRank()))
					{
						error_msg = "Get Rand Medal Param Failed";
						break;
					}

					sip.item_wrapper.has_param = 1;
					param.GetData(sip.item_wrapper.param_data);
				}
				else
				{
					error_msg = "Item Type Not Support Auto Upshelve";
					break;
				}

				int price = m_param.upshelve_price;
				if (!this->GetUpshelvePrice(&price, &sip))
				{
					error_msg = "Get Shelve Price Failed";
					break;
				}
				sip.gold_price = price;

				TradeMarketManager::Instance().AddItemOnSystemShelve(sip, error_msg);
			}
		}
		else
		{
			int price = m_param.upshelve_price;
			if (!this->GetUpshelvePrice(&price, &sip))
			{
				error_msg = "Get Shelve Price Failed";
				break;
			}
			sip.gold_price = price;

			TradeMarketManager::Instance().AddItemOnSystemShelve(sip, error_msg);
		}

	} while (0);
	
	if (error_msg != "")
	{
		gamelog::g_log_trade_market.printf(LL_WARNING, "AutoUpShelveInfo::DoSystemUpshelve FAILED, item[%d] unique_id[%lld] error_msg[%s]", m_param.item_id, m_param.unique_id, error_msg.c_str());
	}
	else
	{
		gamelog::g_log_trade_market.printf(LL_INFO, "AutoUpShelveInfo::DoSystemUpshelve SUCC, item[%d] unique_id[%lld]", m_param.item_id, m_param.unique_id);
	}
}

