#ifndef __RAND_ACTIVITY_FORTUNE_MAZE_CONFIG_HPP__
#define __RAND_ACTIVITY_FORTUNE_MAZE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rafortunemazeparam.hpp"
#include <vector>

struct OtherCfg
{
	OtherCfg() : scene_id(0), weight(0), npc_id(0), reward_group_id(0), box_pos(0, 0) {}
	int scene_id;
	int weight;
	int npc_id;
	int reward_group_id;
	Posi box_pos;
};

struct OtherSectionCfg
{
	OtherSectionCfg(int _section_start, int _section_end) : section_start(_section_start), section_end(_section_end)
	{
		m_other_weight = 0;
	}

	int section_start;
	int section_end;

	int m_other_weight;
	std::vector<OtherCfg> m_other_vec;
};

struct FortuneMazeTaskCfg
{
	FortuneMazeTaskCfg() : seq(0), seq1(0), type(0), parameter(0), open_number(0) {}
	int seq;
	int seq1;
	int type;
	int	parameter;
	int	open_number;
};

struct FortuneMazeTaskListCfg
{
	FortuneMazeTaskListCfg() {}
	int section_start;
	int section_end;

	std::vector<FortuneMazeTaskCfg> task_list;
};

struct FortuneMazeRewardGroupCfg
{
	FortuneMazeRewardGroupCfg() : reward_group_id(0), hearsay(0){}
	int reward_group_id;
	int hearsay;
	std::vector<ItemConfigData> reward_vec;
};

struct FortuneMazeRewardGroupSectionCfg
{
	FortuneMazeRewardGroupSectionCfg(int _section_start, int _section_end) : section_start(_section_start), section_end(_section_end)
	{}

	int section_start;
	int section_end;

	std::map<int, FortuneMazeRewardGroupCfg> m_reward_map;
};

class RandActivityFortuneMazeConfig : public RandActivityConfig
{
public:
	RandActivityFortuneMazeConfig();
	virtual ~RandActivityFortuneMazeConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const OtherCfg * GetRandSceneId(RandActivityManager * ramgr)const;
	const OtherCfg * GetCfgBySceneId(int scene_id, RandActivityManager * ramgr)const;
	const FortuneMazeTaskListCfg * GetFortuneMazeTaskListConfig(RandActivityManager* ramgr)const;
	const FortuneMazeRewardGroupCfg * GetFortuneMazeRewardGroupConfig(int group_id, RandActivityManager * ramgr)const;
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitFortuneMazeTaskListCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);

	std::vector<OtherSectionCfg> m_other_section_vec;
	std::vector<FortuneMazeTaskListCfg> m_fortune_maze_map;
	std::vector<FortuneMazeRewardGroupSectionCfg> m_reward_section_cfg;
};

#endif