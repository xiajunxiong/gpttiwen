#ifndef __RAND_ACTIVITY_DUI_HUAN_LING_YU_CONFIG_HPP__
#define __RAND_ACTIVITY_DUI_HUAN_LING_YU_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

struct RADuiHuanLingYuCfg
{
	static const int BASE_NUM = 10000;

	RADuiHuanLingYuCfg() : price_type(0), price(0)	{}

	ItemConfigData item;
	int price_type;
	int price;
};

class Role;
class RandActivityDuiHuanLingYuConfig : public RandActivityConfig
{
public:
	RandActivityDuiHuanLingYuConfig();
	virtual ~RandActivityDuiHuanLingYuConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RADuiHuanLingYuCfg * GetMarketItemCfg(int index)const;

	void OnSaleItem(Role * role, int index)const;

private:
	int InitCfg(TiXmlElement * RootElement);

	std::map<int, RADuiHuanLingYuCfg> m_market_item_map;
};

#endif	