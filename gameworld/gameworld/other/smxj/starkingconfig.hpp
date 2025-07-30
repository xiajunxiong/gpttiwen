#ifndef __STAR_KING_CONFIG_HPP__
#define __STAR_KING_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "gamedef.h"
#include "npc/npcpool.h"

#include <map>
#include <vector>

struct StarKingConfig
{
	StarKingConfig() : scene_id(0), npc_id(0), pos_x1(0), pos_y1(0), is_map(0){}
	int scene_id;
	int npc_id;
	int pos_x1;
	int pos_y1;
	int is_map;
};

struct StarKingOtherConfig
{
	StarKingOtherConfig() : task_id(0), last_task_id(0), npc_id(0), level(0){}
	int task_id;
	int last_task_id;
	int npc_id;
	int level;
};

class StarKingFbConfig : public ILogicConfig
{
public:
	StarKingFbConfig();
	virtual ~StarKingFbConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const StarKingOtherConfig * GetOtherCfg() { return &m_other_cfg; }
	const StarKingConfig * GetConfig() { return &m_star_king_cfg; }
private:
	int InitOhterCfg(TiXmlElement *RootElement);
	int InitConfig(TiXmlElement *RootElement);
	StarKingOtherConfig m_other_cfg;
	StarKingConfig m_star_king_cfg;
};

#endif