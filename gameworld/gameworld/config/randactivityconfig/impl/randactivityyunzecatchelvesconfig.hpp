#ifndef __RAND_ACTIVITY_YUN_ZE_CATCH_ELVES_CONFIG_HPP__
#define __RAND_ACTIVITY_YUN_ZE_CATCH_ELVES_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"

struct RAYZCatchElvesBasicsCfg
{
	RAYZCatchElvesBasicsCfg():elves_id(0), rate(0), reward_group(0), extra_reward_group(0)
	{}

	int elves_id;
	int rate;
	int reward_group;
	int extra_reward_group;			// 0-无额外奖励
};

struct RAYZCatchElvesRewardCfg
{
	RAYZCatchElvesRewardCfg():reward_group(0)
	{}

	int reward_group;
	std::vector<ItemConfigData> reward;
};

struct RAYZCatchElvesRewardSectionCfg
{
	RAYZCatchElvesRewardSectionCfg():section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RAYZCatchElvesRewardCfg> reward_list;
};

struct RAYZCatchElvesOtherCfg
{
	RAYZCatchElvesOtherCfg():consume_item(0), consume_num(0)
	{}

	int consume_item;			// 消耗道具id
	int consume_num;			// 消耗道具数量
};


class RandActivityManager;
class RandActivityYunZeCatchElvesConfig : public RandActivityConfig
{
public:
	RandActivityYunZeCatchElvesConfig();
	virtual ~RandActivityYunZeCatchElvesConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAYZCatchElvesBasicsCfg * GetBasicsByElvesCfg(int elves_id)const;
	const RAYZCatchElvesBasicsCfg * GetRandRewardCfg()const;
	const RAYZCatchElvesRewardCfg * GetRewardByGroup(RandActivityManager* ramgr,int reward_group)const;
	const RAYZCatchElvesOtherCfg & GetOtherCfg()const { return m_other_cfg; }
	int GetElvesNumCfg()const { return (int)m_basics_cfg.size(); }

private:
	int InitBasicsCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	int m_total_rate;
	std::vector<RAYZCatchElvesBasicsCfg> m_basics_cfg;
	std::vector<RAYZCatchElvesRewardSectionCfg> m_reward_section_cfg;
	RAYZCatchElvesOtherCfg m_other_cfg;
};

#endif