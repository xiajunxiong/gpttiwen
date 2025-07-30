#ifndef __RAND_ACTIVITY_SUMMER_STORE_CONFIG_HPP__
#define __RAND_ACTIVITY_SUMMER_STORE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/raburningsummerparam.hpp"

struct RASummerStoreOtherCfg
{
	RASummerStoreOtherCfg() : phase(0), cfg_ver(0)
	{}

	int phase;
	int cfg_ver;
};

struct RASummerStoreShopCfg
{
	RASummerStoreShopCfg() :/* type(0), */index(0), seq(0), limit_type(0), buy_times(0), today_times(0), price_type(0), price(0)
	{}

	enum PRICE_TYPE
	{
		PRICE_TYPE_MIN,
		PRICE_TYPE_GOLD = 0,//Ôª±¦
		PRICE_TYPE_COIN = 2,//Í­±Ò
		PRICE_TYPE_MONEY = 3,//Ö±¹º
	};

	//int type;
	int index;
	int seq;
	int limit_type;
	int buy_times;
	int today_times;
	int price_type;
	int price;
	ItemConfigData reward_item;
};

struct RASummerStoreSectionShopCfg
{
	RASummerStoreSectionShopCfg() : start_time(0), end_time(0), phase(0)
	{}

	int start_time;
	int end_time;
	int phase;
	std::vector<RASummerStoreShopCfg> shop_list;
};

struct RASummerStoreSeverSectionShopCfg
{
	RASummerStoreSeverSectionShopCfg() : section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RASummerStoreSectionShopCfg> section_list;
};

class RandActivityManager;
class RandActivitySummerStoreConfig : public RandActivityConfig
{
public:
	RandActivitySummerStoreConfig();
	virtual ~RandActivitySummerStoreConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RASummerStoreOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const RASummerStoreShopCfg * GetShopCfg(RandActivityManager * ramgr, int seq)const;

	virtual int GetConfigVer() const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitShopCfg(TiXmlElement *RootElement);

	RASummerStoreOtherCfg m_other_cfg;
	std::vector<RASummerStoreSeverSectionShopCfg> m_shop_section_cfg;
};

#endif //__RAND_ACTIVITY_SUMMER_STORE_CONFIG_HPP__
