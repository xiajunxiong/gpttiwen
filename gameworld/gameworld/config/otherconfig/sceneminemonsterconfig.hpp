#ifndef __SCENE_MINE_MONSTER_CONFIG_HPP__
#define __SCENE_MINE_MONSTER_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include <vector>
#include <map>
#include <set>

struct SceneMineMonsterCfg
{
	SceneMineMonsterCfg() : mine_monster_count_min(0), mine_monster_count_max(0),
		mine_interval_min_s(0), mine_interval_max_s(0), mine_monster_level_min(0),
		mine_monster_level_max(0), prestige_id(0)
	{
		mine_monsters.clear();
		drop_list.clear();
	}
	
	int mine_monster_count_min;				//!< 一组暗雷怪最少怪物数
	int mine_monster_count_max;				//!< 一组暗雷怪最多怪物数
	int mine_interval_min_s;				//!< 遇上暗雷怪的最短间隔时间 
	int mine_interval_max_s;				//!< 遇上暗雷怪的最长间隔时间
	int mine_monster_level_min;				//!< 暗雷怪的最低等级
	int mine_monster_level_max;				//!< 暗雷怪的最高等级
	std::vector<int> mine_monsters;			//!< 暗雷怪种类列表  （每个元素代表怪物ID， 将这些种类拼凑在一起得到一组暗雷怪)
	std::vector<int> drop_list;				//!< 掉落列表
	int prestige_id;						//!< 声望村子id
};

class SceneMineMonsterConfig
{
public:
	SceneMineMonsterConfig();
	~SceneMineMonsterConfig();

	static SceneMineMonsterConfig& Instance();

	bool Init(const std::string &configname, std::string *err);
	static bool Reload(const std::string& configname, std::string *err);
	
	const SceneMineMonsterCfg * GetSceneMineMonsterCfg(int scene_id);
	bool IsMeetActive(int role_level, int active_num);

private:
	int InitSceneMineMonster(TiXmlElement *RootElement);
	int InitActiveLimit(TiXmlElement *RootElement);

	std::map<int, SceneMineMonsterCfg> m_scene_id_to_scene_mine_cfg;
	std::map<int, int> m_active_limit_cfg;				// 活跃限制掉落 key:min_level+1000*max_level 
};

#endif