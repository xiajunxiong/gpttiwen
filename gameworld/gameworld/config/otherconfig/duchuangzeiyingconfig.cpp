#include "duchuangzeiyingconfig.hpp"
#include "gameworld/gameworld/monster/monster_group_pool.hpp"
#include "checkresourcecenter.hpp"

DuChuangZeiYingConfig::DuChuangZeiYingConfig()
{

}

DuChuangZeiYingConfig::~DuChuangZeiYingConfig()
{

}

bool DuChuangZeiYingConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("patrol", InitPatrolCfg);
	LOAD_CONFIG("bandit_camp", InitSceneCfg);
	LOAD_CONFIG("level_monster_group", InitMonterCfg);

	return true;
}

const DuChuangZeiYingSceneCfg * DuChuangZeiYingConfig::GetSceneCfg(int layer)
{
	if(layer < 0 || layer >= MAX_SCENE_LAYER_NUM) return NULL;

	return &m_scene_cfg[layer];
}

int DuChuangZeiYingConfig::GetMonsterGroupId(int npc_id, int level)
{
	if(npc_id < 0 || level <= 0 || level > MAX_ROLE_LEVEL) return 0;

	std::map<int, DuChuangZeiYingMonterLevelGroupCfg>::iterator it = m_npc_monster_group_map.find(npc_id);
	if(it == m_npc_monster_group_map.end()) return 0;

	for (int i = 0; i < it->second.count && i < MAX_LEVEL_GROUP_NUM; i++)
	{
		if (it->second.node_list[i].min_level <= level && it->second.node_list[i].max_level >= level)
		{
			return it->second.node_list[i].monster_group_id;
		}
	}

	return 0;
}

void DuChuangZeiYingConfig::GetPatrolCfg(std::vector<SpecialLogicDuChuangZeiYingObjInfo>* patrol_cfg)
{
	if(NULL == patrol_cfg) return;
	patrol_cfg->clear();

	for (int i = 0; i < MAX_SCENE_PATROL_NUM; i++)
	{
		SpecialLogicDuChuangZeiYingObjInfo node;
		node.npc_id = m_patrol_cfg[i];
		node.status = 1;
		patrol_cfg->push_back(node);
	}
}

int DuChuangZeiYingConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(data_element, "max_player", m_other_cfg.max_team_num) || m_other_cfg.max_team_num != 1) return -1;

	return 0;
}

int DuChuangZeiYingConfig::InitSceneCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	scene_layer_count = 0;
	while (NULL != data_element)
	{
		DuChuangZeiYingSceneCfg node;
		if (!GetSubNodeValue(data_element, "level", node.layer) || node.layer <= 0 || node.layer > MAX_SCENE_LAYER_NUM) return -1;
		if (!GetSubNodeValue(data_element, "scene_id", node.scene_id) || node.scene_id <= 0) return -2;
		if (!GetSubNodeValue(data_element, "transport_id", node.transport_id) || node.transport_id < 0) return -3;

		if (!GetSubNodeValue(data_element, "pos_x", node.transport_pos.x) || node.transport_pos.x < 0) return -4;
		if (!GetSubNodeValue(data_element, "pos_y", node.transport_pos.y) || node.transport_pos.y < 0) return -5;

		if (!GetSubNodeValue(data_element, "pos_x1", node.born_pos.x) || node.born_pos.x < 0) return -6;
		if (!GetSubNodeValue(data_element, "pos_y1", node.born_pos.y) || node.born_pos.y < 0) return -7;

		if (!GetSubNodeValue(data_element, "npc_id", node.npc_id) || node.npc_id < 0) return -8;

		if (!GetSubNodeValue(data_element, "npc_x", node.npc_pos.x) || node.npc_pos.x < 0) return -9;
		if (!GetSubNodeValue(data_element, "npc_y", node.npc_pos.y) || node.npc_pos.y < 0) return -10;
		if (!GetSubNodeValue(data_element, "npc_dir", node.npc_dir) || node.npc_dir < 0) return -11;

		if (!GetSubNodeValue(data_element, "boss_npc_id", node.boss_npc_id) || node.boss_npc_id < 0) return -12;

		if (!GetSubNodeValue(data_element, "boss_x", node.boss_npc_pos.x) || node.boss_npc_pos.x < 0) return -13;
		if (!GetSubNodeValue(data_element, "boss_y", node.boss_npc_pos.y) || node.boss_npc_pos.y < 0) return -14;
		if (!GetSubNodeValue(data_element, "boss_dir", node.boss_npc_dir) || node.boss_npc_dir < 0) return -15;


		if (scene_layer_count == 0)
		{
			if(0 == node.transport_id) return -333;
		}
		else
		{
			if(0 == node.npc_id || 0 == node.boss_npc_id) return -444;
		}

		if(scene_layer_count >= MAX_SCENE_LAYER_NUM) return -666;
		CheckResourceCenter::Instance().GetSceneCheck()->Add(node.scene_id, __FUNCTION__);
		m_scene_cfg[scene_layer_count++] = node;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int DuChuangZeiYingConfig::InitMonterCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	
	while (NULL != data_element)
	{
		DuChuangZeiYingMonterLevelGroupCfg::DuChuangZeiYingMonsterGroupCfg node;
		int npc_id;
		if (!GetSubNodeValue(data_element, "npc_id", npc_id) || npc_id <= 0) return -1;

		if (!GetSubNodeValue(data_element, "min_level", node.min_level) || node.min_level <= 0 || node.min_level > MAX_ROLE_LEVEL) return -2;
		if (!GetSubNodeValue(data_element, "max_level", node.max_level) || node.max_level < node.min_level || node.max_level > MAX_ROLE_LEVEL) return -3;
		if (!GetSubNodeValue(data_element, "monster_group_id", node.monster_group_id) ||  NULL == MonsterGroupPool::GetInstance().GetMonsterGroupCfg(node.monster_group_id)) return -4;

		if(m_npc_monster_group_map[npc_id].count >= MAX_LEVEL_GROUP_NUM) return -666;

		m_npc_monster_group_map[npc_id].node_list[m_npc_monster_group_map[npc_id].count++] = node;
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int DuChuangZeiYingConfig::InitPatrolCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	int count = 0;
	while (NULL != data_element)
	{
		int npc_id = 0;
		if (!GetSubNodeValue(data_element, "npc_id", npc_id) || npc_id <= 0) return -1;

		if(count >= MAX_SCENE_PATROL_NUM) return -444;

		m_patrol_cfg[count++] = npc_id;
		data_element = data_element->NextSiblingElement();
	}

	if(MAX_SCENE_PATROL_NUM != count) return -888;

	return 0;
}
