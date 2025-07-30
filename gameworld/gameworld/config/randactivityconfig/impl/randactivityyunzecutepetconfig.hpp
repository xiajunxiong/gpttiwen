#ifndef __RAND_ACTIVITY_YUN_ZE_CUTE_PET_CONFIG_HPP__
#define __RAND_ACTIVITY_YUN_ZE_CUTE_PET_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itempool.h"
#include "servercommon/activitydef.hpp"

struct RAYZCutePetBasicsCfg
{
	RAYZCutePetBasicsCfg():combat(0), reward_group(0)
	{}

	int combat;
	int reward_group;
};

struct RAYZCutePetTaskCfg
{
	RAYZCutePetTaskCfg():seq(0),task_id(0), task_state(0), task_combat(0)
	{}

	int seq;
	int task_id;
	int task_state;
	int task_combat;
};

struct RAYZCutePetTaskSectionCfg
{
	RAYZCutePetTaskSectionCfg():section_day(0)
	{}

	int section_day;			// �����������
	std::vector<RAYZCutePetTaskCfg> task;
};

struct RAYZCutePetRewardCfg
{
	RAYZCutePetRewardCfg()
	{}

	int reward_group;
	std::vector<ItemConfigData> reward;
};

struct RAYZCutePetRewardSectionCfg
{
	RAYZCutePetRewardSectionCfg():section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RAYZCutePetRewardCfg> reward_list;
};



class RandActivityManager;
class RandActivityYunZeCutePetConfig : public RandActivityConfig
{
public:
	RandActivityYunZeCutePetConfig();
	virtual ~RandActivityYunZeCutePetConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAYZCutePetBasicsCfg * GetMonsterByIdCfg(int monster_id)const;
	int GetTaskCfg(RandActivityManager* ramgr,ARG_OUT std::vector<RAYZCutePetTaskCfg> & task_out)const;
	const RAYZCutePetRewardCfg * GetRewardCfg(RandActivityManager* ramgr, int reward_group)const;
	int GetMonsterNumCfg()const { return (int)m_basics_cfg.size(); }

private:
	int InitBasicsCfg(TiXmlElement *RootElement);
	int InitTaskCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);


	std::map<int, RAYZCutePetBasicsCfg> m_basics_cfg;			// ��������	key-����id	
	std::vector<RAYZCutePetTaskSectionCfg> m_task_cfg;		// ��������
	std::vector<RAYZCutePetRewardSectionCfg> m_reward_section_cfg;	// ��������
};





#endif