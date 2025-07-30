#ifndef __RAND_ACTIVITY_XIAN_SHI_BAO_HE_2_CONFIG_HPP__
#define __RAND_ACTIVITY_XIAN_SHI_BAO_HE_2_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

class RandActivityManager;

struct RAXianShiBaoHe2ItemCfg
{
	RAXianShiBaoHe2ItemCfg()
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

struct RAXianShiBaoHe2SectionCfg
{
	RAXianShiBaoHe2SectionCfg() :section_start(0), section_end(0)
	{
	}

	int section_start;
	int section_end;
	std::vector<RAXianShiBaoHe2ItemCfg> item;
};

struct RAXianShiBaoHe2OtherCfg
{
	RAXianShiBaoHe2OtherCfg()
	{}

	std::vector<ItemConfigData> free_reward;
};

class RandActivityManager;
class RandActivityXianShiBaoHe2Config : public RandActivityConfig
{
public:
	RandActivityXianShiBaoHe2Config();
	virtual ~RandActivityXianShiBaoHe2Config();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RAXianShiBaoHe2ItemCfg * GetRAXianShiBaoHeItemCfg(RandActivityManager * ramgr, int seq) const;
	int GetRAXianShiBaoHeItemNum(RandActivityManager * ramgr) const;
	const RAXianShiBaoHe2OtherCfg & GetOtherCfg()const { return m_other_cfg; }
private:
	int InitBuyCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	std::vector<RAXianShiBaoHe2SectionCfg> m_section_cfg;
	RAXianShiBaoHe2OtherCfg m_other_cfg;
};

#endif	//__RAND_ACTIVITY_LUCKY_BAG_CONFIG_HPP__