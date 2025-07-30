#include <set>

#include "npc/npcpool.h"
#include "medalfbconfig.hpp"

MedalFbConfig::MedalFbConfig()
{
}

MedalFbConfig::~MedalFbConfig()
{
	std::vector<MedalMaterialItemCfg>().swap(m_fb_material_cfg);
}

bool MedalFbConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("medaltask", InitFbLevelCfg);
	LOAD_CONFIG("exchange", InitMaterialCfg);

	return true;
}

const MedalFbLevelCfg * MedalFbConfig::GetMedalFbLevelCfg(int fb_seq)
{
	std::map<int, MedalFbLevelCfg>::iterator iter = m_fb_level_cfg.find(fb_seq);

	if (iter != m_fb_level_cfg.end())
	{
		return &iter->second;
	}

	return NULL;
}

const MedalFbLayerCfg * MedalFbConfig::GetMedalFbLayerCfg(int fb_seq, int fb_layer)
{
	std::map<int, MedalFbLevelCfg>::iterator iter = m_fb_level_cfg.find(fb_seq);

	if (iter != m_fb_level_cfg.end())
	{
		const MedalFbLevelCfg & level_cfg = iter->second;

		int index = fb_layer - 1;
		if (0 <= index && index < (int)level_cfg.layer_cfg.size())
		{
			return &level_cfg.layer_cfg[index];
		}
	}

	return NULL;
}

bool MedalFbConfig::IsSingleFb(int fb_seq)
{
	const MedalFbLevelCfg * level_cfg =this->GetMedalFbLevelCfg(fb_seq);
	if (level_cfg == NULL)
	{
		return false;
	}

	return level_cfg->max_enter_role_num == 1;
}

const MedalMaterialItemCfg * MedalFbConfig::GetMedalMaterialItemCfg(int seq)
{
	if (seq < 0 || seq >= (int)m_fb_material_cfg.size())
	{
		return NULL;
	}

	return &m_fb_material_cfg[seq];
}

int MedalFbConfig::InitFbLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	std::set<int> check_repeat_scene_id;

	while (dataElement)
	{
		int seq = 0;
		if (!GetSubNodeValue(dataElement, "seq", seq))
		{
			return -1;
		}

		MedalFbLevelCfg & fb_level_cfg = m_fb_level_cfg[seq];

		MedalFbLayerCfg cfg;
		if (!GetSubNodeValue(dataElement, "scene_id", cfg.scene_id))
		{
			return -2;
		}

		if (check_repeat_scene_id.find(cfg.scene_id) != check_repeat_scene_id.end())
		{
			return -1998;
		}
		check_repeat_scene_id.insert(cfg.scene_id);


		if (!GetSubNodeValue(dataElement, "pos_x", cfg.transport_pos.x))
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "pos_y", cfg.transport_pos.y))
		{
			return -4;
		}


		if (!GetSubNodeValue(dataElement, "pos_x1", cfg.birth_pos.x))
		{
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "pos_y1", cfg.birth_pos.y))
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "min_level", fb_level_cfg.limit_role_level))
		{
			return -7;
		}

		if (!GetSubNodeValue(dataElement, "min_player", fb_level_cfg.min_enter_role_num))
		{
			return -8;
		}

		if (!GetSubNodeValue(dataElement, "max_player", fb_level_cfg.max_enter_role_num))
		{
			return -9;
		}

		if (!GetSubNodeValue(dataElement, "consume_item", fb_level_cfg.need_item_id))
		{
			return -10;
		}

		for (int i = 0; i < MEDAL_FB_LAYER_MAX_MING_MONSTER_NUM; ++i)
		{
			MedalFbLayerCfg::MonsterInfoItem monster_cfg;

			char npc_id_str[512] = { 0 };
			SNPRINTF(npc_id_str, sizeof(npc_id_str) - 1, "boss%d_npc_id", i);

			char boss_pos_x_str[512] = { 0 };
			SNPRINTF(boss_pos_x_str, sizeof(boss_pos_x_str) - 1, "boss_x%d", i);

			char boss_pos_y_str[512] = { 0 };
			SNPRINTF(boss_pos_y_str, sizeof(boss_pos_y_str) - 1, "boss_y%d", i);

			if (!GetSubNodeValue(dataElement, npc_id_str, monster_cfg.boss_npc_id))
			{
				continue;
			}

			if (!GetSubNodeValue(dataElement, boss_pos_x_str, monster_cfg.boss_pos.x))
			{
				return -9901;
			}

			if (!GetSubNodeValue(dataElement, boss_pos_y_str, monster_cfg.boss_pos.y))
			{
				return -9902;
			}

			char boss_dir[512] = { 0 };
			SNPRINTF(boss_dir, sizeof(boss_dir) - 1, "dir_y%d", i);
			if (!GetSubNodeValue(dataElement, boss_dir, monster_cfg.boss_dir))
			{
				return -9903;
			}

			cfg.monster_item.push_back(monster_cfg);
		}

		if (GetSubNodeValue(dataElement, "npc_id", cfg.npc_id))
		{
			if (NULL == NPCPOOL->GetServerNPCInfo(cfg.npc_id))
			{
				return -__LINE__;
			}

			if (!GetSubNodeValue(dataElement, "npc_x", cfg.npc_pos.x) || cfg.npc_pos.x < 0)
			{
				return -__LINE__;
			}

			if (!GetSubNodeValue(dataElement, "npc_y", cfg.npc_pos.y) || cfg.npc_pos.y < 0)
			{
				return -__LINE__;
			}

			if (!GetSubNodeValue(dataElement, "npc_dir", cfg.npc_dir))
			{
				return -__LINE__;
			}

			if (!GetSubNodeValue(dataElement, "recover_num", cfg.recover_num))
			{
				return -__LINE__;
			}
		}

		fb_level_cfg.layer_cfg.push_back(cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int MedalFbConfig::InitMaterialCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;


	int next_seq = 0;
	while (dataElement)
	{
		int seq = 0;
		if (!GetSubNodeValue(dataElement, "seq", seq) || seq != next_seq++)
		{
			return -1;
		}
		MedalMaterialItemCfg cfg;

		TiXmlElement * target_item_ele = dataElement->FirstChildElement("target");
		if (!cfg.target_item.ReadConfig(target_item_ele))
		{
			return -2;
		}

		for (int i = 0; i < MedalMaterialItemCfg::NEED_COMSUME_ITEM_MAX_COUNT; ++i)
		{
			char itemid[1024] = { 0 };
			char num[1024] = { 0 };

			SNPRINTF(itemid, sizeof(itemid), "stuff_id_%d", i + 1);
			SNPRINTF(num, sizeof(num), "stuff_num_%d", i + 1);

			MedalMaterialItemCfg::ComsumeItem t;
			if (!GetSubNodeValue(dataElement, itemid, t.item_id))
			{
				return -210;
			}

			if (t.item_id != 0)
			{
				if (ITEMPOOL->GetItem(t.item_id) == NULL)
				{
					return -211;
				}
				if (!GetSubNodeValue(dataElement, num, t.num) || t.num <= 0)
				{
					return -212;
				}

				cfg.need_item_list.push_back(t);
			}
		}

		if (cfg.need_item_list.empty())
		{
			return -998;
		}

		m_fb_material_cfg.push_back(cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}
