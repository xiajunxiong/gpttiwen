#include "trademarketconfig.hpp"
#include "servercommon/configcommon.h"
#include "item/itembase.h"
#include "equipment/equipment.h"
#include "item/pet/petitem.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/pet/petconfig.hpp"
#include "servercommon/trade_credit_def.hpp"
#include "checkresourcecenter.hpp"

TradeMarketConfig* g_trade_market_cfg = NULL;
TradeMarketConfig& TradeMarketConfig::Instance()
{
	if (NULL == g_trade_market_cfg)
	{
		g_trade_market_cfg = new TradeMarketConfig();
	}
	return *g_trade_market_cfg;
}

TradeMarketConfig::TradeMarketConfig()
{
	memset(m_search_type_hash_big_type, -1, sizeof(m_search_type_hash_big_type));
	memset(m_seq_hash_item_search_type, -1, sizeof(m_seq_hash_item_search_type));
}

TradeMarketConfig::~TradeMarketConfig()
{

}

bool TradeMarketConfig::Init(const std::string &configname, std::string *err)
{
	char errinfo[1024] = { 0 };
	int iRet = 0;

	TiXmlDocument document;

	if (configname == "" || !document.LoadFile(configname))
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: Load TradeMarketConfig Error,\n %s.", configname.c_str(), document.ErrorDesc());
		*err = errinfo;
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();

	if (NULL == RootElement)
	{
		*err = configname + ": xml root node error.";
		return false;
	}

	{
		// exchange_constant
		TiXmlElement *element = RootElement->FirstChildElement("exchange_constant");
		if (NULL == element)
		{
			*err = configname + ": no [exchange_constant].";
			return false;
		}

		iRet = InitConstantCfg(element);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitConstantCfg failed %d", configname.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	{
		// filtrate
		TiXmlElement *element = RootElement->FirstChildElement("trade_line_filtrate");
		if (NULL == element)
		{
			*err = configname + ": no [trade_line_filtrate].";
			return false;
		}

		iRet = InitFiltrateCfg(element);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitFiltrateCfg failed %d", configname.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	{
		// trade_credit
		TiXmlElement *element = RootElement->FirstChildElement("trade_credit");
		if (NULL == element)
		{
			*err = configname + ": no [trade_credit].";
			return false;
		}

		iRet = InitCreditCfg(element);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitCreditCfg failed %d", configname.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	{
		// Trading_floor_price
		TiXmlElement *element = RootElement->FirstChildElement("Trading_fioor_price");
		if (NULL == element)
		{
			*err = configname + ": no [Trading_fioor_price].";
			return false;
		}

		std::string errormsg;
		iRet = InitPetPriceRangeCfg(element, errormsg);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitPetPriceRangeCfg failed %d, errormsg[%s]", configname.c_str(), iRet, errormsg.c_str());
			*err = errinfo;
			return false;
		}
	}

	{
		// trade_credit
		TiXmlElement *element = RootElement->FirstChildElement("time_fluctuation");
		if (NULL == element)
		{
			*err = configname + ": no [time_fluctuation].";
			return false;
		}

		std::string errormsg;
		iRet = InitTimeFluctuationCfg(element, errormsg);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitTimeFluctuationCfg failed %d, reason[%s]", configname.c_str(), iRet, errormsg.c_str());
			*err = errinfo;
			return false;
		}
	}

	return true;
}

bool TradeMarketConfig::Reload(const std::string &configname, std::string *err)
{
	TradeMarketConfig* temp = new TradeMarketConfig();

	if (!temp->Init(configname, err))
	{
		delete temp;
		temp = NULL;
		return false;
	}

	if (NULL != g_trade_market_cfg) delete g_trade_market_cfg;

	g_trade_market_cfg = temp;

	return true;
}

short TradeMarketConfig::GetSearchBigType(int item_search_type)		//此时的item_search_type实际是物品表的search_type或
{
	if (item_search_type < 0 || item_search_type >= TRADE_ITEM_SEARCH_TYPE_MAX) return -1;

	return m_search_type_hash_big_type[item_search_type];
}

short TradeMarketConfig::GetItemSearchType(int seq)
{
	if(seq < 0 || seq >= TRADE_ITEM_SEARCH_TYPE_MAX) return -1;

	return m_seq_hash_item_search_type[seq];
}

bool TradeMarketConfig::IsNeedAnnounce(const ItemBase* item)
{
	if (I_TYPE_EQUIPMENT == item->GetItemType())
	{
		int announce_equip_color = m_contant_cfg.publicity_equip_color;
		if (Equipment::EQUIP_COLOR_IDX_MIN == announce_equip_color)
		{
			return false;
		}
		
		return ((Equipment*)item)->GetEquipColor() >= announce_equip_color;
	}
	else if (I_TYPE_PET == item->GetItemType())
	{
		int announce_pet_quality = m_contant_cfg.publicity_pet_quality;
		if (PET_QUALITY_INVALID == announce_pet_quality)
		{
			return false;
		}

		const PetCfg* cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(item->GetItemId());
		if (NULL == cfg)
		{
			return false;
		}

		return cfg->quality >= announce_pet_quality;
	}
	else if (I_TYPE_JEWELRY == item->GetItemType())
	{
		int announce_quality = m_contant_cfg.publicity_jewel_color;
		if (ItemBase::I_COLOR_INVALID == announce_quality)
		{
			return false;
		}

		return item->GetColor() >= announce_quality;
	}

	return false;
}

const TradeCreditCfg* TradeMarketConfig::GetTradeCreditCfg(int credit_type)
{
	if (credit_type <= CREDIT_TYPE_INVALID || credit_type >= CREDIT_TYPE_MAX)
	{
		return NULL;
	}

	return &m_credit_cfg_list[credit_type];
}

int TradeMarketConfig::GetSmallSearchTypeList(int big_search_type, ARG_OUT int* type_list, int max_list_num)
{
	if (big_search_type < 0 || big_search_type >= TRADE_ITEM_SEARCH_BIG_TYPE_MAX)
	{
		return 0;
	}

	if (m_big_type_to_small_type_list[big_search_type].empty())
	{
		return 0;
	}

	int small_type_num = static_cast<int>(m_big_type_to_small_type_list[big_search_type].size());
	if (max_list_num >= small_type_num)
	{
		memcpy(type_list, &m_big_type_to_small_type_list[big_search_type][0], small_type_num * sizeof(m_big_type_to_small_type_list[big_search_type][0]));
		return small_type_num;
	}
	else
	{
		memcpy(type_list, &m_big_type_to_small_type_list[big_search_type][0], max_list_num * sizeof(m_big_type_to_small_type_list[big_search_type][0]));
		return max_list_num;
	}
}

const TradePetPriceRangeCfg* TradeMarketConfig::GetPetPriceRangeCfg(int pet_id, int pet_strength_lv) const
{
	TradePetPriceRangeKey key(pet_id, pet_strength_lv);
	PetPriceRangeCfgMap::const_iterator it = m_pet_price_range_cfg_map.find(key);
	if (it == m_pet_price_range_cfg_map.end())
	{
		return NULL;
	}

	return &it->second;
}

int TradeMarketConfig::GetTimeFluctuationSeq(int opendays)
{
	for (size_t i = 0; i < m_time_fluctuation_cfg_vec.size(); ++i)
	{
		if (opendays >= m_time_fluctuation_cfg_vec[i].open_days_start && opendays <= m_time_fluctuation_cfg_vec[i].open_days_end)
		{
			return m_time_fluctuation_cfg_vec[i].ret_seq;
		}
	}

	return 0;
}

int TradeMarketConfig::InitConstantCfg(TiXmlElement *RootElement)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	if (!GetSubNodeValue(dataElement, "tax_rate", m_contant_cfg.tax_rate) || m_contant_cfg.tax_rate < 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(dataElement, "upper_shelf_price_min", m_contant_cfg.upshelve_fee_min) || m_contant_cfg.upshelve_fee_min < 0)
	{
		return -2;
	}

	if (!GetSubNodeValue(dataElement, "upper_shelf_price_max", m_contant_cfg.upshelve_fee_max) || m_contant_cfg.upshelve_fee_max < m_contant_cfg.upshelve_fee_min)
	{
		return -3;
	}

	if (!GetSubNodeValue(dataElement, "advert_price_rate", m_contant_cfg.advertise_fee_rate) || m_contant_cfg.advertise_fee_rate < 0)
	{
		return -4;
	}

	if (!GetSubNodeValue(dataElement, "immortal_coin_advert_min", m_contant_cfg.immortal_coin_advertise_fee_min) || m_contant_cfg.immortal_coin_advertise_fee_min < 0)
	{
		return -5;
	}

	if (!GetSubNodeValue(dataElement, "immortal_coin_advert_max", m_contant_cfg.immortal_coin_advertise_fee_max) || m_contant_cfg.immortal_coin_advertise_fee_max < m_contant_cfg.immortal_coin_advertise_fee_min)
	{
		return -6;
	}

	if (!GetSubNodeValue(dataElement, "gold_advert_min", m_contant_cfg.gold_advertise_fee_min) || m_contant_cfg.gold_advertise_fee_min < 0)
	{
		return -7;
	}

	if (!GetSubNodeValue(dataElement, "gold_advert_max", m_contant_cfg.gold_advertise_fee_max) || m_contant_cfg.gold_advertise_fee_max < m_contant_cfg.gold_advertise_fee_min)
	{
		return -8;
	}

	if (!GetSubNodeValue(dataElement, "notice", m_contant_cfg.need_gold_notice) || m_contant_cfg.need_gold_notice < 0)
	{
		return -9;
	}

	if (!GetSubNodeValue(dataElement, "publicity_time", m_contant_cfg.publicity_time_s) || m_contant_cfg.publicity_time_s < 0)
	{
		return -10;
	}

	if (!GetSubNodeValue(dataElement, "publicity_pet_quality", m_contant_cfg.publicity_pet_quality) || m_contant_cfg.publicity_pet_quality < PET_QUALITY_INVALID || m_contant_cfg.publicity_pet_quality >= PET_QUALITY_MAX)
	{
		return -11;
	}

	if (!GetSubNodeValue(dataElement, "publicity_equipment_color", m_contant_cfg.publicity_equip_color) || m_contant_cfg.publicity_equip_color < Equipment::EQUIP_COLOR_IDX_MIN || m_contant_cfg.publicity_jewel_color >= Equipment::EQUIP_COLOR_IDX_MAX)
	{
		return -12;
	}

	if (!GetSubNodeValue(dataElement, "publicity_jewel_color", m_contant_cfg.publicity_jewel_color) || m_contant_cfg.publicity_jewel_color < ItemBase::I_COLOR_INVALID || m_contant_cfg.publicity_jewel_color >= ItemBase::I_COLOR_MAX)
	{
		return -13;
	}
		
	return 0;
}

int TradeMarketConfig::InitFiltrateCfg(TiXmlElement *RootElement)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	while (dataElement)
	{
		int seq = 0;
		if (!GetSubNodeValue(dataElement, "seq", seq) || seq < 0 || seq >= TRADE_ITEM_SEARCH_TYPE_MAX)
		{
			return -1;
		}

		short item_type = 0;
		if (!GetSubNodeValue(dataElement, "item_type", item_type) || item_type < 0 || item_type >= TRADE_ITEM_SEARCH_BIG_TYPE_MAX)
		{
			return -2;
		}
		short item_search_type = 0;
		if (!GetSubNodeValue(dataElement, "item_search_type", item_search_type) || item_search_type < -1 || item_search_type >= TRADE_ITEM_LEVEL_SEARCH_TYPE_NUM)
		{
			return -3;
		}

		m_search_type_hash_big_type[seq] = item_type;
		m_big_type_to_small_type_list[item_type].push_back(seq);
		m_seq_hash_item_search_type[seq] = item_search_type;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TradeMarketConfig::InitCreditCfg(TiXmlElement *RootElement)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	int last_type = 0;
	while (dataElement)
	{
		int type = 0;
		if (!GetSubNodeValue(dataElement, "type", type) || type != last_type + 1 || type <= CREDIT_TYPE_INVALID || type >= CREDIT_TYPE_MAX)
		{
			return -1;
		}
		last_type = type;

		int credit_num_per_time = 0;
		if (!GetSubNodeValue(dataElement, "credit_num", credit_num_per_time) || credit_num_per_time <= 0)
		{
			return -2;
		}

		int day_credit_limit = 0;
		if (!GetSubNodeValue(dataElement, "day_limit", day_credit_limit) || day_credit_limit < 0)
		{
			return -3;
		}

		m_credit_cfg_list[type].credit_num_per_time = credit_num_per_time;
		m_credit_cfg_list[type].day_credit_limit = day_credit_limit;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TradeMarketConfig::InitPetPriceRangeCfg(TiXmlElement *RootElement, std::string& errormsg)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		errormsg = STRING_SPRINTF("data node not found");
		return -10000;
	}

	while (dataElement)
	{
		int pet_id = 0;
		if (!GetSubNodeValue(dataElement, "id", pet_id))
		{
			errormsg = STRING_SPRINTF("id node not found");
			return -1;
		}
		CheckResourceCenter::Instance().GetPetCheck()->Add(pet_id, __FUNCTION__);

		int pet_strength_lv = 0;
		if (!GetSubNodeValue(dataElement, "level", pet_strength_lv) || pet_strength_lv <= 0 || pet_strength_lv > PET_STRENGTHEN_MAX_LEVEL)
		{
			errormsg = STRING_SPRINTF("level node not found, or level[%d] not in range(0,%d]", pet_strength_lv, PET_STRENGTHEN_MAX_LEVEL);
			return -2;
		}

		int min_price = 0;
		if (!GetSubNodeValue(dataElement, "min_price", min_price) || min_price <= 0)
		{
			errormsg = STRING_SPRINTF("min_price node not found, or min_price[%d] <= 0", min_price);
			return -3;
		}

		int max_price = 0;
		if (!GetSubNodeValue(dataElement, "max_price", max_price) || max_price < min_price)
		{
			errormsg = STRING_SPRINTF("max_price node not found, or max_price[%d] < min_price[%d]", max_price, min_price);
			return -4;
		}

		TradePetPriceRangeKey key(pet_id, pet_strength_lv);
		TradePetPriceRangeCfg cfg;
		cfg.pet_id = pet_id;
		cfg.pet_strength_lv = pet_strength_lv;
		cfg.min_price = min_price;
		cfg.max_price = max_price;

		m_pet_price_range_cfg_map[key] = cfg;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int TradeMarketConfig::InitTimeFluctuationCfg(TiXmlElement* RootElement, std::string& errormsg)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		errormsg = "data node not found";
		return -10000;
	}

	int last_end = 0;
	int last_seq = -1;
	while (dataElement)
	{
		TimeFluctuationCfg cfg;
		if (!GetSubNodeValue(dataElement, "section_start", cfg.open_days_start) || cfg.open_days_start != last_end + 1)
		{
			errormsg = STRING_SPRINTF("section_start[%d] is not equal to last section_end[%d]", cfg.open_days_start, last_end);
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "section_end", cfg.open_days_end))
		{
			errormsg = STRING_SPRINTF("node[section_end] not found");
			return -2;
		}
		if (0 == cfg.open_days_end)
		{
			cfg.open_days_end = MAX_UINT16;
		}
		if (cfg.open_days_end < cfg.open_days_start)
		{
			errormsg = STRING_SPRINTF("section_end[%d] < section_start[%d]", cfg.open_days_end, cfg.open_days_start);
			return -22;
		}
		last_end = cfg.open_days_end;

		if (!GetSubNodeValue(dataElement, "seq", cfg.ret_seq) || cfg.ret_seq != last_seq + 1)
		{
			return -3;
		}
		last_seq = cfg.ret_seq;

		m_time_fluctuation_cfg_vec.push_back(cfg);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

