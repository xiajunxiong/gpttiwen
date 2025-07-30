#ifndef __RAND_ACTIVITY_SHAN_HAI_BAO_XIANG_CONFIG_HPP__
#define __RAND_ACTIVITY_SHAN_HAI_BAO_XIANG_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rashanhaibaoxiangparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

class RandActivityManager;

struct RAShanHaiBaoXiangItemCfg
{
	RAShanHaiBaoXiangItemCfg()
	{
		seq = 0;
		reward_group_id = 0;
		reward_select_num = 0;
		need_chong_zhi_gold = 0;
		limit_type = 0;
		limit_buy_times_day = 0;
		limit_buy_times = 0;
	}

	int seq;
	int reward_group_id;
	int reward_select_num;
	int need_chong_zhi_gold;
	int limit_type;
	int limit_buy_times_day;
	int limit_buy_times;
};

struct RAShanHaiBaoXiangSectionCfg
{
	RAShanHaiBaoXiangSectionCfg() :section_start(0), section_end(0)
	{
	}

	int section_start;
	int section_end;
	std::vector<RAShanHaiBaoXiangItemCfg> item;
};

struct RAShanHaiBaoXiangRewardGroup
{
	RAShanHaiBaoXiangRewardGroup()
	{
	}

	std::vector<ItemConfigData> rewards;
};

struct RAShanHaiBaoXiangOtherCfg
{
	RAShanHaiBaoXiangOtherCfg()
	{}

	std::vector<ItemConfigData> free_reward;
};

class RandActivityManager;
class RandActivityShanHaiBaoXiangConfig : public RandActivityConfig
{
public:
	RandActivityShanHaiBaoXiangConfig();
	virtual ~RandActivityShanHaiBaoXiangConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RAShanHaiBaoXiangItemCfg * GetRAShanHaiBaoXiangItemCfg(RandActivityManager * ramgr, int seq) const;
	const RAShanHaiBaoXiangRewardGroup * GetRAShanHaiBaoXiangRewardGroup(int reward_group_id) const;
	int GetRAShanHaiBaoXiangItemNum(RandActivityManager * ramgr) const;
	const RAShanHaiBaoXiangOtherCfg & GetOtherCfg()const { return m_other_cfg; }
private:
	int InitBuyCfg(TiXmlElement * RootElement);
	int InitRewardGroupCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	std::map<int, RAShanHaiBaoXiangRewardGroup> m_reward_group;
	std::vector<RAShanHaiBaoXiangSectionCfg> m_section_cfg;
	RAShanHaiBaoXiangOtherCfg m_other_cfg;
};

#endif	//__RAND_ACTIVITY_SHAN_HAI_BAO_XIANG_CONFIG_HPP__