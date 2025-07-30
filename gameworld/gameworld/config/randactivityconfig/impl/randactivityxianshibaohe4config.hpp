#ifndef __RAND_ACTIVITY_XIAN_SHI_BAO_HE_4_CONFIG_HPP__
#define __RAND_ACTIVITY_XIAN_SHI_BAO_HE_4_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

class RandActivityManager;

struct RAXianShiBaoHe4ItemCfg
{
	RAXianShiBaoHe4ItemCfg()
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

struct RAXianShiBaoHe4SectionCfg
{
	RAXianShiBaoHe4SectionCfg() :section_start(0), section_end(0)
	{
	}

	int section_start;
	int section_end;
	std::vector<RAXianShiBaoHe4ItemCfg> item;
};

struct RAXianShiBaoHe4OtherCfg
{
	RAXianShiBaoHe4OtherCfg() : cfg_ver(0)
	{}

	int cfg_ver;
	std::vector<ItemConfigData> free_reward;
};

class RandActivityManager;
class RandActivityXianShiBaoHe4Config : public RandActivityConfig
{
public:
	RandActivityXianShiBaoHe4Config();
	virtual ~RandActivityXianShiBaoHe4Config();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RAXianShiBaoHe4ItemCfg * GetRAXianShiBaoHeItemCfg(RandActivityManager * ramgr, int seq) const;
	int GetRAXianShiBaoHeItemNum(RandActivityManager * ramgr) const;
	const RAXianShiBaoHe4OtherCfg & GetOtherCfg()const { return m_other_cfg; }

	virtual int GetConfigVer() const;
private:
	int InitBuyCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	std::vector<RAXianShiBaoHe4SectionCfg> m_section_cfg;
	RAXianShiBaoHe4OtherCfg m_other_cfg;
};

#endif	