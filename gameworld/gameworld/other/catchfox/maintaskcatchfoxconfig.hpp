#ifndef __MAIN_TASK_CATCH_FOX_CONFIG_HPP__
#define __MAIN_TASK_CATCH_FOX_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "gamedef.h"
#include "npc/npcpool.h"

#include <map>
#include <vector>

struct MainTaskCatchFoxFindFoxConfig
{
	MainTaskCatchFoxFindFoxConfig() : level(0), scene_id(0), pos_x1(0), pos_y1(0){}
	int level;
	int scene_id;
	int pos_x1;
	int pos_y1;
};

struct MainTaskCatchFoxOtherConfig
{
	MainTaskCatchFoxOtherConfig() : task_id(0), last_task_id(0), npc_id(0), level(0), fox_npc_id(0){}
	int task_id;
	int last_task_id;
	int npc_id;
	int level;
	int fox_npc_id;
};

struct MainTaskCatchFoxMonsterConfig
{
	MainTaskCatchFoxMonsterConfig() : npc_id(0), min_level(0), max_level(0), monster_group_id(0){}
	int npc_id;
	int min_level;
	int max_level;
	int monster_group_id;
};

class MainTaskCatchFoxFbConfig : public ILogicConfig
{
public:
	MainTaskCatchFoxFbConfig();
	virtual ~MainTaskCatchFoxFbConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const MainTaskCatchFoxOtherConfig * GetOtherCfg() { return &m_other_cfg; }
	const MainTaskCatchFoxFindFoxConfig * GetFindFoxConfig() { return &m_catch_fox_cfg; }
	const MainTaskCatchFoxMonsterConfig * GetMonsterConfig(int level);
private:
	int InitOhterCfg(TiXmlElement *RootElement);
	int InitFindFoxConfig(TiXmlElement *RootElement);
	int InitLevelMonsterGroup(TiXmlElement *RootElement);

	MainTaskCatchFoxOtherConfig m_other_cfg;
	MainTaskCatchFoxFindFoxConfig m_catch_fox_cfg;

	std::vector<MainTaskCatchFoxMonsterConfig> m_monster_cfg_vec;
};

#endif