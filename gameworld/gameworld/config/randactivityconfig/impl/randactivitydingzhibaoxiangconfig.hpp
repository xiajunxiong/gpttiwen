#ifndef __RAND_ACTIVITY_DING_ZHI_BAO_XIANG_CONFIG_HPP__
#define __RAND_ACTIVITY_DING_ZHI_BAO_XIANG_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

class RandActivityManager;

struct RADingZhiBaoXiangItemCfg
{
	RADingZhiBaoXiangItemCfg()
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

struct RADingZhiBaoXiangSectionCfg
{
	RADingZhiBaoXiangSectionCfg() :section_start(0), section_end(0)
	{
	}

	int section_start;
	int section_end;
	std::vector<RADingZhiBaoXiangItemCfg> item;
};

struct RADingZhiBaoXiangRewardGroup
{
	RADingZhiBaoXiangRewardGroup()
	{
	}

	std::vector<ItemConfigData> rewards;
};

struct RADingZhiBaoXiangOtherCfg
{
	RADingZhiBaoXiangOtherCfg()
	{}

	std::vector<ItemConfigData> free_reward;
};

class RandActivityManager;
class RandActivityDingZhiBaoXiangConfig : public RandActivityConfig
{
public:
	RandActivityDingZhiBaoXiangConfig();
	virtual ~RandActivityDingZhiBaoXiangConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RADingZhiBaoXiangItemCfg * GetRADingZhiBaoXiangItemCfg(RandActivityManager * ramgr, int seq) const;
	const RADingZhiBaoXiangRewardGroup * GetRADingZhiBaoXiangRewardGroup(int reward_group_id) const;
	int GetRADingZhiBaoXiangItemNum(RandActivityManager * ramgr) const;
	const RADingZhiBaoXiangOtherCfg & GetOtherCfg()const { return m_other_cfg; }

private:
	int InitBuyCfg(TiXmlElement * RootElement);
	int InitRewardGroupCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	std::map<int, RADingZhiBaoXiangRewardGroup> m_reward_group;
	std::vector<RADingZhiBaoXiangSectionCfg> m_section_cfg;
	RADingZhiBaoXiangOtherCfg m_other_cfg;
};

#endif	//__RAND_ACTIVITY_LUCKY_BAG_CONFIG_HPP__