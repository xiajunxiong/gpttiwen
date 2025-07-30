#ifndef __RAND_ACTIVITY_DING_ZHI_BAO_XIANG_2_CONFIG_HPP__
#define __RAND_ACTIVITY_DING_ZHI_BAO_XIANG_2_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

class RandActivityManager;

struct RADingZhiBaoXiang2ItemCfg
{
	RADingZhiBaoXiang2ItemCfg()
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

struct RADingZhiBaoXiang2SectionCfg
{
	RADingZhiBaoXiang2SectionCfg() :section_start(0), section_end(0)
	{
	}

	int section_start;
	int section_end;
	std::vector<RADingZhiBaoXiang2ItemCfg> item;
};

struct RADingZhiBaoXiang2RewardGroup
{
	RADingZhiBaoXiang2RewardGroup()
	{
	}

	std::vector<ItemConfigData> rewards;
};

struct RADingZhiBaoXiang2OtherCfg
{
	RADingZhiBaoXiang2OtherCfg()
	{}

	std::vector<ItemConfigData> free_reward;
};

class RandActivityManager;
class RandActivityDingZhiBaoXiang2Config : public RandActivityConfig
{
public:
	RandActivityDingZhiBaoXiang2Config();
	virtual ~RandActivityDingZhiBaoXiang2Config();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RADingZhiBaoXiang2ItemCfg * GetRADingZhiBaoXiangItemCfg(RandActivityManager * ramgr, int seq) const;
	const RADingZhiBaoXiang2RewardGroup * GetRADingZhiBaoXiangRewardGroup(int reward_group_id) const;
	int GetRADingZhiBaoXiangItemNum(RandActivityManager * ramgr) const;
	const RADingZhiBaoXiang2OtherCfg & GetOtherCfg()const { return m_other_cfg; }
private:
	int InitBuyCfg(TiXmlElement * RootElement);
	int InitRewardGroupCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	std::map<int, RADingZhiBaoXiang2RewardGroup> m_reward_group;
	std::vector<RADingZhiBaoXiang2SectionCfg> m_section_cfg;
	RADingZhiBaoXiang2OtherCfg m_other_cfg;
};

#endif	//__RAND_ACTIVITY_LUCKY_BAG_CONFIG_HPP__