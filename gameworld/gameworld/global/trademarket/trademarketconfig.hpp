#ifndef __TRADE_MARKET_CONFIG_HPP__
#define __TRADE_MARKET_CONFIG_HPP__

#include <string>
#include "servercommon/struct/global/trademarketparam.hpp"
#include "servercommon/trade_credit_def.hpp"

#define TRADEMARKET_CFG TradeMarketConfig::Instance()

struct TradeMarketConstantCfg
{
	TradeMarketConstantCfg() : tax_rate(0), advertise_fee_rate(0), upshelve_fee_min(0), upshelve_fee_max(0),immortal_coin_advertise_fee_min(0), 
		immortal_coin_advertise_fee_max(0), gold_advertise_fee_min(0), gold_advertise_fee_max(0), need_gold_notice(0),
		publicity_time_s(0), publicity_pet_quality(0), publicity_equip_color(0),
		publicity_jewel_color(0)
	{
	}

	short tax_rate;				// 千分比
	short advertise_fee_rate;	// 千分比
	int upshelve_fee_min;
	int upshelve_fee_max;
	int immortal_coin_advertise_fee_min;
	int immortal_coin_advertise_fee_max;
	int gold_advertise_fee_min;
	int gold_advertise_fee_max;
	int need_gold_notice;			//大于或等于这个数量则发传闻
	int publicity_time_s;			//公示时间（秒）
	int publicity_pet_quality;		//公示宠物质量 1普 2银 3金 4稀 5神
 	int publicity_equip_color;		//公示装备颜色 1白 2绿 3蓝 4紫 5橙 6红
	int publicity_jewel_color;		//公示灵饰颜色 
};

struct TradeCreditCfg
{
	TradeCreditCfg() : credit_num_per_time(0), day_credit_limit(0) {}

	int credit_num_per_time;
	int day_credit_limit;
};

struct TradePetPriceRangeKey
{
	TradePetPriceRangeKey() : pet_id(0), pet_strength_lv(0) {}
	TradePetPriceRangeKey(int _pet_id, int _pet_strength_lv) : pet_id(_pet_id), pet_strength_lv(_pet_strength_lv) {}

	bool operator < (const TradePetPriceRangeKey& other) const
	{
		if (pet_id != other.pet_id)
		{
			return pet_id < other.pet_id;
		}
		else
		{
			return pet_strength_lv < other.pet_strength_lv;
		}
	}

	int pet_id;
	int pet_strength_lv;
};

struct TradePetPriceRangeCfg
{
	TradePetPriceRangeCfg() : pet_id(0), pet_strength_lv(0), min_price(0), max_price(0) {}

	int pet_id;
	int pet_strength_lv;
	int min_price;
	int max_price;
};

struct TimeFluctuationCfg
{
	TimeFluctuationCfg() : open_days_start(0), open_days_end(0), ret_seq(0) {}

	int open_days_start;
	int open_days_end;
	int ret_seq;
};

class ItemBase;
class TradeMarketConfig
{
public:
	typedef std::vector<int> SmallTypeVec;
	typedef std::map<TradePetPriceRangeKey, TradePetPriceRangeCfg> PetPriceRangeCfgMap;

public:
	TradeMarketConfig();
	~TradeMarketConfig();

	static TradeMarketConfig& Instance();

	bool Init(const std::string &configname, std::string *err);
	static bool Reload(const std::string &configname, std::string *err);

	const TradeMarketConstantCfg& GetConstantCfg() const { return m_contant_cfg; }
	short GetSearchBigType(int item_search_type);
	short GetItemSearchType(int seq);
	bool IsNeedAnnounce(const ItemBase* item);
	const TradeCreditCfg* GetTradeCreditCfg(int credit_type);

	int GetSmallSearchTypeList(int big_search_type, ARG_OUT int* type_list, int max_list_num);

	const TradePetPriceRangeCfg* GetPetPriceRangeCfg(int pet_id, int pet_strength_lv) const;
	int GetTimeFluctuationSeq(int opendays);
private:
	int InitConstantCfg(TiXmlElement *RootElement);
	int InitFiltrateCfg(TiXmlElement *RootElement);
	int InitCreditCfg(TiXmlElement *RootElement);
	int InitPetPriceRangeCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitTimeFluctuationCfg(TiXmlElement* RootElement, std::string& errormsg);

	TradeMarketConstantCfg m_contant_cfg;
	short m_search_type_hash_big_type[TRADE_ITEM_SEARCH_TYPE_MAX];
	short m_seq_hash_item_search_type[TRADE_ITEM_SEARCH_TYPE_MAX];
	SmallTypeVec m_big_type_to_small_type_list[TRADE_ITEM_SEARCH_BIG_TYPE_MAX];

	TradeCreditCfg m_credit_cfg_list[CREDIT_TYPE_MAX];

	PetPriceRangeCfgMap m_pet_price_range_cfg_map;

	std::vector<TimeFluctuationCfg> m_time_fluctuation_cfg_vec;
};

#endif