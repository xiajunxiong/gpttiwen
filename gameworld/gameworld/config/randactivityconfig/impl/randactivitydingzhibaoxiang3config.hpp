#ifndef __RAND_ACTIVITY_DING_ZHI_BAO_XIANG_3_CONFIG_HPP__
#define __RAND_ACTIVITY_DING_ZHI_BAO_XIANG_3_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

class RandActivityManager;

struct RADingZhiBaoXiang3ItemCfg
{
	RADingZhiBaoXiang3ItemCfg()
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

struct RADingZhiBaoXiang3SectionCfg
{
	RADingZhiBaoXiang3SectionCfg() :section_start(0), section_end(0)
	{
	}

	int section_start;
	int section_end;
	std::vector<RADingZhiBaoXiang3ItemCfg> item;
};

struct RADingZhiBaoXiang3RewardGroup
{
	RADingZhiBaoXiang3RewardGroup()
	{
	}

	std::vector<ItemConfigData> rewards;
};

struct RADingZhiBaoXiang3OtherCfg
{
	RADingZhiBaoXiang3OtherCfg() : cfg_ver(0)
	{}

	int cfg_ver;
	std::vector<ItemConfigData> free_reward;
};

class RandActivityManager;
class RandActivityDingZhiBaoXiang3Config : public RandActivityConfig
{
public:
	RandActivityDingZhiBaoXiang3Config();
	virtual ~RandActivityDingZhiBaoXiang3Config();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RADingZhiBaoXiang3ItemCfg * GetRADingZhiBaoXiangItemCfg(RandActivityManager * ramgr, int seq) const;
	const RADingZhiBaoXiang3RewardGroup * GetRADingZhiBaoXiangRewardGroup(int reward_group_id) const;
	int GetRADingZhiBaoXiangItemNum(RandActivityManager * ramgr) const;
	const RADingZhiBaoXiang3OtherCfg & GetOtherCfg()const { return m_other_cfg; }

	virtual int GetConfigVer() const;
private:
	int InitBuyCfg(TiXmlElement * RootElement);
	int InitRewardGroupCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	std::map<int, RADingZhiBaoXiang3RewardGroup> m_reward_group;
	std::vector<RADingZhiBaoXiang3SectionCfg> m_section_cfg;
	RADingZhiBaoXiang3OtherCfg m_other_cfg;
};

#endif