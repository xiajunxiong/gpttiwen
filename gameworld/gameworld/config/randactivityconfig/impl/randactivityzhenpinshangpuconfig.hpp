#ifndef __RAND_ACTIVITY_ZHEN_PIN_SHANG_PU_CONFIG_HPP__
#define __RAND_ACTIVITY_ZHEN_PIN_SHANG_PU_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

class RandActivityManager;

struct RAZhenPinShangPuItemCfg
{
	RAZhenPinShangPuItemCfg()
	{
		type = 0;
		seq = 0;
		need_chong_zhi_gold = 0;
		limit_type = 0;
		limit_buy_times_day = 0;
		limit_buy_times = 0;
		min_buy_day = 0;
		max_buy_day = 0;
		buy_type = 0;
		select_reward_num = 0;
		reward_group_id = 0;
		add_gold = 0;
	}
	int type;
	int seq;
	int need_chong_zhi_gold;
	int limit_type;
	int limit_buy_times_day;
	int limit_buy_times;
	int min_buy_day;
	int max_buy_day;

	int buy_type;
	int select_reward_num;
	int reward_group_id;

	int add_gold;
};

struct  RAZhenPinShangPuOpenDay
{
	bool operator==(const RAZhenPinShangPuOpenDay & other)const
	{
		if (min_buy_day == other.min_buy_day && max_buy_day == other.max_buy_day)
		{
			return true;
		}

		return false;
	}

	int min_buy_day;
	int max_buy_day;
};

struct RAZhenPinShangPuSectionCfg
{
	RAZhenPinShangPuSectionCfg() :section_start(0), section_end(0)
	{
	}

	bool HasSame(RAZhenPinShangPuOpenDay other)
	{
		for (int i = 0; i < (int)open_day.size(); ++i)
		{
			if (open_day[i] == other)
			{
				return true;
			}
		}

		return false;
	}

	int section_start;
	int section_end;
	std::vector<RAZhenPinShangPuOpenDay> open_day;
	std::vector<RAZhenPinShangPuItemCfg> item;
};

struct RAZhenPinShangPuRewardGroupCfg
{
	RAZhenPinShangPuRewardGroupCfg()
	{
		group_id = 0;
	}

	int group_id;
	std::vector<ItemConfigData> item;
};

struct RAZhenPinShangPuOtherCfg
{
	RAZhenPinShangPuOtherCfg()
	{
		cfg_ver = 0;
	}

	int cfg_ver;
};

class RandActivityManager;
class RandActivityZhenPinShangPuConfig : public RandActivityConfig
{
public:
	RandActivityZhenPinShangPuConfig();
	virtual ~RandActivityZhenPinShangPuConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	int GetOpenDay(RandActivityManager * ramgr) const;
	const RAZhenPinShangPuItemCfg * GetRAZhenPinShangPuItemCfg(RandActivityManager * ramgr, int seq) const;
	bool GetRewards(int item_group_id, int select_num, int select_flag, std::vector<ItemConfigData> & rewards) const;
	const RAZhenPinShangPuRewardGroupCfg * GetRAZhenPinShangPuRewardGroupCfg(int item_group_id) const;
	virtual int GetConfigVer() const;
private:
	int InitBuyCfg(TiXmlElement * RootElement);
	int InitRewardGroupCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	std::vector<RAZhenPinShangPuSectionCfg> m_section_cfg;
	std::map<int, RAZhenPinShangPuRewardGroupCfg > m_reward_group_cfg;
	RAZhenPinShangPuOtherCfg m_other_cfg;
};

#endif	