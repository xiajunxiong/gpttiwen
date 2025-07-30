#ifndef __RAND_ACTIVITY_SHAN_HAI_BAO_HE_CONFIG_HPP__
#define __RAND_ACTIVITY_SHAN_HAI_BAO_HE_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rashanhaibaoheparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

class RandActivityManager;

struct RAShanHaiBaoHeItemCfg
{
	RAShanHaiBaoHeItemCfg()
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

struct RAShanHaiBaoHeSectionCfg
{
	RAShanHaiBaoHeSectionCfg() :section_start(0), section_end(0)
	{
	}

	int section_start;
	int section_end;
	std::vector<RAShanHaiBaoHeItemCfg> item;
};

struct RAShanHaiBaoHeOtherCfg
{
	RAShanHaiBaoHeOtherCfg()
	{}

	std::vector<ItemConfigData> free_reward;
};

class RandActivityManager;
class RandActivityShanHaiBaoHeConfig : public RandActivityConfig
{
public:
	RandActivityShanHaiBaoHeConfig();
	virtual ~RandActivityShanHaiBaoHeConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RAShanHaiBaoHeItemCfg * GetRAShanHaiBaoHeItemCfg(RandActivityManager * ramgr, int seq) const;
	int GetRAShanHaiBaoHeItemNum(RandActivityManager * ramgr) const;
	const RAShanHaiBaoHeOtherCfg & GetOtherCfg()const { return m_other_cfg; }
private:
	int InitBuyCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	std::vector<RAShanHaiBaoHeSectionCfg> m_section_cfg;
	RAShanHaiBaoHeOtherCfg m_other_cfg;
};

#endif	//__RAND_ACTIVITY_SHAN_HAI_BAO_HE_CONFIG_HPP__