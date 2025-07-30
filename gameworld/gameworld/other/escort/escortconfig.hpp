#ifndef __ESCORT_CONFIG_HPP__
#define __ESCORT_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include <vector>
#include <map>
#include <algorithm>

enum ESCORT_TYPE
{
	ESCORT_TYPE_MIN = 0,
	ESCORT_TYPE_NORMAL,
	ESCORT_TYPE_SENIOR,
	ESCORT_TYPE_CHALLENGE_TASK,
	ESCORT_TYPE_OTHER_TASK,
	ESCORT_TYPE_MAX,
};

struct EscortOtherCfg
{
	EscortOtherCfg()
	{
		daily_num = 0;
		normal_open_level = 0;
		senior_open_level = 0;
		accept_npc = 0;
		slow_down = 0;
		offline_protect_time = 0;
		time_limit = 0;
	}
	int daily_num;
	int normal_open_level;
	int senior_open_level;
	int accept_npc;
	int slow_down;
	int offline_protect_time;
	int time_limit;
};

struct EscortMonsterGroupCfg
{
	EscortMonsterGroupCfg()
	{
		total_weight = 0;
	}

	int total_weight;
	std::map<int, int> monster_group;
};

static const int ESCORT_REWARD_MAX_NUM = 5;
struct EscortTaskCfg
{
	EscortTaskCfg()
	{
		seq = 0;
		min_level = 0;
		max_level = 0;
		reward_count = 0;
		deposit = 0;
		reward_coin = 0;
		progress_desc = 0;
		random_monster = 0;
		NPC_group = 0;
		task_id = 0;
	}

	int seq;
	int min_level;
	int max_level;

	int reward_count;
	ItemConfigData reward_list[ESCORT_REWARD_MAX_NUM];

	int deposit;
	int reward_coin;
	int progress_desc;
	int random_monster;
	int NPC_group;
	int task_id;
};

struct EscortNpcCfg
{
	EscortNpcCfg()
	{
		npc_id=0;
		rate=0;
		village_id = 0;
		reward_prestige = 0;
	}

	int npc_id;
	int rate;
	int village_id;
	int reward_prestige;
};


class EscortConfig : public ILogicConfig
{
public:
	EscortConfig();
	virtual ~EscortConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const EscortTaskCfg* GetTaskCfg(int seq);
	const EscortOtherCfg& GetOtherCfg() { return m_other_cfg; }
	int GetTaskSeq(int level, int task_type, int task_id);
	int RandNpc(int seq);
	int RandMonsterGroup(int seq);
	const EscortNpcCfg* GetEscortNpcCfg(int npc_group, int npc_id);
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitMonsterGroupCfg(TiXmlElement *RootElement);
	int InitNpcGroupCfg(TiXmlElement *RootElement);
	int InitEscortTaskCfg(TiXmlElement *RootElement);

	EscortOtherCfg m_other_cfg;
	std::map<int, EscortMonsterGroupCfg> m_monster_group_cfg;

	std::map<int, int> m_npc_group_total_weight;
	std::map<int, std::vector<EscortNpcCfg> > m_npc_group_cfg;
	std::map<int, EscortTaskCfg> m_escort_task_cfg;

};

#endif