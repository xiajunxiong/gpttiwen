#ifndef __RAND_ACTIVITY_XIAN_SHI_BAO_HE_3_CONFIG_HPP__
#define __RAND_ACTIVITY_XIAN_SHI_BAO_HE_3_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

class RandActivityManager;

struct RAXianShiBaoHe3ItemCfg
{
	RAXianShiBaoHe3ItemCfg()
	{
		seq = 0;
		need_chong_zhi_gold = 0;
		limit_type = 0;
		limit_buy_times_day = 0;
		limit_buy_times = 0;
		add_gold = 0;
	}

	int seq;
	int need_chong_zhi_gold;
	int limit_type;
	int limit_buy_times_day;
	int limit_buy_times;

	int add_gold;
	std::vector<ItemConfigData> rewards;
};

struct RAXianShiBaoHe3SectionCfg
{
	RAXianShiBaoHe3SectionCfg() :section_start(0), section_end(0)
	{
	}

	int section_start;
	int section_end;
	std::vector<RAXianShiBaoHe3ItemCfg> item;
};

struct RAXianShiBaoHe3OtherCfg
{
	RAXianShiBaoHe3OtherCfg() : cfg_ver(0)
	{}

	int cfg_ver;
	std::vector<ItemConfigData> free_reward;
};

class RandActivityManager;
class RandActivityXianShiBaoHe3Config : public RandActivityConfig
{
public:
	RandActivityXianShiBaoHe3Config();
	virtual ~RandActivityXianShiBaoHe3Config();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RAXianShiBaoHe3ItemCfg * GetRAXianShiBaoHeItemCfg(RandActivityManager * ramgr, int seq) const;
	int GetRAXianShiBaoHeItemNum(RandActivityManager * ramgr) const;
	const RAXianShiBaoHe3OtherCfg & GetOtherCfg()const { return m_other_cfg; }

	virtual int GetConfigVer() const;

private:
	int InitBuyCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	std::vector<RAXianShiBaoHe3SectionCfg> m_section_cfg;
	RAXianShiBaoHe3OtherCfg m_other_cfg;
};

#endif	