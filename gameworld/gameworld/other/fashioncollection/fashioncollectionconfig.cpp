#include "fashioncollectionconfig.hpp"
#include "servercommon/struct/itemlistparam.h"
#include "item/itempool.h"

FashionCollectionConfig::FashionCollectionConfig()
{
}

FashionCollectionConfig::~FashionCollectionConfig()
{
	for (int i = 0; i < PROFESSION_TYPE_NUM; i++)
	{
		for (LevelUpMap::iterator it = m_level_up_cfg[i].begin(); it != m_level_up_cfg[i].end(); it++)
		{
			std::vector<FashionCollectionLevelUpCfg>().swap(it->second);
		}
	}

	for (StarAddMap::iterator it = m_star_add_cfg.begin(); it != m_star_add_cfg.end(); it++)
	{
		std::vector<FashionCollectionStarAddCfg>().swap(it->second);
	}
}

bool FashionCollectionConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("other", InitOtherCfg);
	LOAD_CONFIG_2("fashion_level_up", InitFashionLevelUpCfg);
	LOAD_CONFIG_2("star_add", InitStarAddCfg);
	LOAD_CONFIG_2("fashion_resolve", InitDecomposeCfg);

	return true;
}

const FashionCollectionLevelUpCfg * FashionCollectionConfig::GetLevelUpCfg(int prof, int fashion_id, int level) const
{
	if (prof <= PROF_TYPE_INVALID || prof >= PROF_TYPE_MAX) return NULL;
	int base_prof = prof / PROFESSION_BASE;
	if (base_prof <= 0 || base_prof >= PROFESSION_TYPE_NUM) return NULL;

	LevelUpMap::const_iterator it = m_level_up_cfg[base_prof].find(fashion_id);
	if (it == m_level_up_cfg[base_prof].end()) return NULL;
	
	for (size_t i = 0; i < it->second.size(); i++)
	{
		if (level == it->second[i].fashion_level)
		{
			return &it->second[i];
		}
	}

	return NULL;
}

const FashionCollectionStarAddCfg * FashionCollectionConfig::GetStarCfg(int prof, int level) const
{
	StarAddMap::const_iterator it = m_star_add_cfg.find(prof);
	if (it == m_star_add_cfg.end()) return NULL;

	for (size_t i = 0; i < it->second.size(); i++)
	{
		if (it->second[i].level == level)
		{
			return &it->second[i];
		}
	}

	return NULL;
}

const std::pair<int, int>* FashionCollectionConfig::GetDecomposeCfg(int item_id) const
{
	std::map<int, std::pair<int, int> >::const_iterator it = m_decompose_cfg.find(item_id);
	if (it != m_decompose_cfg.end())
	{
		return &it->second;
	}

	return NULL;
}

int FashionCollectionConfig::InitFashionLevelUpCfg(TiXmlElement * RootElement, std::string & err_msg)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int fashion_id = 0;
		if (!GetSubNodeValue(data_element, "fashion_id", fashion_id) || fashion_id <= 0 || fashion_id >= SURFACE_MAX_NUM)
		{
			err_msg = STRING_SPRINTF("fashion_id[%d] <= 0 || >= %d", fashion_id, SURFACE_MAX_NUM);
			return -1;
		}

		int limit_prof = 0;
		if (!GetSubNodeValue(data_element, "limit_prof", limit_prof) || limit_prof <= PROF_TYPE_INVALID || limit_prof >= PROF_TYPE_MAX)
		{
			err_msg = STRING_SPRINTF("limit_prof[%d] <= %d || >= %d", limit_prof, PROF_TYPE_INVALID, PROF_TYPE_MAX);
			return -111;
		}

		limit_prof /= PROFESSION_BASE;
		if (limit_prof <= 0 || limit_prof >= PROFESSION_TYPE_NUM)
		{
			return -112;
		}

		LevelUpMap::iterator it = m_level_up_cfg[limit_prof].find(fashion_id);
		if (it == m_level_up_cfg[limit_prof].end())
		{
			m_level_up_cfg[limit_prof].insert(std::make_pair(fashion_id, std::vector<FashionCollectionLevelUpCfg>()));
			it = m_level_up_cfg[limit_prof].find(fashion_id);
		}

		FashionCollectionLevelUpCfg level_up_cfg;

		if (!GetSubNodeValue(data_element, "fashion_level", level_up_cfg.fashion_level) || level_up_cfg.fashion_level <= 0 || level_up_cfg.fashion_level > m_other_cfg.max_level)
		{
			err_msg = STRING_SPRINTF("fashion_level[%d] <= 0 || > %d", level_up_cfg.fashion_level, m_other_cfg.max_level);
			return -2;
		}

		std::string attr_type("attr_type_"), attr_add("attr_add_");
		for (int i = 1; i <= 6; i++)
		{
			AttrItemCfg attr;
			if (!GetSubNodeValue(data_element, attr_type + STRING_SPRINTF("%d", i), attr.attr_type) || attr.attr_type < 0 || attr.attr_type >= BATTLE_ATTR_MAX)
			{
				err_msg = STRING_SPRINTF("attr_type_%d[%d] < 0 || >= %d", i, attr.attr_type, BATTLE_ATTR_MAX);
				return -3;
			}

			if (!GetSubNodeValue(data_element, attr_add + STRING_SPRINTF("%d", i), attr.attr_value) || attr.attr_value < 0)
			{
				err_msg = STRING_SPRINTF("attr_add_%d[%d] < 0", i, attr.attr_value);
				return -4;
			}

			level_up_cfg.attr_list.push_back(attr);
		}

		TiXmlElement* consume_list_element = data_element->FirstChildElement("strengthen_consume_item_list");
		if (NULL == consume_list_element) return -543;
		TiXmlElement* consume_element = consume_list_element->FirstChildElement("strengthen_consume_item");
		if (NULL == consume_element) return -542;

		while (consume_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(consume_element)) return -5;

			level_up_cfg.consume_list.push_back(item);

			consume_element = consume_element->NextSiblingElement();
		}

		it->second.push_back(level_up_cfg);

		if (it->second.size() > 255) return -6;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int FashionCollectionConfig::InitStarAddCfg(TiXmlElement * RootElement, std::string & err_msg)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int prof_limit = -1;
		if (!GetSubNodeValue(data_element, "limit_prof", prof_limit) || prof_limit < 0 || prof_limit >= PROF_TYPE_MAX)
		{
			err_msg = STRING_SPRINTF("limit_prof[%d] < 0 || >= %d", prof_limit, PROF_TYPE_MAX);
			return -1;
		}

		StarAddMap::iterator it = m_star_add_cfg.find(prof_limit);
		if (it == m_star_add_cfg.end())
		{
			m_star_add_cfg[prof_limit] = std::vector<FashionCollectionStarAddCfg>();
			it = m_star_add_cfg.find(prof_limit);
		}

		FashionCollectionStarAddCfg star_add_cfg;

		if (!GetSubNodeValue(data_element, "level", star_add_cfg.level) || star_add_cfg.level <= 0 || star_add_cfg.level > m_other_cfg.max_star_level)
		{
			err_msg = STRING_SPRINTF("level[%d] <= 0 || > %d", star_add_cfg.level, m_other_cfg.max_star_level);
			return -101;
		}

		if (!GetSubNodeValue(data_element, "star_level", star_add_cfg.star_level) || star_add_cfg.star_level < 0)
		{
			err_msg = STRING_SPRINTF("star_level[%d] < 0", star_add_cfg.star_level);
			return -2;
		}

		std::string attr_type("attr_type_"), attr_add("attr_add_");
		for (int i = 1; i <= 6; i++)
		{
			AttrItemCfg attr;
			if (!GetSubNodeValue(data_element, attr_type + STRING_SPRINTF("%d", i), attr.attr_type) || attr.attr_type < 0 || attr.attr_type >= BATTLE_ATTR_MAX)
			{
				err_msg = STRING_SPRINTF("attr_type_%d[%d] < 0 || >= %d", i, attr.attr_type, BATTLE_ATTR_MAX);
				return -3;
			}

			if (!GetSubNodeValue(data_element, attr_add + STRING_SPRINTF("%d", i), attr.attr_value) || attr.attr_value < 0)
			{
				err_msg = STRING_SPRINTF("attr_add_%d[%d] < 0", i, attr.attr_value);
				return -4;
			}

			star_add_cfg.attr_list.push_back(attr);
		}

		it->second.push_back(star_add_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int FashionCollectionConfig::InitDecomposeCfg(TiXmlElement * RootElement, std::string & err_msg)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int consume_item_id;
		if (!GetSubNodeValue(data_element, "item_id", consume_item_id) || NULL == ITEMPOOL->GetItem(consume_item_id))
		{
			err_msg = STRING_SPRINTF("item_id[%d] can not find", consume_item_id);
			return -1;
		}

		std::pair<int, int> cfg;
		if (!GetSubNodeValue(data_element, "decompose_item", cfg.first) || NULL == ITEMPOOL->GetItem(cfg.first))
		{
			err_msg = STRING_SPRINTF("item_id[%d] can not find", cfg.first);
			return -2;
		}

		if (!GetSubNodeValue(data_element, "decompose_num", cfg.second) || cfg.second <= 0) return -3;

		if (m_decompose_cfg.find(consume_item_id) != m_decompose_cfg.end())
		{
			err_msg = STRING_SPRINTF("item_id[%d] repeat", consume_item_id);
			return -4;
		}

		m_decompose_cfg[consume_item_id] = cfg;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int FashionCollectionConfig::InitOtherCfg(TiXmlElement * RootElement, std::string & err_msg)
{
	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	if (!GetSubNodeValue(data_element, "max_level", m_other_cfg.max_level) || m_other_cfg.max_level <= 0) return -1;
	if (!GetSubNodeValue(data_element, "max_star_level", m_other_cfg.max_star_level) || m_other_cfg.max_star_level <= 0) return -2;

	return 0;
}
