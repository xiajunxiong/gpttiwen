#include <set>

#include "funcguideconfig.h"
#include "gameworld/gameworld/item/itempool.h"
#include "item/itembase.h"

FuncGuideConfig::FuncGuideConfig()
{
}

FuncGuideConfig::~FuncGuideConfig()
{
	std::vector<ItemConfigData>().swap(m_guide_pet_cfg);
	std::vector<GuideFBLevelCfg>().swap(m_guide_level_cfg);
}

bool FuncGuideConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("choose_pet", InitPetChoiceOptionCfg);
	LOAD_CONFIG("new_scene", InitGuideFbLevelCfg);
	LOAD_CONFIG("scene_other", InitOtherCfg);
	LOAD_CONFIG("pet_skill", InitOtherCfg2);
	LOAD_CONFIG("pet_skill_two", InitOtherCfg3);


	return true;
}

const ItemConfigData * FuncGuideConfig::GetGuidePet()
{
	if (m_guide_pet_cfg.empty())
	{
		return NULL;
	}

	int r = RandomNum((int)m_guide_pet_cfg.size());

	return &m_guide_pet_cfg[r];
}

const GuideFBLevelCfg * FuncGuideConfig::GetGuideFBLevelCfg(int layer)
{
	int index = layer - 1;
	if (index < 0 || index >= (int)m_guide_level_cfg.size())
	{
		return NULL;
	}

	return &m_guide_level_cfg[index];
}

bool FuncGuideConfig::IsLastLayer(int layer)
{
	return layer == (int)m_guide_level_cfg.size();
}

const FuncGuideOtherCfg & FuncGuideConfig::GetOtherCfg()
{
	return m_other_cfg;
}

int FuncGuideConfig::InitPetChoiceOptionCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		ItemConfigData item;
		TiXmlElement * item_node = root_element->FirstChildElement("reward_item");
		if (NULL == item_node) return -2000;
		
		if (!item.ReadConfig(item_node))
		{
			return -2;
		}

		const ItemBase * item_base = ITEMPOOL->GetItem(item.item_id);
		if (item_base == NULL || item_base->GetItemType() != I_TYPE_SPECIAL_PET)
		{
			return -5;
		}

		m_guide_pet_cfg.push_back(item);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int FuncGuideConfig::InitGuideFbLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	std::set<int> check_repeat_scene;
	
	while (NULL != root_element)
	{
		GuideFBLevelCfg cfg;
		if (!GetSubNodeValue(root_element, "scene_map", cfg.scene_id))
		{
			return -1;
		}

		if (check_repeat_scene.find(cfg.scene_id) != check_repeat_scene.end())
		{
			return -2;
		}
		check_repeat_scene.insert(cfg.scene_id);

		if (!GetSubNodeValue(root_element, "monster_npc_id", cfg.monster_npc_id))
		{
			return -3;
		}

		if (cfg.monster_npc_id != 0)
		{
			if (!GetSubNodeValue(root_element, "monster_x1", cfg.monster_pos.x) || cfg.monster_pos.x < 0)
			{
				return -31;
			}

			if (!GetSubNodeValue(root_element, "monster_y1", cfg.monster_pos.y) || cfg.monster_pos.y < 0)
			{
				return -32;
			}
		}

		if (!GetSubNodeValue(root_element, "boss_npc_id", cfg.boss_npc_id))
		{
			return -4;
		}
		if (cfg.boss_npc_id != 0)
		{
			if (!GetSubNodeValue(root_element, "monster_boss_x1", cfg.boss_pos.x) || cfg.boss_pos.x < 0)
			{
				return -41;
			}

			if (!GetSubNodeValue(root_element, "monster_boss_y1", cfg.boss_pos.y) || cfg.boss_pos.y < 0)
			{
				return -42;
			}
		}


		if (!GetSubNodeValue(root_element, "box_res_id", cfg.box_id))
		{
			return -5;
		}
		if (cfg.box_id != 0)
		{
			if (!GetSubNodeValue(root_element, "box_x1", cfg.box_pos.x) || cfg.box_pos.x < 0)
			{
				return -51;
			}

			if (!GetSubNodeValue(root_element, "box_y1", cfg.box_pos.y) || cfg.box_pos.y < 0)
			{
				return -52;
			}
		}

		TiXmlElement * box_reward_element = root_element->FirstChildElement("reward_item_list");
		if (NULL != box_reward_element)
		{
			TiXmlElement *node_element = box_reward_element->FirstChildElement("reward_item");
			while (node_element)
			{
				ItemConfigData temp;
				if (!temp.ReadConfig(node_element))
				{
					return -102;
				}

				cfg.box_reward.push_back(temp);
				node_element = node_element->NextSiblingElement();
			}
		}

		m_guide_level_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int FuncGuideConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "task_id", m_other_cfg.guide_fb_task_id))
	{
		return -1;
	}

	return 0;
}

int FuncGuideConfig::InitOtherCfg2(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "xunbao_key", m_other_cfg.draw_guide_pet_comsume_item))
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "pet_monster", m_other_cfg.catch_pet_guide_monster_group_id))
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "pet_item", m_other_cfg.catch_pet_guide_monster_pet_item_id))
	{
		return -4;
	}

	const ItemBase * item = ITEMPOOL->GetItem(m_other_cfg.catch_pet_guide_monster_pet_item_id);
	if (item == NULL || item->GetItemType() != I_TYPE_SPECIAL_PET)
	{
		return -5;
	}

	if (!GetSubNodeValue(root_element, "xunbao_item_2", m_other_cfg.draw_guide_pet_2_reward_special_item_id))
	{
		return -50001;
	}

	item = ITEMPOOL->GetItem(m_other_cfg.draw_guide_pet_2_reward_special_item_id);
	if (item == NULL || item->GetItemType() != I_TYPE_SPECIAL_PET)
	{
		return -50002;
	}

	if (!GetSubNodeValue(root_element, "xunbao_pet_level_2", m_other_cfg.draw_guide_pet_2_pet_level))
	{
		return -51000;
	}

	if (!GetSubNodeValue(root_element, "pet_id", m_other_cfg.guide_pet_id))
	{
		return -6;
	}

	if (!GetSubNodeValue(root_element, "skill_num", m_other_cfg.guide_replace_passive_skill_replace_index))
	{
		return -7;
	}
	m_other_cfg.guide_replace_passive_skill_replace_index -= 1;

	if (!GetSubNodeValue(root_element, "pet_skill_guide", m_other_cfg.guide_replace_passive_skill_id))
	{
		return -8;
	}

	if (!GetSubNodeValue(root_element, "skill_item_id", m_other_cfg.guide_replace_passive_skill_item_id))
	{
		return -9;
	}

	if (!GetSubNodeValue(root_element, "lingwu_skill_id", m_other_cfg.guide_comprehend_skill_id))
	{
		return -11;
	}

	if (!GetSubNodeValue(root_element, "pet_id_1", m_other_cfg.guide_pet_id_2))
	{
		return -12;
	}

	if (ReadItemConfigDataList(root_element, "first_partner_id", m_other_cfg.draw_guide_partner_reward) != 0)
	{
		return -13;
	}

	if (ReadItemConfigDataList(root_element, "scend_partner_id", m_other_cfg.draw_guide_partner_reward_2) != 0)
	{
		return -14;
	}

	if (!GetSubNodeValue(root_element, "comsume_item_id", m_other_cfg.draw_guide_partner_comsume_item_id))
	{
		return -15;
	}

	if (!GetSubNodeValue(root_element, "comsume_item_num", m_other_cfg.draw_guide_partner_comsume_item_num))
	{
		return -16;
	}

	if (!GetSubNodeValue(root_element, "comsume_item_id_2", m_other_cfg.draw_guide_partner_comsume_item_id_2))
	{
		return -17;
	}

	if (!GetSubNodeValue(root_element, "comsume_item_num_2", m_other_cfg.draw_guide_partner_comsume_item_num_2))
	{
		return -18;
	}

	return 0;
}

int FuncGuideConfig::InitOtherCfg3(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "dream_notes", m_other_cfg.fast_dream_notes_level) || m_other_cfg.fast_dream_notes_level <= 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "lunhuiwangchuan", m_other_cfg.pass_lunhui_wangchuan_level) || m_other_cfg.pass_lunhui_wangchuan_level <= 0)
	{
		return -2;
	}

	return 0;
}
