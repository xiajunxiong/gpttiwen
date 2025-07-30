#ifndef __RAND_ACTIVITY_TIGER_TEASE_NEW_YEAR_CONFIG_HPP__
#define __RAND_ACTIVITY_TIGER_TEASE_NEW_YEAR_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itempool.h"
#include "servercommon/activitydef.hpp"

// 基础配置
struct RATTNYBasisCfg
{
	RATTNYBasisCfg():win_rate_group(0), fail_rate_group(0)
	{}

	int win_rate_group;
	std::vector<ItemConfigData> win_reward_list;
	int fail_rate_group;
	std::vector<ItemConfigData> fail_reward_list;
};

struct RATTNYBasisSectionCfg
{
	RATTNYBasisSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	RATTNYBasisCfg reward_cfg;		
};

// 奖励组
struct RATTNYRewardCfg
{
	RATTNYRewardCfg():rate(0)
	{}

	int rate;
	std::vector<ItemConfigData> reward_list;
};

struct RATTNYRewardGroupCfg
{
	RATTNYRewardGroupCfg():reward_group(0), total_rate(0)
	{}

	int reward_group;
	int total_rate;
	std::vector<RATTNYRewardCfg> reward_cfg;
};

// 其他
struct RATTNYOtherCfg
{
	RATTNYOtherCfg():day_num(0)
	{}

	int day_num;
};


class RandActivityManager;
class RandActivityTigerTeaseNewYearConfig : public RandActivityConfig
{
public:
	RandActivityTigerTeaseNewYearConfig();
	virtual ~RandActivityTigerTeaseNewYearConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RATTNYBasisCfg * GetBasisCfg(RandActivityManager * ramgr)const;
	const RATTNYRewardCfg * GetRandRewardCfg(int reward_group)const;
	const RATTNYOtherCfg & GetOtherCfg()const { return m_other_cfg; }

private:
	int InitBasisCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::vector<RATTNYBasisSectionCfg> m_basis_cfg;
	std::vector<RATTNYRewardGroupCfg> m_reward_cfg;
	RATTNYOtherCfg m_other_cfg;
};


#endif