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
	
	int mine_monster_count_min;				//!< һ�鰵�׹����ٹ�����
	int mine_monster_count_max;				//!< һ�鰵�׹���������
	int mine_interval_min_s;				//!< ���ϰ��׹ֵ���̼��ʱ�� 
	int mine_interval_max_s;				//!< ���ϰ��׹ֵ�����ʱ��
	int mine_monster_level_min;				//!< ���׹ֵ���͵ȼ�
	int mine_monster_level_max;				//!< ���׹ֵ���ߵȼ�
	std::vector<int> mine_monsters;			//!< ���׹������б�  ��ÿ��Ԫ�ش������ID�� ����Щ����ƴ����һ��õ�һ�鰵�׹�)
	std::vector<int> drop_list;				//!< �����б�
	int prestige_id;						//!< ��������id
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
	std::map<int, int> m_active_limit_cfg;				// ��Ծ���Ƶ��� key:min_level+1000*max_level 
};

#endif