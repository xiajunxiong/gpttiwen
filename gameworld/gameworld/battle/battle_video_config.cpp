#include "battle_video_config.hpp"
#include "servercommon/struct/battle/battle_def.hpp"
#include "monster/monster_group_pool.hpp"

BattleVideoConfig::BattleVideoConfig()
{

}

BattleVideoConfig::~BattleVideoConfig()
{

}

bool BattleVideoConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("videotape_list", InitVideoMonsterGroupList);

	return true;
}

const VideoCfg* BattleVideoConfig::GetVideoCfg(int monster_group_id)
{
	VideoCfgMap::iterator it = m_video_cfg_map.find(monster_group_id);
	if (it != m_video_cfg_map.end())
	{
		return &it->second;
	}
	return NULL;
}

int BattleVideoConfig::InitVideoMonsterGroupList(TiXmlElement *RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		VideoCfg cfg;
		if (!GetSubNodeValue(dataElement, "battle_mode", cfg.battle_mode) || cfg.battle_mode <= BATTLE_MODE_INVALID || cfg.battle_mode >= BATTLE_MODE_MAX)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "monster_group_id", cfg.monster_group_id) || NULL == MonsterGroupPool::GetInstance().GetMonsterGroupCfg(cfg.monster_group_id))
		{
			printf("BattleVideoConfig::InitVideoMonsterGroupList monster_group_id[%d] not exist\n", cfg.monster_group_id);
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "version", cfg.version) || cfg.version < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "is_cross", cfg.is_cross) || cfg.is_cross < 0)
		{
			return -4;
		}

		VideoCfgMap::iterator it = m_video_cfg_map.find(cfg.monster_group_id);
		if (it != m_video_cfg_map.end())
		{
			return -5;
		}
		m_video_cfg_map.insert(VideoCfgMap::value_type(cfg.monster_group_id, cfg));

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

