#ifndef __RAND_ACTIVITY_YUN_ZE_BLOOMING_RECORD_CONFIG_HPP__
#define __RAND_ACTIVITY_YUN_ZE_BLOOMING_RECORD_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"

struct RAYZBloomingBasicsCfg
{
	RAYZBloomingBasicsCfg():integral(0), reward_group(0)
	{}

	int integral;
	int reward_group;
};

struct RAYZBloomingRecordRewardCfg
{
	RAYZBloomingRecordRewardCfg():reward_group(0)
	{}

	int reward_group;
	std::vector<ItemConfigData> reward_list;
};

struct RAYZBloomingRecordRewardSectionCfg
{
	RAYZBloomingRecordRewardSectionCfg():section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RAYZBloomingRecordRewardCfg> reward_group_list;
};

struct RAYZBloomingRecordOtherCfg
{
	RAYZBloomingRecordOtherCfg():time(0), frequency(0), frequency_total(0)
	{}

	int time;
	int frequency;
	int frequency_total;
};


class RandActivityManager;
class RandActivityYunZeBloomingRecordConfig : public RandActivityConfig
{
public:
	RandActivityYunZeBloomingRecordConfig();
	virtual ~RandActivityYunZeBloomingRecordConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAYZBloomingRecordRewardCfg * GetRewardByIntegralCfg(RandActivityManager * ramgr, int reward_group)const;
	const RAYZBloomingRecordOtherCfg & GetOtherCfg()const { return m_other_cfg; }
	bool IsCanReceiveReward(int cur_integral, int reward_group)const;

private:
	int InitBasicsCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);


	std::vector<RAYZBloomingBasicsCfg> m_basics_cfg;
	std::vector<RAYZBloomingRecordRewardSectionCfg> m_reward_cfg;
	RAYZBloomingRecordOtherCfg m_other_cfg;
};


#endif