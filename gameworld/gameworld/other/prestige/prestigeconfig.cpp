#include "prestigeconfig.hpp"
#include "checkresourcecenter.hpp"
#include "item/itempool.h"
#include "servercommon/prestigedef.hpp"

PrestigeConfig::PrestigeConfig()
{
}

PrestigeConfig::~PrestigeConfig()
{
	for (std::map<int, std::vector<PrestigeDonateCfg> >::iterator it = m_donate_map.begin(); it != m_donate_map.end();it++)
	{
		std::vector<PrestigeDonateCfg>().swap(it->second);
	}

	BuyCountVersionVec().swap(m_buy_count_version_vec);
}

bool PrestigeConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("upgrade", InitUpGradeCfg);
	LOAD_CONFIG("shop", InitShopCfg);
	LOAD_CONFIG("donate", InitDonateCfg);
	LOAD_CONFIG("village_list", InitVillageListCfg);
	LOAD_CONFIG("shop_version_replace", InitBuyCountVersionCfg);

	return true;
}

const PrestigeUpGradeCfg * PrestigeConfig::GetUpGradeCfg(int village_id, int level)
{
	std::map<int, std::map<int, PrestigeUpGradeCfg> >::const_iterator it = m_up_grade_cfg.find(village_id);
	if (it == m_up_grade_cfg.end()) return NULL;
	std::map<int, PrestigeUpGradeCfg>::const_iterator level_it = it->second.find(level);
	if (level_it == it->second.end())return NULL;
	return &level_it->second;
}

const PrestigeUpGradeCfg * PrestigeConfig::GetMaxLevelCfg(int village_id)
{
	std::map<int, std::map<int, PrestigeUpGradeCfg> >::const_iterator it = m_up_grade_cfg.find(village_id);
	if (it == m_up_grade_cfg.end()) return NULL;

	if (it->second.empty() || it->second.begin() == it->second.end()) return NULL;

	int key_ = 0;
	for (std::map<int, PrestigeUpGradeCfg>::const_iterator level_it = it->second.begin(); level_it != it->second.end(); level_it++)
	{
		if (level_it->first > key_)
		{
			key_ = level_it->first;
		}
	}

	std::map<int, PrestigeUpGradeCfg>::const_iterator itor = it->second.find(key_);
	if (itor == it->second.end()) return NULL;

	return &itor->second;
}

const PrestigeShopCfg * PrestigeConfig::GetShopCfg(int village_id, int seq)
{
	std::map<int, std::map<int, PrestigeShopCfg> >::const_iterator it = m_shop_cfg.find(village_id);
	if (it == m_shop_cfg.end()) return NULL;
	std::map<int, PrestigeShopCfg>::const_iterator shop_it = it->second.find(seq);
	if (shop_it == it->second.end())return NULL;
	return &shop_it->second;
}

const PrestigeShopCfg * PrestigeConfig::GetShopByIndexCfg(int index)
{
	std::map<int, PrestigeShopCfg>::const_iterator shop_it = m_shop_index_cfg.find(index);
	if (shop_it == m_shop_index_cfg.end())return NULL;
	return &shop_it->second;
}

const PrestigeDonateCfg * PrestigeConfig::GetDonateCfg(int village_id, int color, int level)
{
	std::map<int, std::vector<PrestigeDonateCfg> >::iterator vill_it = m_donate_map.find(village_id);
	if (m_donate_map.end() != vill_it)
	{
		for (std::vector<PrestigeDonateCfg>::iterator donate_it = vill_it->second.begin(); donate_it != vill_it->second.end(); ++donate_it)
		{
			if (color == donate_it->color && level == donate_it->level) return &*donate_it;
		}
	}

	return NULL;
}

const PrestigeVillageCfg * PrestigeConfig::GetVillageCfg(int village_id)
{
	std::map<int, PrestigeVillageCfg>::const_iterator it = m_village_map.find(village_id);
	if (it == m_village_map.end())return NULL;
	return &it->second;
}

int PrestigeConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		if (!GetSubNodeValue(root_element, "donate_max_num", m_other_cfg.donate_max_num) || m_other_cfg.donate_max_num < 0) return -1;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PrestigeConfig::InitUpGradeCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int village_id = 0;
		if (!GetSubNodeValue(root_element, "village_id", village_id) || village_id < 0 || village_id >= PRESTIGE_VILLAGE_MAX_NUM) return -1;
		int relation= 0;
		if (!GetSubNodeValue(root_element, "relation", relation) || relation < 0 || relation >= MAX_UINT8) return -2;
		if (m_up_grade_cfg[village_id].find(relation) != m_up_grade_cfg[village_id].end()) return -3;
		PrestigeUpGradeCfg& node_cfg = m_up_grade_cfg[village_id][relation];
		node_cfg.village_id = village_id;
		node_cfg.relation = relation;

		if (!GetSubNodeValue(root_element, "need_prestige", node_cfg.need_prestige) || node_cfg.need_prestige < 0) return -4;
		if (!GetSubNodeValue(root_element, "title_id", node_cfg.title_id) || node_cfg.title_id < 0) return -5;

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PrestigeConfig::InitShopCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int village_id = 0;
		if (!GetSubNodeValue(root_element, "village_id", village_id) || village_id < 0 || village_id >= PRESTIGE_VILLAGE_MAX_NUM) return -1;
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq < 0) return -2;
		if (m_shop_cfg[village_id].find(seq) != m_shop_cfg[village_id].end()) return -3;
		PrestigeShopCfg& node_cfg = m_shop_cfg[village_id][seq];
		node_cfg.village_id = village_id;
		node_cfg.seq = seq;

		if (!GetSubNodeValue(root_element, "index", node_cfg.index) || node_cfg.index < 0 || node_cfg.index >= NEW_PRESTIGE_VILLAGE_GOODS_MAX_NUM) return -10;
		if (!GetSubNodeValue(root_element, "relation", node_cfg.relation) || node_cfg.relation < 0) return -4;
		if (!GetSubNodeValue(root_element, "item_id", node_cfg.item_id) || node_cfg.item_id < 0) return -5;
		if (!GetSubNodeValue(root_element, "price", node_cfg.price) || node_cfg.price < 0) return -6;
		if (!GetSubNodeValue(root_element, "is_bind", node_cfg.is_bind) || node_cfg.is_bind < 0) return -7;
		if (!GetSubNodeValue(root_element, "limit_type", node_cfg.limit_type) || node_cfg.limit_type < PRESTIGE_SHOP_LIMIT_TYPE_BEGIN || node_cfg.limit_type >= PRESTIGE_SHOP_LIMIT_TYPE_END) return -8;
		if (!GetSubNodeValue(root_element, "limit_times", node_cfg.limit_times) || node_cfg.limit_times < 0 || node_cfg.limit_times >= MAX_UINT8) return -9;
		if (!GetSubNodeValue(root_element, "money_type", node_cfg.money_type) || node_cfg.money_type < 0 || node_cfg.money_type >= CURRENCY_TYPE_MAX) return -10;

		if (!GetSubNodeValue(root_element, "require_type", node_cfg.require_type) || node_cfg.require_type < PRESTIGE_SHOP_REQUIRE_TYPE_NO || node_cfg.require_type >= PRESTIGE_SHOP_REQUIRE_TYPE_END) return -1;
		if (!GetSubNodeValue(root_element, "require_value", node_cfg.require_value) || node_cfg.require_value < 0) return -12;

		if (CURRENCY_TYPE_COIN_BIND == node_cfg.money_type)
		{
			if (!ITEMPOOL->CheckItemLegal(node_cfg.item_id, node_cfg.price, __FUNCTION__))
			{
				return -33;
			}
		}

		m_shop_index_cfg[node_cfg.index] = node_cfg;
		m_shop_limit_buy_count_cfg[node_cfg.limit_type][node_cfg.index] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PrestigeConfig::InitDonateCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int village_id = 0; PrestigeDonateCfg node;
		if (!GetSubNodeValue(root_element, "color", node.color)) return -1;
		if (!GetSubNodeValue(root_element, "level", node.level)) return -2;
		if (!GetSubNodeValue(root_element, "reward_prestige", node.reward_prestige)) return -3;
		if (!GetSubNodeValue(root_element, "village_id", village_id)) return -4;

		std::map<int, std::vector<PrestigeDonateCfg> >::iterator vill_it = m_donate_map.find(village_id);
		if (m_donate_map.end() != vill_it)
		{
			vill_it->second.push_back(node);
		}
		else
		{
			std::vector<PrestigeDonateCfg> tmp_list; tmp_list.push_back(node);
			m_donate_map[village_id] = tmp_list;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PrestigeConfig::InitVillageListCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		PrestigeVillageCfg node_cfg;
		if (!GetSubNodeValue(root_element, "village_id", node_cfg.village_id) || node_cfg.village_id < 0 || node_cfg.village_id >= PRESTIGE_VILLAGE_MAX_NUM) return -1;
		if (!GetSubNodeValue(root_element, "donate_min_level", node_cfg.donate_min_level) || node_cfg.donate_min_level < 0) return -2;
		if (!GetSubNodeValue(root_element, "donate_max_level", node_cfg.donate_max_level) || node_cfg.donate_max_level < node_cfg.donate_min_level) return -3;
	
		m_village_map[node_cfg.village_id] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PrestigeConfig::InitBuyCountVersionCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return 0; // 这个配置没有也没所谓

	while (NULL != data_element)
	{
		PrestigeBuyCountVersionCfg cfg;
		if (!GetSubNodeValue(data_element, "version", cfg.version) || cfg.version <= 0 || cfg.version >= MAX_INT8)
		{
			return -1;
		}

		if (!GetSubNodeValue(data_element, "reset_index", cfg.index) || cfg.index < 0 || cfg.index >= NEW_PRESTIGE_VILLAGE_GOODS_MAX_NUM)
		{
			return -2;
		}

		m_buy_count_version_vec.push_back(cfg);

		data_element = data_element->NextSiblingElement();
	}

	for (size_t i = 1; i < m_buy_count_version_vec.size(); ++i)
	{
		if (m_buy_count_version_vec[i].version < m_buy_count_version_vec[i - 1].version ||
			m_buy_count_version_vec[i].version - m_buy_count_version_vec[i - 1].version > 1)
		{
			printf("InitBuyCountVersionCfg please make sure [version] is equal or plus 1");
			return -999;
		}
	}

	return 0;
}
