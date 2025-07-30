#ifndef __RAND_ACTIVITY_SALE_SHOP_CONFIG_HPP__
#define __RAND_ACTIVITY_SALE_SHOP_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rasaleshopparam.hpp"
#include "item/itembase.h"

struct RASaleShopCfg
{
	RASaleShopCfg() :item_seq(0), price_type(0), price(0), buy_times(0), today_times(0), limit_type(0)
	{}

	int item_seq;					//道具类型下的seq
	ItemConfigData item;
	int price_type;				//消耗的货币类型
	int price;					//所需货币量
	int buy_times;				//限购次数(活动时间内)
	int today_times;			//每日限购
	int limit_type;				//限购类型
};

struct RASaleShopSectionCfg
{
	RASaleShopSectionCfg() :section_start(0), section_end(0)
	{}

	struct RASaleShopTypeCfg
	{
		RASaleShopTypeCfg() :item_type(0)
		{}

		int item_type;								//道具种类
		std::vector<RASaleShopCfg> item_list;
	};

	int section_start;
	int section_end;
	RASaleShopTypeCfg node_cfg[SALE_SHOP_ITEM_TYPE_MAX];
};

class RandActivityManager;

class RandActivitySaleShopConfig : public RandActivityConfig
{
public:
	RandActivitySaleShopConfig();
	virtual ~RandActivitySaleShopConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RASaleShopCfg* GetSaleShopItemCfg(RandActivityManager* ramgr, int item_type, int item_seq) const;
	int GetSaleShopTypeItemCount(RandActivityManager* ramgr, int item_type) const ;
	const RASaleShopSectionCfg* GetSaleShopSectionCfg(RandActivityManager* ramgr)const;
private:
	int InitCfg(TiXmlElement *RootElement);

	std::vector<RASaleShopSectionCfg> m_sale_shop_section_cfg;
};

#endif	//__RAND_ACTIVITY_SALE_SHOP_CONFIG_HPP__