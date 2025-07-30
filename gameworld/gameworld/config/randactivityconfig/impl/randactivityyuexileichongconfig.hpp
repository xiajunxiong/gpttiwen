#ifndef __RAND_ACTIVITY_YUE_XI_LEI_CHONG_CONFIG_HPP__
#define __RAND_ACTIVITY_YUE_XI_LEI_CHONG_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itempool.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/roleactivity/rayuexidengmiparam.hpp"

struct RAYueXiLeiChongRewardCfg
{
	RAYueXiLeiChongRewardCfg():acc_recharge_down(0), gold_reward(0)
	{}

	int acc_recharge_down;		// 累充下限(表里有配上限,但感觉不太需要)
	int gold_reward;			// 元宝奖励
	std::vector<ItemConfigData> reward_list;
};

struct RAYueXiLeiChongRewardSectionCfg
{
	RAYueXiLeiChongRewardSectionCfg() :section_start(0), section_end(0)
	{}

	bool IsInclude(int real_open_server_day) const
	{
		return (section_start <= real_open_server_day && real_open_server_day <= section_end);
	}

	int section_start;
	int section_end;
	std::vector<RAYueXiLeiChongRewardCfg> reward_cfg;
};

class RandActivityYueXiLeiChongConfig : public RandActivityConfig
{
public:
	RandActivityYueXiLeiChongConfig();
	virtual ~RandActivityYueXiLeiChongConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAYueXiLeiChongRewardCfg* GetRewardCfg(RandActivityManager* ramgr, int seq)const;

private:
	int InitRewardCfg(TiXmlElement* RootElement, std::string& err);

	std::vector<RAYueXiLeiChongRewardSectionCfg> m_reward_section_cfg;
};

#endif