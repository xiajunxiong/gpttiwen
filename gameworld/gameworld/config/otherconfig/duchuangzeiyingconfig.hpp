#ifndef __DU_CHUANG_ZEI_YING_CONFIG_HPP__
#define __DU_CHUANG_ZEI_YING_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "protocol/msgduchuangzeiying.hpp"

static const int MAX_LEVEL_GROUP_NUM = 12;
static const int MAX_SCENE_LAYER_NUM = 2;
static const int MAX_SCENE_PATROL_NUM = 4;

using namespace Protocol;
struct DuChuangZeiYingOtherCfg
{
	DuChuangZeiYingOtherCfg() : max_team_num(0)
	{}

	int max_team_num;
};

struct DuChuangZeiYingSceneCfg
{
	DuChuangZeiYingSceneCfg(): layer(0), scene_id(0), npc_id(0), npc_dir(0), boss_npc_id(0), boss_npc_dir(0)
	{
		transport_pos = Posi(0, 0);
		born_pos = Posi(0, 0);
		npc_pos = Posi(0, 0);
		boss_npc_pos = Posi(0, 0);
	}

	int layer;					//当前层数
	int scene_id;				//当前场景ID
	int transport_id;			//传送阵传送到的场景ID
	Posi transport_pos;			//传送阵坐标
	Posi born_pos;				//出生点位
	int npc_id;
	Posi npc_pos;	
	Dir npc_dir;			
	int boss_npc_id;			
	Posi boss_npc_pos;			//坐标
	Dir boss_npc_dir;			
};

struct DuChuangZeiYingMonterLevelGroupCfg
{
	DuChuangZeiYingMonterLevelGroupCfg() : count(0)
	{}


	struct DuChuangZeiYingMonsterGroupCfg
	{
		DuChuangZeiYingMonsterGroupCfg() : min_level(0), max_level(0), monster_group_id(0)
		{}

		int min_level;
		int max_level;
		int monster_group_id;
	};

	int count;
	DuChuangZeiYingMonsterGroupCfg node_list[MAX_LEVEL_GROUP_NUM];
};

class DuChuangZeiYingConfig : public ILogicConfig
{
public:
	DuChuangZeiYingConfig();
	virtual ~DuChuangZeiYingConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const DuChuangZeiYingOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const DuChuangZeiYingSceneCfg * GetSceneCfg(int layer);
	int GetMonsterGroupId(int npc_id, int level);
	void GetPatrolCfg(std::vector<SpecialLogicDuChuangZeiYingObjInfo> * patrol_cfg);

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitSceneCfg(TiXmlElement *RootElement);
	int InitMonterCfg(TiXmlElement *RootElement);
	int InitPatrolCfg(TiXmlElement *RootElement);

	DuChuangZeiYingOtherCfg m_other_cfg;
	int scene_layer_count;
	DuChuangZeiYingSceneCfg m_scene_cfg[MAX_SCENE_LAYER_NUM];							//副本场景配置
	std::map<int, DuChuangZeiYingMonterLevelGroupCfg> m_npc_monster_group_map;			//key为npc_boss_id
	int m_patrol_cfg[MAX_SCENE_PATROL_NUM];						//巡逻配置
};

#endif