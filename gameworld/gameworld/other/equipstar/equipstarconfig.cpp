#include "equipstarconfig.hpp"

EquipStarConfig::EquipStarConfig()
{
}

EquipStarConfig::~EquipStarConfig()
{
	std::vector<EquipStarCfg>().swap(equip_star_vec);
	for (std::map<int, std::vector<EquipStarGroupCfg *> >::iterator it = equip_star_group_map.begin(); it != equip_star_group_map.end(); it++)
	{
		std::vector<EquipStarGroupCfg *>().swap(it->second);
	}
	std::vector<EquipStarGroupCfg>().swap(equip_star_group_vec);
}

bool EquipStarConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("star_equip", InitEquipStarCfg);
	LOAD_CONFIG_2("star_group", InitEquipStarGroupCfg);
	LOAD_CONFIG_2("equip_star_up", InitEquipStarUpCfg);

	return true;
}

const EquipStarGroupCfg * EquipStarConfig::RandStarLevelCfg(int item_id)
{
	std::map<int, std::vector<EquipStarGroupCfg *> >::iterator it = equip_star_group_map.find(item_id);
	if (it != equip_star_group_map.end())
	{
		std::vector<EquipStarGroupCfg *> & group_vec = it->second;
		std::map<int, int>::iterator weight_it = equip_star_weight_map.find(item_id);
		if (weight_it == equip_star_weight_map.end())return NULL;

		if (weight_it->second <= 0)return NULL;

		int rand_num = rand() % weight_it->second;

		for (std::vector<EquipStarGroupCfg *>::iterator rand_it = group_vec.begin(); rand_it != group_vec.end(); rand_it++)
		{
			if ((*rand_it)->rate > rand_num)
			{
				return *rand_it;
			}
			rand_num -= (*rand_it)->rate;
		}
	}
	return NULL;
}

const EquipStarCfg * EquipStarConfig::GetEquipStarAttrCfg(int item_id, int level)
{
	for (std::vector<EquipStarCfg>::iterator it = equip_star_vec.begin(); it != equip_star_vec.end(); it++)
	{
		if (it->id == item_id && it->item_star == level)
		{
			return &(*it);
		}
	}
	return NULL;
}

const EquipStarUpCfg * EquipStarConfig::GetEquipStarUpCfg(int seq)
{
	std::map<int, EquipStarUpCfg>::iterator it = m_star_up_map.find(seq);
	if (it != m_star_up_map.end())
	{
		return &it->second;
	}
	return NULL;
}

int EquipStarConfig::InitEquipStarCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;
	while (dataElement != NULL)
	{
		EquipStarCfg node;

		if (!GetSubNodeValue(dataElement, "id", node.id) || NULL == ITEMPOOL->GetItem(node.id))
		{
			errormsg = STRING_SPRINTF("not data id OR id is NULL itemdata id[%d]", node.id);
			return -1;
		}
		if (!GetSubNodeValue(dataElement, "item_star", node.item_star))
		{
			errormsg = STRING_SPRINTF("not item_star ,item_star[%d] id[%d]", node.item_star, node.id);
			return -2;
		}
		
		if (!GetSubNodeIntArrayValueBySeperator(dataElement, "min_price_times", node.trade_min_gold_price, '|') || node.trade_min_gold_price.empty())
		{
			errormsg = STRING_SPRINTF("not min_price_times ERR size[%d]", (int)node.trade_min_gold_price.size());
			return -3;
		}

		if (!GetSubNodeIntArrayValueBySeperator(dataElement, "max_pricee_times", node.trade_max_gold_price, '|') || node.trade_max_gold_price.size() != node.trade_min_gold_price.size())
		{
			errormsg = STRING_SPRINTF("not max_pricee_times ERR size[%d]", (int)node.trade_max_gold_price.size());
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "return_times", node.return_times)) return -5;
		for (int i = 0; i < EquipStarCfg::ATTR_NUM_MAX; i++)
		{
			std::string read_path = STRING_SPRINTF("attr_type_%d", i + 1);
			int attr_type = 0;
			if (!GetSubNodeValue(dataElement, read_path, attr_type)) return -6;
			read_path = STRING_SPRINTF("attr_min_%d", i + 1);
			int attr_min = 0;
			if (!GetSubNodeValue(dataElement, read_path, attr_min)) return -7;
			read_path = STRING_SPRINTF("attr_max_%d", i + 1);
			int attr_max = 0;
			if (!GetSubNodeValue(dataElement, read_path, attr_max)) return -8;
			node.attr_voc[i].attr_min = attr_min;
			node.attr_voc[i].attr_max = attr_max;
			node.attr_voc[i].attr_type = attr_type;
		}

		equip_star_vec.push_back(node);
		dataElement = dataElement->NextSiblingElement();
	}
	
	return 0;
}

int EquipStarConfig::InitEquipStarGroupCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	while (dataElement != NULL)
	{
		EquipStarGroupCfg node;

		if (!GetSubNodeValue(dataElement, "group_id", node.group_id) || NULL == ITEMPOOL->GetItem(node.group_id))
		{
			errormsg = STRING_SPRINTF("not data group_id OR group_id is NULL itemdata group_id[%d]", node.group_id);
			return -1;
		}
		if (!GetSubNodeValue(dataElement, "item_star", node.item_star) || node.item_star < 0)
		{
			errormsg = STRING_SPRINTF("not item_star ,item_star[%d]", node.item_star);
			return -2;
		}
		if (!GetSubNodeValue(dataElement, "rate", node.rate) || node.rate < 0)
		{
			errormsg = STRING_SPRINTF("not rate ,rate[%d]", node.rate);
			return -3;
		}

		equip_star_group_vec.push_back(node);

		dataElement = dataElement->NextSiblingElement();
	}

	for (std::vector<EquipStarGroupCfg>::iterator it = equip_star_group_vec.begin(); it != equip_star_group_vec.end(); it++)
	{
		std::vector<EquipStarGroupCfg *> & point_it = equip_star_group_map[it->group_id];
		if (equip_star_weight_map.find(it->group_id) == equip_star_weight_map.end())
		{
			int & weight = equip_star_weight_map[it->group_id];
			weight = 0;
		}
		int & weight = equip_star_weight_map[it->group_id];
		weight += it->rate;
		point_it.push_back(&(*it));
	}

	return 0;
}

int EquipStarConfig::InitEquipStarUpCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	while (dataElement != NULL)
	{
		int compose_seq = 0;
		if (!GetSubNodeValue(dataElement, "compose_seq", compose_seq) || compose_seq <= 0)
		{
			errormsg = STRING_SPRINTF("not compose_seq OR compose_seq < 0 ,compose_seq[%d]", compose_seq);
			return -1;
		}
		if (m_star_up_map.find(compose_seq) != m_star_up_map.end())
		{
			errormsg = STRING_SPRINTF("m_star_up_map has same compose_seq[%d]", compose_seq);
			return -2;
		}
		EquipStarUpCfg &node = m_star_up_map[compose_seq];
		if (!GetSubNodeValue(dataElement, "star_limit", node.star_limit) || node.star_limit < 0)
		{
			errormsg = STRING_SPRINTF("not star_limit OR star_limit < 0 ,star_limit[%d]", node.star_limit);
			return -3;
		}
		if (!GetSubNodeValue(dataElement, "level_limit", node.level_limit) || node.level_limit < 0)
		{
			errormsg = STRING_SPRINTF("not level_limit OR level_limit < 0 ,level_limit[%d]", node.star_limit);
			return -4;
		}
		if (!GetSubNodeValue(dataElement, "color_limit", node.color_limit) || node.color_limit < 0)
		{
			errormsg = STRING_SPRINTF("not color_limit OR color_limit < 0 ,color_limit[%d]", node.color_limit);
			return -5;
		}
		if (!GetSubNodeValue(dataElement, "compose_id", node.compose_id) || NULL == ITEMPOOL->GetItem(node.compose_id))
		{
			errormsg = STRING_SPRINTF("not compose_id OR compose_id is NULL,compose_id[%d]", node.compose_id);
			return -6;
		}
		if (!GetSubNodeValue(dataElement, "item_star", node.item_star) || node.item_star < 0)
		{
			errormsg = STRING_SPRINTF("not item_star OR item_star < 0,item_star[%d]", node.item_star);
			return -7;
		}

		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}
