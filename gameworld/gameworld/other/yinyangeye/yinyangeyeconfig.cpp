#include "item/itempool.h"
#include "yinyangeyeconfig.hpp"

YinYangEyeConfig::YinYangEyeConfig()
{
}

YinYangEyeConfig::~YinYangEyeConfig()
{
	std::vector<YinYangEyeBoxCfg>().swap(m_box_cfg);
	std::vector<YinYangEyeNpcCfg>().swap(m_npc_cfg);
}

bool YinYangEyeConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("box", InitBoxCfg);
	LOAD_CONFIG("NPC", InitNpcCfg);
	LOAD_CONFIG("reward", InitRewardCfg);
	LOAD_CONFIG("other", InitOtherCfg);

	return true;
}

const YinYangEyeBoxCfg * YinYangEyeConfig::GetBoxCfg(int seq) const
{
	if (0 > seq || seq >= (int)m_box_cfg.size())
	{
		return NULL;
	}

	return &m_box_cfg[seq];
}

const YinYangEyeNpcCfg * YinYangEyeConfig::GetNpcCfg(int seq) const
{
	if (0 > seq || seq >= (int)m_npc_cfg.size())
	{
		return NULL;
	}

	return &m_npc_cfg[seq];
}

const YinYangEyeRewardCfg * YinYangEyeConfig::GetRewardCfg(int reward_group) const
{
	std::map<int, YinYangEyeRewardCfg>::const_iterator it = m_reward_cfg.find(reward_group);
	if (it == m_reward_cfg.end())
	{
		return NULL;
	}

	return &it->second;
}

int YinYangEyeConfig::InitBoxCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{

		YinYangEyeBoxCfg node;

		if (!GetSubNodeValue(root_element, "seq", node.seq) || node.seq < 0 || node.seq != (int)m_box_cfg.size() || node.seq >= YIN_YANG_EYE_BOX_MAXNUM)
		{
			return -1;
		}
		if (!GetSubNodeValue(root_element, "scene_id", node.scene_id) || node.scene_id < 0)
		{
			return -2;
		}
		if (!GetSubNodeValue(root_element, "x", node.pos.x) || node.pos.x < 0)
		{
			return -3;
		}
		if (!GetSubNodeValue(root_element, "y", node.pos.y) || node.pos.y < 0)
		{
			return -4;
		}
		if (!GetSubNodeValue(root_element, "reward_group", node.reward_group) || node.reward_group <= 0)
		{
			return -5;
		}
		if (!GetSubNodeValue(root_element, "npc_id", node.npc_id) || node.npc_id <= 0)
		{
			return -6;
		}
		if (!GetSubNodeValue(root_element, "NPC_type", node.npc_type) || node.npc_type < YIN_YANG_EYE_NPC_TYPE_GOOD || node.npc_type >= YIN_YANG_EYE_NPC_TYPE_END)
		{
			return -7;
		}
		if (!GetSubNodeValue(root_element, "monster_group_id", node.monster_group_id) || node.monster_group_id < 0)
		{
			return -8;
		}
		if (!GetSubNodeValue(root_element, "is_always_appear", node.is_always_appear) || node.is_always_appear < 0)
		{
			return -9;
		}

		m_box_cfg.push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int YinYangEyeConfig::InitNpcCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{

		YinYangEyeNpcCfg node;

		if (!GetSubNodeValue(root_element, "seq", node.seq) || node.seq < 0 || node.seq != (int)m_npc_cfg.size() || node.seq >= YIN_YANG_EYE_NPC_MAXNUM)
		{
			return -1;
		}
		if (!GetSubNodeValue(root_element, "scene_id", node.scene_id) || node.scene_id < 0)
		{
			return -2;
		}
		if (!GetSubNodeValue(root_element, "x", node.pos.x) || node.pos.x < 0)
		{
			return -3;
		}
		if (!GetSubNodeValue(root_element, "y", node.pos.y) || node.pos.y < 0)
		{
			return -4;
		}
		if (!GetSubNodeValue(root_element, "npc_id", node.npc_id) || node.npc_id <= 0)
		{
			return -5;
		}
		if (!GetSubNodeValue(root_element, "NPC_type", node.npc_type) || node.npc_type < YIN_YANG_EYE_NPC_TYPE_GOOD || node.npc_type >= YIN_YANG_EYE_NPC_TYPE_END)
		{
			return -6;
		}
		if (!GetSubNodeValue(root_element, "is_anecdotes", node.is_anecdotes) || node.is_anecdotes < 0 || node.is_anecdotes > 1)
		{
			return -7;
		}
		if (!GetSubNodeValue(root_element, "reward_group_id", node.reward_group) || node.reward_group <= 0)
		{
			return -8;
		}
		if (!GetSubNodeValue(root_element, "monster_group_id", node.monster_group_id) || node.monster_group_id < 0)
		{
			return -9;
		}
		if (!GetSubNodeValue(root_element, "is_always_appear", node.is_always_appear) || node.is_always_appear < 0)
		{
			return -10;
		}

		m_npc_cfg.push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int YinYangEyeConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int reward_group = 0;
		if (!GetSubNodeValue(root_element, "reward_group", reward_group) || 0 >= reward_group)
		{
			return -1;
		}

		YinYangEyeRewardCfg & cfg = m_reward_cfg[reward_group];

		TiXmlElement * reward_list_element = root_element->FirstChildElement("reward_item_list");
		if (reward_list_element == NULL)
		{
			return -__LINE__;
		}

		TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward_item");
		while (reward_element)
		{
			ItemConfigData reward;
			if (!reward.ReadConfig(reward_element))
			{
				return -100;
			}
			cfg.reward_list.push_back(reward);
			reward_element = reward_element->NextSiblingElement();
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int YinYangEyeConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{

		YinYangEyeOtherCfg & cfg = m_other_cfg;

		if (!GetSubNodeValue(root_element, "box_radius", cfg.box_radius) || cfg.box_radius <= 0)
		{
			return -1;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}