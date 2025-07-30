#ifndef __RAND_ACTIVITY_YUN_ZE_TERRITORY_CONFIG_HPP__
#define __RAND_ACTIVITY_YUN_ZE_TERRITORY_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rayunzeterritoryparam.hpp"


struct RAYZTerritoryTaskRewardCfg
{
	RAYZTerritoryTaskRewardCfg():picture(0)
	{}

	int section_day;		// 活动天数
	int picture;			// 图片编号
	std::vector<ItemConfigData> item_list;
};

struct RAYZTerritoryRewardSectionCfg
{
	RAYZTerritoryRewardSectionCfg():section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RAYZTerritoryTaskRewardCfg> node_list;
};

struct RAYZTerritoryBasicsCfg
{
	RAYZTerritoryBasicsCfg():frequency_total(0), frequency(0)
	{}

	int frequency_total;		// 总探索张数
	int frequency;				// 每日探索次数
};


class RandActivityManager;
class RandActivityYunZeTerritoryConfig : public RandActivityConfig
{
public: 
	RandActivityYunZeTerritoryConfig();
	virtual ~RandActivityYunZeTerritoryConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAYZTerritoryBasicsCfg & GetBasicsCfg()const { return m_basics_cfg; }
	const RAYZTerritoryTaskRewardCfg * GetTaskRewardCfg(RandActivityManager* ramgr)const;

private:
	int InitTaskRrwardCfg(TiXmlElement *RootElement);
	int InitBasicsCfg(TiXmlElement *RootElement);


	RAYZTerritoryBasicsCfg m_basics_cfg;								// 任务基础配置
	std::vector<RAYZTerritoryRewardSectionCfg> m_reward_section_cfg;	// 任务奖励配置
};

#endif