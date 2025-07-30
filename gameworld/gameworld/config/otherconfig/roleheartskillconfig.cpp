#include "roleheartskillconfig.hpp"
#include "skill/skillpool.hpp"
#include "item/itembase.h"

RoleHeartSkillConfig::RoleHeartSkillConfig()
{
}

RoleHeartSkillConfig::~RoleHeartSkillConfig()
{
	std::vector<HeartSkillConvertCfg>().swap(m_convert_cfg);
}

bool RoleHeartSkillConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;
	LOAD_CONFIG("heart_skill_base", InitBaseCfg);
	LOAD_CONFIG("heart_skill_hole", InitHoleCfg);
	LOAD_CONFIG("heart_skill_score", InitScoreCfg);		//技能评分配置要放在基础配置读取之后
	LOAD_CONFIG("heart_skill_level", InitLevelCfg);		//等级配置读取需要放在基础配置和评分配置读取之后
	LOAD_CONFIG("resonance_level", InitResonanceLevelCfg);		
	LOAD_CONFIG("heart_resonance_group", InitResonanceCfg);		//共鸣配置读取需要放在共鸣等配置与基础配置读取之后
	LOAD_CONFIG("heart_skill_compose", InitComposeCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("heart_skill_displace", InitDisplaceCfg);
	LOAD_CONFIG("heart_attr", InitAttrCfg);
	
	return true;
}

const HeartSkillBaseCfg * RoleHeartSkillConfig::GetBaseCfg(int heart_skill_seq)
{
	std::map<int, HeartSkillBaseCfg>::iterator it = m_base_cfg.find(heart_skill_seq);
	if(it == m_base_cfg.end()) return NULL;

	return &it->second;
}

int RoleHeartSkillConfig::GetBaseColorById(int item_id)
{
	std::map<int, int>::iterator it = m_base_color_cfg.find(item_id);
	if(it == m_base_color_cfg.end()) return 0;

	return it->second;
}

const HeartSkillHoleCfg* RoleHeartSkillConfig::GetUnlockLevel(int hole_seq)
{
	std::map<int, HeartSkillHoleCfg>::iterator it = m_skill_hole_cfg.find(hole_seq);
	if(it == m_skill_hole_cfg.end()) return NULL;

	return &it->second;
}

int RoleHeartSkillConfig::GetSkillScore(int heart_skill_seq, int skill_level)
{
	int score_key = this->GetKeyBySeqAndLevel(heart_skill_seq, skill_level);
	std::map<int, int>::iterator it = m_skill_score_cfg.find(score_key);
	if(it == m_skill_score_cfg.end()) return 0;

	return it->second;
}

int RoleHeartSkillConfig::GetMaxLevelBySeq(int heart_skill_seq)
{
	std::map<int, std::map<int, HeartSkillLevelCfg> >::iterator it = m_skill_level_map.find(heart_skill_seq);
	if(it == m_skill_level_map.end() || it->second.empty()) return 0;

	return it->second.rbegin()->first;
}

const HeartSkillLevelCfg * RoleHeartSkillConfig::GetLevelCfg(int heart_skill_seq, int skill_level)
{
	std::map<int, std::map<int, HeartSkillLevelCfg> >::iterator it = m_skill_level_map.find(heart_skill_seq);
	if (it == m_skill_level_map.end() || it->second.empty()) return NULL;

	std::map<int, HeartSkillLevelCfg>::iterator level_it = it->second.find(skill_level);
	if(level_it == it->second.end()) return NULL;

	return &level_it->second;
}

int RoleHeartSkillConfig::GetSkillTypeByStatus(int skill_status)
{
	switch (skill_status)
	{
	case HEART_SKILL_INITIATIVE_INDEX:
	case HEART_SKILL_INITIATIVE_INDEX1:
	{
		return HEART_SKILL_SKILL_TYPE_INITATIVE;
	}
	break;
	case HEART_SKILL_PASSIVE_INDEX:
	case HEART_SKILL_PASSIVE_INDEX1:
	case HEART_SKILL_PASSIVE_INDEX2:
	case HEART_SKILL_PASSIVE_INDEX3:
	{
		return HEART_SKILL_SKILL_TYPE_PASSIVE;
	}
	break;
	default:
		break;
	}
	return 0;
}

bool RoleHeartSkillConfig::IsMeetReasonance(int reasonance_group, int count)
{
	std::map<int, int>::iterator it = m_reasonance_count_by_group.find(reasonance_group);
	if(it == m_reasonance_count_by_group.end() || it->second > count) return false;

	return true;
}

int RoleHeartSkillConfig::GetReasonanceGroupBySeq(int heart_skill_seq)
{
	std::map<int, int>::iterator it = m_reasonance_group_by_seq.find(heart_skill_seq);
	if(it == m_reasonance_group_by_seq.end()) return 0;
		
	return it->second;
}

const HeartSkillResonanceLevelCfg * RoleHeartSkillConfig::GetReasonanceLevelCfg(int resonance_group, int level)
{
	std::map<int, HeartSkillResonanceLevelCfg>::iterator it = m_skiLL_resonance_level_cfg.find(this->GetKeyBySeqAndLevel(resonance_group, level));
	if(it == m_skiLL_resonance_level_cfg.end()) return NULL;

	return &it->second;
}

const ItemConfigData* RoleHeartSkillConfig::GetComposeCfg(int compose_target)
{
	std::map<int, ItemConfigData>::const_iterator it = m_compose_map.find(compose_target);
	if(it == m_compose_map.end()) return NULL;

	return &it->second;
}

const HeartSkillConvertCfg * RoleHeartSkillConfig::GetConvertCfg(int convert_item_id)
{
	for (int i = 0; i < (int)m_convert_cfg.size(); i++)
	{
		if (m_convert_cfg[i].convert_list.end() != m_convert_cfg[i].convert_list.find(convert_item_id))
		{
			return &m_convert_cfg[i];
		}
	}
	return NULL;
}

const HeartSkillDisplaceCfg * RoleHeartSkillConfig::GetDisplaceCfg(int heart_skill_color)
{
	std::map<int, HeartSkillDisplaceCfg>::iterator it = m_displace_map.find(heart_skill_color);
	if(it == m_displace_map.end()) return NULL;

	return &it->second;
}

const HeartSkillAttrCfg * RoleHeartSkillConfig::GetAttrCfg(int heart_skill_seq, int skill_level, int prof)
{
	if (prof <= 0 || prof >= ARRAY_ITEM_COUNT(m_attr_cfg_list))
	{
		return NULL;
	}
	int map_key = this->GetKeyBySeqAndLevel(heart_skill_seq, skill_level);
	std::map<int, HeartSkillAttrCfg>::const_iterator it = m_attr_cfg_list[prof].find(map_key);
	if (it == m_attr_cfg_list[prof].end())
	{
		return NULL;
	}

	return &it->second;
}

int RoleHeartSkillConfig::InitBaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	int initative_skill_num = 0, passive_skill_num = 0;
	while (NULL != root_element)
	{
		int heart_skill_seq = 0;
		if (!GetSubNodeValue(root_element, "heart_skill_seq", heart_skill_seq) || m_base_cfg.find(heart_skill_seq) != m_base_cfg.end()) return -1;
		
		HeartSkillBaseCfg base_cfg;
		if (!GetSubNodeValue(root_element, "heart_skill_type", base_cfg.heart_skill_type)) return -2;
		if (!GetSubNodeValue(root_element, "heart_skill_id", base_cfg.heart_skill_id)) return -3;

		switch (base_cfg.heart_skill_type)
		{
		case HEART_SKILL_SKILL_TYPE_INITATIVE:
		{
			if(NULL == SkillPool::Instance().GetSkillCfg(base_cfg.heart_skill_id)) return -4;
			initative_skill_num++;
		}
		break;
		case HEART_SKILL_SKILL_TYPE_PASSIVE:
		{
			if (NULL == SkillPool::Instance().GetPassiveSkillCfg(base_cfg.heart_skill_id)) return -5;
			passive_skill_num++;
		}
		break;
		default:
			return -6;
		}
		int item_id = 0, heart_skill_color = 0;
		if (!GetSubNodeValue(root_element, "item_id", item_id) || NULL == ITEMPOOL->GetItem(item_id)) return -7;
		if (!GetSubNodeValue(root_element, "heart_skill_color", heart_skill_color) || heart_skill_color <= 0) return -8;

		m_base_cfg[heart_skill_seq] = base_cfg;
		m_base_color_cfg[item_id] = heart_skill_color;
		root_element = root_element->NextSiblingElement();
	}
	if (initative_skill_num <= 0 || initative_skill_num > MAX_HEART_SKILL_INITIATIVE_NUM || passive_skill_num <= 0 || passive_skill_num > MAX_HEART_SKILL_PASSIVE_NUM)
	{
		return -7;
	}

	return 0;
}

int RoleHeartSkillConfig::InitHoleCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int hole_seq = 0;
		HeartSkillHoleCfg node_cfg;
		if (!GetSubNodeValue(root_element, "hole_seq", hole_seq)) return -1;
		if (!GetSubNodeValue(root_element, "deblocking_level", node_cfg.unlock_level) || node_cfg.unlock_level <= 0 || node_cfg.unlock_level > MAX_ROLE_LEVEL) return -2;
		if (!GetSubNodeValue(root_element, "heart_skill_type", node_cfg.heart_skill_type) || node_cfg.heart_skill_type <= 0) return -3;

		if (node_cfg.heart_skill_type != RoleHeartSkillConfig::GetSkillTypeByStatus(hole_seq))
		{
			return -4;
		}

		m_skill_hole_cfg[hole_seq] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	if (HEART_SKILL_STATUS_MAX != m_skill_hole_cfg.size())
	{
		return -5;
	}

	return 0;
}

int RoleHeartSkillConfig::InitScoreCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int heart_skill_seq = 0;
		if (!GetSubNodeValue(root_element, "heart_skill_seq", heart_skill_seq) || m_base_cfg.find(heart_skill_seq) == m_base_cfg.end()) return -1;

		int skill_level = 0;
		if (!GetSubNodeValue(root_element, "skill_level", skill_level) || skill_level <= 0 || skill_level > MAX_BATTLE_SKILL_LEVEL) return -2;

		//检测重复配置
		int score_key = this->GetKeyBySeqAndLevel(heart_skill_seq, skill_level);
		if (m_skill_score_cfg.find(score_key) != m_skill_score_cfg.end())
		{
			return -3;
		}
		int skill_score = 0;
		if (!GetSubNodeValue(root_element, "skill_score", skill_score) || skill_score <= 0) return -4;

		m_skill_score_cfg[score_key] = skill_score;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int RoleHeartSkillConfig::InitLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int heart_skill_seq = 0;
		if (!GetSubNodeValue(root_element, "heart_skill_seq", heart_skill_seq) || m_base_cfg.find(heart_skill_seq) == m_base_cfg.end()) return -1;

		int skill_level = 0;
		if (!GetSubNodeValue(root_element, "skill_level", skill_level) || skill_level <= 0 || skill_level > MAX_BATTLE_SKILL_LEVEL) return -2;

		//检测重复配置
		std::map<int, std::map<int, HeartSkillLevelCfg> >::iterator it = m_skill_level_map.find(heart_skill_seq);
		if (it != m_skill_level_map.end()  && it->second.find(skill_level) != it->second.end())
		{
			return -3;
		}
		//检测是否有对应的评分
		int score_key = this->GetKeyBySeqAndLevel(heart_skill_seq, skill_level);
		if (m_skill_score_cfg.find(score_key) == m_skill_score_cfg.end())
		{
			return -4;
		}

		HeartSkillLevelCfg node_cfg;
		node_cfg.skill_level = skill_level;
		if (!GetSubNodeValue(root_element, "coin_consume", node_cfg.coin_consume) || node_cfg.coin_consume < 0) return -5;
		if (!GetSubNodeValue(root_element, "gold_consume", node_cfg.gold_consume) || node_cfg.gold_consume < 0) return -6;

		TiXmlElement * list_node = root_element->FirstChildElement("item_consume_list");
		if (NULL == list_node)
		{
			return -100;
		}
		TiXmlElement * item_node = list_node->FirstChildElement("item_consume");
		if (NULL == item_node)
		{
			return -101;
		}
		while (NULL != item_node)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_node)) return -102;

			node_cfg.item_consume_list.push_back(item);
			item_node = item_node->NextSiblingElement();
		}

		m_skill_level_map[heart_skill_seq][skill_level] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int RoleHeartSkillConfig::InitResonanceCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int resonance_group = 0;
		if (!GetSubNodeValue(root_element, "resonance_group", resonance_group) || resonance_group <= 0) return -1;
		
		int resonance_key = this->GetKeyBySeqAndLevel(resonance_group, 1);	//共鸣一定是从1级开始的
		if (m_skiLL_resonance_level_cfg.end() == m_skiLL_resonance_level_cfg.find(resonance_key))
		{
			return -2;
		}
		int heart_skill_seq = 0;
		if (!GetSubNodeValue(root_element, "heart_skill_seq", heart_skill_seq) || m_base_cfg.find(heart_skill_seq) == m_base_cfg.end() || 
			m_reasonance_group_by_seq.end() != m_reasonance_group_by_seq.find(heart_skill_seq)) return -3;

		m_reasonance_count_by_group[resonance_group] += 1;
		m_reasonance_group_by_seq[heart_skill_seq] = resonance_group;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RoleHeartSkillConfig::InitResonanceLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int resonance_group = 0, level = 0;
		if (!GetSubNodeValue(root_element, "resonance_group", resonance_group) || resonance_group <= 0) return -1;
		if (!GetSubNodeValue(root_element, "resonance_level", level) || level <= 0 || level >= MAX_HEART_SKILL_BASE_NUM) return -2;

		int resonance_key = this->GetKeyBySeqAndLevel(resonance_group, level);
		if (m_skiLL_resonance_level_cfg.end() != m_skiLL_resonance_level_cfg.find(resonance_key))
		{
			return -3;
		}
		char buf_type[64] = { 0 };
		char buf_num[64] = { 0 };
		HeartSkillResonanceLevelCfg node_cfg;
		for (int i = 1; i <= MAX_HEART_SKILL_RESONANCE_ATTR_NUM; i++)
		{
			SNPRINTF(buf_type, sizeof(buf_type), "attr_type_%d", i);
			SNPRINTF(buf_num, sizeof(buf_num), "attr_num_%d", i);
			int index  = i - 1;
			if (!GetSubNodeValue(root_element, buf_type, node_cfg.attr_list[index].attr_type) || node_cfg.attr_list[index].attr_type < BATTLE_ATTR_MIN ||
				node_cfg.attr_list[index].attr_type >= BATTLE_ATTR_MAX) return -10 * i - 1;

			if (!GetSubNodeValue(root_element, buf_num, node_cfg.attr_list[index].attr_value) || node_cfg.attr_list[index].attr_value < 0 ||
				node_cfg.attr_list[index].attr_value >= MAX_INT16) return -10 * i - 2;

		}
		m_skiLL_resonance_level_cfg[resonance_key] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int RoleHeartSkillConfig::InitComposeCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int compose_target = 0;
		if (!GetSubNodeValue(root_element, "compose_target", compose_target) || NULL == ITEMPOOL->GetItem(compose_target)) return -1;
		if (m_compose_map.end() != m_compose_map.find(compose_target))
		{
			return -2;
		}

		TiXmlElement * item_node = root_element->FirstChildElement("compose_consume");
		if (NULL == item_node)
		{
			return -101;
		}
	
		ItemConfigData item;
		if (!item.ReadConfig(item_node))
		{
			return -102;
		}

		m_compose_map[compose_target] = item;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int RoleHeartSkillConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	char buf[64] = {0};
	char buf_1[64] = {0};
	std::set<int> convert_list;
	for (int i = 1; i <= MAX_HEART_SKILL_CONVERT_GROUP_NUM; ++i)
	{
		HeartSkillConvertCfg convert_cfg;
		SNPRINTF(buf, sizeof(buf), "switch_comsums_%d", i);
		SNPRINTF(buf_1, sizeof(buf_1), "switch_group_%d", i);
		if (!GetSubNodeValue(root_element, buf, convert_cfg.convert_consume_item_id) || NULL == ITEMPOOL->GetItem(convert_cfg.convert_consume_item_id)) return -(i * 10 + 1);

		TiXmlElement * switch_list = root_element->FirstChildElement(buf_1);
		if (NULL == switch_list)
		{
			return -(i * 10 + 2);
		}
		TiXmlElement *node_element = switch_list->FirstChildElement("node");
		if (NULL == node_element)
		{
			return -(i * 10 + 3);
		}
		while (node_element)
		{
			int item_id = 0;
			if (!GetNodeValue(node_element, item_id) || NULL == ITEMPOOL->GetItem(item_id))
			{
				return -(i * 10 + 4);
			}
			if (convert_list.end() != convert_list.find(item_id))
			{
				return -(i * 10 + 5);
			}

			convert_cfg.convert_list.insert(item_id);
			node_element = node_element->NextSiblingElement();
		}
		m_convert_cfg.push_back(convert_cfg);
	}
	return 0;
}

int RoleHeartSkillConfig::InitDisplaceCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	
	while (NULL != root_element)
	{
		int heart_skill_color = 0;
		if (!GetSubNodeValue(root_element, "heart_skill_color", heart_skill_color) || heart_skill_color <= 0) return -1;

		if (m_displace_map.end() != m_displace_map.find(heart_skill_color))
		{
			return -2;
		}
		HeartSkillDisplaceCfg node_cfg;
		if (!GetSubNodeValue(root_element, "heart_skill_num", node_cfg.need_heart_skill_num) || node_cfg.need_heart_skill_num <= 0 || node_cfg.need_heart_skill_num > MAX_HEART_SKILL_DISPLACE_NEED_NUM) return -3;
		if (!GetSubNodeValue(root_element, "consume_item", node_cfg.need_consume_item_id) || NULL == ITEMPOOL->GetItem(node_cfg.need_consume_item_id)) return -4;
		if (!GetSubNodeValue(root_element, "displace_target", node_cfg.displace_target_item_id) || NULL == ITEMPOOL->GetItem(node_cfg.displace_target_item_id)) return -5;
		if (!GetSubNodeValue(root_element, "target_num", node_cfg.target_num) || node_cfg.target_num <= 0) return -6;

		m_displace_map[heart_skill_color] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RoleHeartSkillConfig::InitAttrCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int heart_skill_seq = 0, limit_prof = 0, skill_level = 0;
		if (!GetSubNodeValue(root_element, "heart_skill_seq", heart_skill_seq) || m_base_cfg.find(heart_skill_seq) == m_base_cfg.end()) return -1;
		if (!GetSubNodeValue(root_element, "limit_prof", limit_prof) || limit_prof <= 0 || limit_prof >= ARRAY_ITEM_COUNT(m_attr_cfg_list)) return -2;
		if (!GetSubNodeValue(root_element, "heart_level", skill_level) || skill_level <= 0 || skill_level > MAX_BATTLE_SKILL_LEVEL) return -3;
	
		int map_key = GetKeyBySeqAndLevel(heart_skill_seq, skill_level);

		HeartSkillAttrCfg & node_cfg = m_attr_cfg_list[limit_prof][map_key];
		for (int i = 0; i < ARRAY_ITEM_COUNT(node_cfg.attr_list); ++i)
		{
			std::string attr_type = STRING_SPRINTF("attr_type_%d", i + 1);
			std::string attr_add = STRING_SPRINTF("attr_add_%d", i + 1);
			if (!GetSubNodeValue(root_element, attr_type, node_cfg.attr_list[i].attr_type) || node_cfg.attr_list[i].attr_type < BATTLE_ATTR_MIN ||
				node_cfg.attr_list[i].attr_type >= BATTLE_ATTR_MAX) return -10 * i - 1;

			if (!GetSubNodeValue(root_element, attr_add, node_cfg.attr_list[i].attr_value) || node_cfg.attr_list[i].attr_value < 0 ||
				node_cfg.attr_list[i].attr_value >= MAX_INT16) return -10 * i - 2;
		}

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int RoleHeartSkillConfig::GetKeyBySeqAndLevel(int heart_skill_seq, int skill_level)
{
	return MAX_HEART_SKILL_BASE_NUM * heart_skill_seq + skill_level;
}
