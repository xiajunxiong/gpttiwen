#include "lunhuiwangchuanconfig.hpp"
#include "servercommon/struct/itemlistparam.h"

LunHuiWangChuanConfig::LunHuiWangChuanConfig()
{
}

LunHuiWangChuanConfig::~LunHuiWangChuanConfig()
{
}

bool LunHuiWangChuanConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("pass_reward", InitPassRewardGroupCfg);
	LOAD_CONFIG("monster_description", InitMonsterDetailCfg);
	LOAD_CONFIG("layer_reward", InitLayerCfg);

	return true;
}

const LunHuiWangChuanPassRewardCfg * LunHuiWangChuanConfig::GetLunHuiWangChuanPassRewardCfg(int challenge_id, int level)
{
	if (level <= 0 || level > MAX_ROLE_LEVEL)
	{
		return NULL;
	}
	std::map<int, LunHuiWangChuanPassRewardListCfg>::iterator it = m_pass_map.find(challenge_id);
	if (it != m_pass_map.end())
	{
		return &it->second.pass_list[level - 1];
	}
	return NULL;
}

const LunHuiWangChuanMonsterGroupCfg * LunHuiWangChuanConfig::GetLunHuiWangChuanMonsterDetailCfg(int challenge_id)
{
	if (challenge_id < 0 || challenge_id >= MAX_LUN_HUI_WANG_CHUAN_INDEX)return NULL;
	if (!m_monster_group_map[challenge_id].is_value)return NULL;
	return &m_monster_group_map[challenge_id];
}

const LunHuiWangChuanRewardCfg * LunHuiWangChuanConfig::GetLunHuiWangChuanRewardCfg(int role_level, int seq)
{
	if (role_level <= 0 || role_level > MAX_ROLE_LEVEL)
	{
		return NULL;
	}
	if (seq < 0 || seq >= MAX_LUN_HUI_WANG_CHUAN_INDEX)
	{
		return NULL;
	}
	if (!m_reward_map[role_level - 1][seq].is_value)return NULL;
	return &m_reward_map[role_level - 1][seq];
}

int LunHuiWangChuanConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	if (!GetSubNodeValue(dataElement, "refresh_time", m_other.refresh_day) || m_other.refresh_day <= 0)
	{
		return -1;
	}

	return 0;
}

int LunHuiWangChuanConfig::InitPassRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int challenge_id = 0;
		if (!GetSubNodeValue(dataElement, "challenge_id", challenge_id) || challenge_id <= 0)
		{
			return -1;
		}

		LunHuiWangChuanPassRewardListCfg &list_node = m_pass_map[challenge_id];

		int level = 0;
		if (!GetSubNodeValue(dataElement, "role_level", level) || level <= 0 || level > MAX_ROLE_LEVEL)
		{
			return -2;
		}

		LunHuiWangChuanPassRewardCfg & node = list_node.pass_list[level - 1];

		node.challenge_id = challenge_id;
		node.role_level = level;

		TiXmlElement* item_list_Element = dataElement->FirstChildElement("item_list_list");
		if (NULL != item_list_Element)
		{
			TiXmlElement* item_Element = item_list_Element->FirstChildElement("item_list");
			while (NULL != item_Element)
			{
				ItemConfigData reward_node;
				if (!reward_node.ReadConfig(item_Element))
				{
					return -3;
				}
				node.reward_list.push_back(reward_node);
				item_Element = item_Element->NextSiblingElement();
			}
		}

		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int LunHuiWangChuanConfig::InitMonsterDetailCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int challenge_id = 0;
		if (!GetSubNodeValue(dataElement, "challenge_id", challenge_id) || challenge_id <= 0)
		{
			return -1;
		}

		LunHuiWangChuanMonsterGroupCfg &node = m_monster_group_map[challenge_id];
		node.challenge_id = challenge_id;
		if (!GetSubNodeValue(dataElement, "monster_group_id_1", node.monster_group_id_1))
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "monster_group_id_2", node.monster_group_id_2))
		{
			return -3;
		}
		node.is_value = true;
		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int LunHuiWangChuanConfig::InitLayerCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int role_level = 0;
		if (!GetSubNodeValue(dataElement, "role_level", role_level) || role_level <= 0 || role_level > MAX_ROLE_LEVEL)
		{
			return -1;
		}

		int layer = 0;
		if (!GetSubNodeValue(dataElement, "layer", layer) || layer <= 0)
		{
			return -2;
		}

		LunHuiWangChuanRewardCfg &node = m_reward_map[role_level - 1][layer];
		node.role_level = role_level;
		node.layer = layer;

		TiXmlElement* item_list_Element = dataElement->FirstChildElement("item_list_list");
		if (NULL != item_list_Element)
		{
			TiXmlElement* item_Element = item_list_Element->FirstChildElement("item_list");
			while (NULL != item_Element)
			{
				ItemConfigData reward_node;
				if (!reward_node.ReadConfig(item_Element))
				{
					return -3;
				}
				node.reward_list.push_back(reward_node);
				item_Element = item_Element->NextSiblingElement();
			}
		}
		node.is_value = true;
		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}
