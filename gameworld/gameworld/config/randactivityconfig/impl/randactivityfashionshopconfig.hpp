#ifndef __RAND_ACTIVITY_FASHION_SHOP_CONFIG_HPP__
#define __RAND_ACTIVITY_FASHION_SHOP_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

class RandActivityManager;

struct RAFashionShopCfg
{
	RAFashionShopCfg()
	{
		seq = 0;
		need_chong_zhi = 0;
		gold = 0;
		limit_buy_times = 0;
	}

	int seq;
	int need_chong_zhi;
	int gold;
	unsigned int limit_buy_times;
	std::vector<ItemConfigData> rewards;
};

struct RAFashionShopSectionCfg
{
	RAFashionShopSectionCfg() :section_start(0), section_end(0)
	{
	}

	int section_start;
	int section_end;
	std::vector<RAFashionShopCfg> item;
};

struct RAFashionShopOtherCfg
{
	RAFashionShopOtherCfg()
	{
		cfg_ver = 0;
	}

	int cfg_ver;
};

class RandActivityManager;
class RandActivityFashionShopConfig : public RandActivityConfig
{
public:
	RandActivityFashionShopConfig();
	virtual ~RandActivityFashionShopConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RAFashionShopCfg * GetRAFashionShopCfg(RandActivityManager * ramgr, int seq) const;
	virtual int GetConfigVer() const;
private:
	int InitBuyCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);


	std::vector<RAFashionShopSectionCfg> m_section_cfg;
	RAFashionShopOtherCfg m_other_cfg;
};

#endif	