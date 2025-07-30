#include "partnergifttreeconfig.hpp"

PartnerGiftTreeConfig::PartnerGiftTreeConfig()
{
}

PartnerGiftTreeConfig::~PartnerGiftTreeConfig()
{
	std::vector<PartnerGiftTreeNodeCfg>().swap(m_partner_gift_list);
	std::vector<PartnerGiftUplevelCfg>().swap(m_partner_uplevel_vec);
	std::vector<PartnerGiftSpCfg>().swap(m_partner_gift_sp_cfg);
	std::vector<PartnerGiftTreeReturnCfg>().swap(m_partner_skill_return_vec);
}

bool PartnerGiftTreeConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("inborn_base", InitPartnerGiftTreeCfg);
	LOAD_CONFIG("inborn_level", InitPartnerGiftUpLevelCfg);
	LOAD_CONFIG("inborn_effect", InitPartnerGiftSPCfg);
	LOAD_CONFIG("inborn_return", InitPartnerGiftReturnCfg);

	return true;
}

const PartnerGiftTreeNodeCfg* PartnerGiftTreeConfig::GetPartnerGiftTreeByPartnerId(int partner_id,int tree_point)
{
	if (partner_id > 0 && partner_id < PARTNER_NUM_MAX)
	{
		if (tree_point > 0 && tree_point <= MAX_PARTNER_GIFT_TREE_SKILL_NUM)
		{
			return partner_tree_list[partner_id][tree_point - 1];
		}
	}
	return NULL;
}

const PartnerGiftUplevelCfg * PartnerGiftTreeConfig::GetPartnerGiftTreeUplevelByPartnerId(int partner_id, int tree_point, int level)
{
	if (partner_id > 0 && partner_id < PARTNER_NUM_MAX)
	{
		if (tree_point > 0 && tree_point <= MAX_PARTNER_GIFT_TREE_SKILL_NUM)
		{
			if (level > 0 && level <= MAX_PARTNER_GIFT_TREE_LEVEL)
			{
				return partner_gift_up_level[partner_id][tree_point - 1][level - 1];
			}
		}
	}
	return NULL;
}

const PartnerGiftSpListCfg * PartnerGiftTreeConfig::GetPartnerGiftSpCfg(int partner_id)
{
	if (partner_id <= 0 || partner_id >= PARTNER_NUM_MAX)return NULL;
	return &partner_gift_sp_vec[partner_id];
}

const PartnerGiftTreeReturnCfg * PartnerGiftTreeConfig::GetPartnerGiftTreeReturnCfg(int tree_point, int level)
{
	if (tree_point > 0 && tree_point <= MAX_PARTNER_GIFT_TREE_SKILL_NUM)
	{
		if (level > 0 && level <= MAX_PARTNER_GIFT_TREE_LEVEL)
		{
			return partner_skill_return[tree_point - 1][level - 1];
		}
	}
	return NULL;
}

int PartnerGiftTreeConfig::InitPartnerGiftTreeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;
	memset(partner_tree_list, NULL, sizeof(partner_tree_list));
	while (NULL != root_element)
	{
		PartnerGiftTreeNodeCfg node;
		if (!GetSubNodeValue(root_element, "inborn_type", node.inborn_type) || node.inborn_type <= 0 || node.inborn_type >= PARTNER_NUM_MAX)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "inborn_hole_id", node.inborn_hole_id) || node.inborn_hole_id <= 0 || node.inborn_hole_id >= MAX_PARTNER_GIFT_TREE_SKILL_NUM)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "pre_hole_num", node.pre_hole_num) || node.pre_hole_num > PartnerGiftTreeNodeCfg::MAX_PRE_HOLE_NUM)
		{
			return -3;
		}

		char path[30];
		for (int i = 0; i < PartnerGiftTreeNodeCfg::MAX_PRE_HOLE_NUM; i++)
		{
			memset(path, 0, sizeof(path));
			sprintf(path, "pre_hole_%d", i + 1);
			if (!GetSubNodeValue(root_element, path, node.pre_hole[i].hole))
			{
				return -4;
			}

			memset(path, 0, sizeof(path));
			sprintf(path, "pre_level_%d", i + 1);
			if (!GetSubNodeValue(root_element, path, node.pre_hole[i].level_limit))
			{
				return -5;
			}
		}

		if (!GetSubNodeValue(root_element, "pos_hole_num", node.pos_hole_num) || node.pos_hole_num > PartnerGiftTreeNodeCfg::MAX_POS_HOLE_NUM)
		{
			return -6;
		}

		for (int i = 0; i < PartnerGiftTreeNodeCfg::MAX_POS_HOLE_NUM; i++)
		{
			memset(path, 0, sizeof(path));
			sprintf(path, "pos_hole_%d", i + 1);
			if (!GetSubNodeValue(root_element, path, node.pos_hole[i]))
			{
				return -7;
			}
		}

		if (!GetSubNodeValue(root_element, "reset_consume_coin", node.reset_consume_coin))
		{
			return -8;
		}

		m_partner_gift_list.push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	for (std::vector<PartnerGiftTreeNodeCfg>::iterator it = m_partner_gift_list.begin(); it != m_partner_gift_list.end(); it++)
	{
		if (it->inborn_type > 0 && it->inborn_type <= PARTNER_NUM_MAX)
		{
			if (it->inborn_hole_id > 0 && it->inborn_hole_id <= MAX_PARTNER_GIFT_TREE_SKILL_NUM)
			{
				if (partner_tree_list[it->inborn_type][it->inborn_hole_id - 1] == NULL)
				{
					partner_tree_list[it->inborn_type][it->inborn_hole_id - 1] = &(*it);
				}
			}
		}
	}

	return 0;
}

int PartnerGiftTreeConfig::InitPartnerGiftUpLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;
	memset(partner_gift_up_level, NULL, sizeof(partner_gift_up_level));
	while (NULL != root_element)
	{
		PartnerGiftUplevelCfg node;
		if (!GetSubNodeValue(root_element, "inborn_type", node.inborn_type) || node.inborn_type <= 0 || node.inborn_type >= PARTNER_NUM_MAX)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "inborn_hole_id", node.inborn_hole_id) || node.inborn_hole_id <= 0 || node.inborn_hole_id >= MAX_PARTNER_GIFT_TREE_SKILL_NUM)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "inborn_hole_level", node.inborn_hole_level) || node.inborn_hole_level < 0)
		{
			return -3;
		}

		char path[30];
		for (int i = 0; i < PartnerGiftUplevelCfg::MAX_PARTNER_TREE_MAX_ATTR_NUM; i++)
		{
			memset(path, 0, sizeof(path));
			sprintf(path, "attr_type_%d", i + 1);
			if (!GetSubNodeValue(root_element, path, node.attr_list[i].attr_type))
			{
				return -4;
			}

			memset(path, 0, sizeof(path));
			sprintf(path, "attr_add_%d", i + 1);
			if (!GetSubNodeValue(root_element, path, node.attr_list[i].attr_value))
			{
				return -5;
			}
		}

		if (!GetSubNodeValue(root_element, "sp_id", node.sp_id))
		{
			return -6;
		}

		TiXmlElement *  consume_item_list_element = root_element->FirstChildElement("strengthen_consume_item_list");
		if (NULL != consume_item_list_element)
		{
			TiXmlElement* consume_item_element = consume_item_list_element->FirstChildElement("strengthen_consume_item");
			while (consume_item_element != NULL)
			{
				ItemConfigData node_cfg;
				if (!node_cfg.ReadConfig(consume_item_element))
				{
					return -7;
				}
				node.consume_item_list.push_back(node_cfg);
				consume_item_element = consume_item_element->NextSiblingElement();
			}
		}

		m_partner_uplevel_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	for (std::vector<PartnerGiftUplevelCfg>::iterator it = m_partner_uplevel_vec.begin(); it != m_partner_uplevel_vec.end(); it++)
	{
		if (it->inborn_type > 0 && it->inborn_type < PARTNER_NUM_MAX)
		{
			if (it->inborn_hole_id > 0 && it->inborn_hole_id <= MAX_PARTNER_GIFT_TREE_SKILL_NUM)
			{
				if (it->inborn_hole_level > 0 && it->inborn_hole_level <= MAX_PARTNER_GIFT_TREE_LEVEL)
				{
					if (partner_gift_up_level[it->inborn_type][it->inborn_hole_id - 1][it->inborn_hole_level - 1] == NULL)
					{
						partner_gift_up_level[it->inborn_type][it->inborn_hole_id - 1][it->inborn_hole_level - 1] = &(*it);
					}
				}
			}
		}
	}

	return 0;
}

int PartnerGiftTreeConfig::InitPartnerGiftSPCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;
	while (NULL != root_element)
	{
		PartnerGiftSpCfg node;
		if (!GetSubNodeValue(root_element, "inborn_type", node.inborn_type) || node.inborn_type <= 0 || node.inborn_type >= PARTNER_NUM_MAX)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "sp_id", node.sp_id))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "inborn_level_ask", node.inborn_level_ask))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "sp_hole", node.sp_hole))
		{
			return -4;
		}

		m_partner_gift_sp_cfg.push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	for (std::vector<PartnerGiftSpCfg>::iterator it = m_partner_gift_sp_cfg.begin(); it != m_partner_gift_sp_cfg.end(); it++)
	{
		if (it->inborn_type > 0 && it->inborn_type < PARTNER_NUM_MAX)
		{
			if (it->sp_hole > 0 && it->sp_hole <= PartnerGiftSpListCfg::MAX_SP_NUM)
			{
				if (partner_gift_sp_vec[it->inborn_type].sp_list[it->sp_hole - 1] == NULL)
				{
					partner_gift_sp_vec[it->inborn_type].sp_list[it->sp_hole - 1] = &(*it);
				}
			}
		}
	}

	return 0;
}

int PartnerGiftTreeConfig::InitPartnerGiftReturnCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;
	memset(partner_skill_return, NULL, sizeof(partner_skill_return));
	while (NULL != root_element)
	{
		PartnerGiftTreeReturnCfg node;
		if (!GetSubNodeValue(root_element, "inborn_hole_id", node.inborn_hole_id) || node.inborn_hole_id <= 0 || node.inborn_hole_id >= MAX_PARTNER_GIFT_TREE_SKILL_NUM)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "inborn_hole_level", node.inborn_hole_level) || node.inborn_hole_level < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "return_item", node.return_item) || NULL == ITEMPOOL->GetItem(node.return_item))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "return_num", node.return_num) || node.return_num <= 0)
		{
			return -4;
		}

		m_partner_skill_return_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	for (std::vector<PartnerGiftTreeReturnCfg>::iterator it = m_partner_skill_return_vec.begin(); it != m_partner_skill_return_vec.end(); it++)
	{
		if (it->inborn_hole_id > 0 && it->inborn_hole_id <= MAX_PARTNER_GIFT_TREE_SKILL_NUM)
		{
			if (it->inborn_hole_level > 0 && it->inborn_hole_level <= MAX_PARTNER_GIFT_TREE_LEVEL)
			{
				if (partner_skill_return[it->inborn_hole_id - 1][it->inborn_hole_level - 1] == NULL)
				{
					partner_skill_return[it->inborn_hole_id - 1][it->inborn_hole_level - 1] = &(*it);
				}
			}
		}
	}

	return 0;
}
