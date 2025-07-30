#ifndef __RAND_ACTIVITY_SECRET_TREASURE_CONFIG_HPP__
#define __RAND_ACTIVITY_SECRET_TREASURE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rasecrettreasureparam.hpp"

struct RASecretTreasureShopCfg
{
	RASecretTreasureShopCfg() : type(0), seq(0), limit_type(0), buy_times(0), today_times(0), price_type(0), price(0)
	{}

	enum PRICE_TYPE
	{
		PRICE_TYPE_MIN,
		PRICE_TYPE_GOLD = 0,//元宝
		PRICE_TYPE_COIN = 2,//铜币
		PRICE_TYPE_MONEY = 3,//直购
	};

	int type;
	int seq;
	int limit_type;
	int buy_times;
	int today_times;
	int price_type;
	int price;
	ItemConfigData item;
};

struct RASecretTreasureShopSectionCfg
{
	RASecretTreasureShopSectionCfg() : phase(0), section_start(0), section_end(0)
	{}

	int phase;
	int section_start;		//依据活动开启时间
	int section_end;
	std::vector<RASecretTreasureShopCfg> node_list;
};

class RandActivityManager;
class RandActivitySecretTreasureConfig : public RandActivityConfig
{
public:
	RandActivitySecretTreasureConfig();
	virtual ~RandActivitySecretTreasureConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);
	
	const RASecretTreasureShopCfg* GetShopCfg(int phase, int seq)const;
	const RASecretTreasureShopCfg* GetShopCfg(int type)const;
	int GetPhase(RandActivityManager * ramgr) const;

private:
	int InitSectionCfg(TiXmlElement  *RootElement);

	std::vector<RASecretTreasureShopSectionCfg> m_shop_section_cfg;
	std::map<int, RASecretTreasureShopCfg> m_shop_map;		//key : type
};

#endif	//__RAND_ACTIVITY_SECRET_TREASURE_CONFIG_HPP__