#ifndef __MAP_EXPLORATION_CONFIG_HPP__
#define __MAP_EXPLORATION_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include <map>
#include <vector>
#include "servercommon/serverdef.h"
#include "servercommon/struct/itemlistparam.h"

struct MapExplorationCfg
{
	MapExplorationCfg() : scene_id(0), npc_pos(0, 0), reward_group_id(0) {}

	int scene_id;
	Posi npc_pos;
	int reward_group_id;
};

struct MapExplorationOtherCfg
{
	MapExplorationOtherCfg() : npc_radius(0) {}

	int npc_radius;
};

struct MapExplorationRewardCfg
{
	MapExplorationRewardCfg() : reward_group(0) {}

	int reward_group;
	std::vector<ItemConfigData> item_list;
};

class MapExplorationConfig : public ILogicConfig
{
public:
	MapExplorationConfig();
	virtual ~MapExplorationConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const MapExplorationCfg * GetExplorationCfg(int seq);
	const MapExplorationRewardCfg * GetExplorationRewardCfg(int reward_id);
private:
	int InitMapExplorationCfg(TiXmlElement *RootElement);
	int InitMapExplorationRewardCfg(TiXmlElement *RootElement);

	MapExplorationOtherCfg m_other_cfg;
	std::map<int, MapExplorationCfg> m_exploration_map;
	std::map<int, MapExplorationRewardCfg> m_reward_map;
};

#endif