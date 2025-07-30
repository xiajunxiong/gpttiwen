#ifndef __LING_YAO_JIU_ZHU_CONFIG_HPP__
#define __LING_YAO_JIU_ZHU_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "gamedef.h"
#include "npc/npcpool.h"

#include <map>
#include <vector>

struct LYJZOtherConfig
{
	LYJZOtherConfig() : special_task_id(0), last_task_id(0), special_talk(0), restart_talk(0), task_item_id(0), yjcnpc_ID(0), scene_ID(0), x(0), y(0), dir_y(0), npc_id_1(0), special_bubble_1(0){}
	int special_task_id;
	int last_task_id;
	int special_talk;
	int restart_talk;
	int task_item_id;
	int yjcnpc_ID;
	int scene_ID;
	int x;
	int y;
	int dir_y;
	int npc_id_1;
	int special_bubble_1;
};

class LingYaoJiuZhuFbConfig : public ILogicConfig
{
public:
	LingYaoJiuZhuFbConfig();
	virtual ~LingYaoJiuZhuFbConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const LYJZOtherConfig * GetOtherCfg() { return &m_other_cfg; }
private:
	int InitOhterCfg(TiXmlElement *RootElement);
	LYJZOtherConfig m_other_cfg;
};

#endif