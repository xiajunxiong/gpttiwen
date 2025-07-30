#ifndef __CATCH_FOX_CONFIG_HPP__
#define __CATCH_FOX_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "gamedef.h"
#include "npc/npcpool.h"

#include <map>
#include <vector>

struct CatchFoxFindFoxConfig
{
	CatchFoxFindFoxConfig() : level(0), scene_id(0), pos_x1(0), pos_y1(0){}
	int level;
	int scene_id;
	int pos_x1;
	int pos_y1;
};

struct CatchFoxOtherConfig
{
	CatchFoxOtherConfig() : task_id(0), last_task_id(0), npc_id(0), level(0), fox_npc_id(0){}
	int task_id;
	int last_task_id;
	int npc_id;
	int level;
	int fox_npc_id;
};

struct CatchFoxMonsterConfig
{
	CatchFoxMonsterConfig() : npc_id(0), min_level(0), max_level(0), monster_group_id(0){}
	int npc_id;
	int min_level;
	int max_level;
	int monster_group_id;
};

class CatchFoxFbConfig : public ILogicConfig
{
public:
	CatchFoxFbConfig();
	virtual ~CatchFoxFbConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const CatchFoxOtherConfig * GetOtherCfg() { return &m_other_cfg; }
	const CatchFoxFindFoxConfig * GetFindFoxConfig() { return &m_catch_fox_cfg; }
	const CatchFoxMonsterConfig * GetMonsterConfig(int level);
private:
	int InitOhterCfg(TiXmlElement *RootElement);
	int InitFindFoxConfig(TiXmlElement *RootElement);
	int InitLevelMonsterGroup(TiXmlElement *RootElement);

	CatchFoxOtherConfig m_other_cfg;
	CatchFoxFindFoxConfig m_catch_fox_cfg;

	std::vector<CatchFoxMonsterConfig> m_monster_cfg_vec;
};

#endif