#include "mysterystoreconfig.hpp"
#include "item/itempool.h"
#include "item/itembase.h"
#include "skill/skillpool.hpp"
#include "gamelog.h"
#include "checkresourcecenter.hpp"
#include "servercommon/performancecatch.hpp"

MysteryStoreConfig::MysteryStoreConfig()
{
}

MysteryStoreConfig::~MysteryStoreConfig()
{
	for (std::map<LevelLimit, std::vector<MysteryStoreStoreCfg> >::iterator it = m_store_cfg.begin(); it != m_store_cfg.end(); it++)
	{
		std::vector<MysteryStoreStoreCfg>().swap(it->second);
	}

	for (std::map<int, std::vector<MysteryStoreItemGroupCfg> >::iterator it = m_item_group_cfg.begin(); it != m_item_group_cfg.end(); it++)
	{
		std::vector<MysteryStoreItemGroupCfg>().swap(it->second);
	}
	std::vector<MysterStoreConsumeAddCfg>().swap(m_consume_add_cfg);
}

bool MysteryStoreConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("mystery_shop", InitMysteryStoreStoreCfg)
	LOAD_CONFIG("item_group", InitMysteryStoreItemCfg);
	LOAD_CONFIG("other", InitMysteryStoreOtherCfg);
	LOAD_CONFIG("consume_add", InitMysteryStoreConAddCfg);
	LOAD_CONFIG("item_unlock", InitMysteryStoreLimitCfg);

	return true;
}

const int MysteryStoreConfig::GetStoreCfgSize(int level)
{
	for (std::map<LevelLimit, std::vector<MysteryStoreStoreCfg> >::const_iterator it = m_store_cfg.begin(); it != m_store_cfg.end(); ++it)
	{
		if (it->first.level_min <= level && it->first.level_max >= level)
		{
			return (int)it->second.size();
		}
	}
	return 0;
}

const std::vector<MysteryStoreStoreCfg> * MysteryStoreConfig::GetStoreCfg(int level)
{
	for (std::map<LevelLimit, std::vector<MysteryStoreStoreCfg> >::const_iterator it = m_store_cfg.begin(); it != m_store_cfg.end(); ++it)
	{
		if (it->first.level_min <= level && it->first.level_max >= level)
		{
			return &it->second;
		}
	}
	return NULL;
}

const MysteryStoreItemGroupCfg * MysteryStoreConfig::RandGetItemCfg(int item_group, int pass_level)
{
	std::map<int, std::vector<MysteryStoreItemGroupCfg> >::const_iterator it = m_item_group_cfg.find(item_group);
	if (m_item_group_cfg.end() == it) return NULL;

	int all_weight = 0;
	static std::vector<MysteryStoreItemGroupCfg> temp_pool;
	temp_pool.clear();
	for (std::vector<MysteryStoreItemGroupCfg>::const_iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
	{
		std::map<int, int >::const_iterator limit_it = m_mystery_store_limit_cfg.find(vit->item.item_id);
		if (m_mystery_store_limit_cfg.end() != limit_it)
		{
			if (limit_it->second > pass_level)
			{
				continue;
			}
		}

		all_weight += vit->rate;
		temp_pool.push_back(*vit);
	}

	int rand_weight = RandomNum(all_weight);
	for (std::vector<MysteryStoreItemGroupCfg>::const_iterator vit = temp_pool.begin(); vit != temp_pool.end(); ++vit)
	{
		if (rand_weight < vit->rate)
		{
			return &(*vit);
		}
		rand_weight -= vit->rate;
	}

	gamelog::g_log_partner.printf(LL_INFO, "%s Error item_group[%d] pass_level[%d]", __FUNCTION__, item_group, pass_level);
	return NULL;
}

const MysteryStoreItemGroupCfg * MysteryStoreConfig::GetItemCfg(int group_id, int seq)
{
	std::map<int, std::vector<MysteryStoreItemGroupCfg> >::const_iterator it = m_item_group_cfg.find(group_id);
	if (m_item_group_cfg.end() == it) return NULL;

	for (std::vector<MysteryStoreItemGroupCfg>::const_iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
	{
		if (seq == vit->seq)
		{
			return &(*vit);
		}
	}

	return NULL;
}

const MysterStoreConsumeAddCfg * MysteryStoreConfig::GetConsumeAddCfg(int refresh_time)
{
	for (int i = 0; i < (int)m_consume_add_cfg.size(); i++)
	{
		if (refresh_time < m_consume_add_cfg[i].manual_refresh_times)
		{
			return &m_consume_add_cfg[i];
		}
	}

	return NULL;
}

int MysteryStoreConfig::InitMysteryStoreStoreCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int level_min, level_max = 0;
		if (!GetSubNodeValue(root_element, "level_min", level_min) || level_min <= 0) return -1;
		if (!GetSubNodeValue(root_element, "level_max", level_max) || level_max > 120) return -2;

		LevelLimit l_t = LevelLimit(level_min, level_max);
		std::vector<MysteryStoreStoreCfg> & node = m_store_cfg[l_t];
		MysteryStoreStoreCfg cfg;

		if (!GetSubNodeValue(root_element, "item_group", cfg.item_group) || cfg.item_group <= 0) return -3;

		node.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int MysteryStoreConfig::InitMysteryStoreItemCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", group_id) || group_id <= 0) return -1;

		std::vector<MysteryStoreItemGroupCfg> & node = m_item_group_cfg[group_id];
		MysteryStoreItemGroupCfg cfg;

		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || cfg.seq < 0) return -2;
		if (!GetSubNodeValue(root_element, "rate", cfg.rate) || cfg.rate <= 0) return -3;
		if (!GetSubNodeValue(root_element, "price_type", cfg.price_type) || cfg.price_type < MONEY_TYPE_GOLD || cfg.price_type >= MONEY_TYPE_MAX) return -4;
		if (!GetSubNodeValue(root_element, "price", cfg.price) || cfg.price < 0) return -5;
		if (!GetSubNodeValue(root_element, "limit_convert_count", cfg.limit_buy) || cfg.limit_buy < 0) return -6;

		TiXmlElement * item_element = root_element->FirstChildElement("item");
		if (NULL != item_element)
		{
			if (!cfg.item.ReadConfig(item_element)) return -100;
		}

		node.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int MysteryStoreConfig::InitMysteryStoreOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "auto_time", m_other_cfg.auto_time) || m_other_cfg.auto_time <= 0) return -1;
	if (!GetSubNodeValue(root_element, "reset", m_other_cfg.mystery_store_ver) || m_other_cfg.mystery_store_ver < 0) return -2;

	return 0;
}

int MysteryStoreConfig::InitMysteryStoreConAddCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	int last_manual_refresh_times = -1;
	while (NULL != root_element)
	{	
		int manual_refresh_times;
		if (!GetSubNodeValue(root_element, "manual_refresh_times", manual_refresh_times) || manual_refresh_times < 0) return -1;
		if (0 == manual_refresh_times)
		{
			manual_refresh_times = MAX_INT;
		}
		if (-1 != last_manual_refresh_times)
		{
			if(manual_refresh_times < last_manual_refresh_times) return -2;
		}
		last_manual_refresh_times = manual_refresh_times;
		MysterStoreConsumeAddCfg node;
		node.manual_refresh_times = manual_refresh_times;

		if (!GetSubNodeValue(root_element, "manual_price_type", node.refresh_money_type) || node.refresh_money_type < MONEY_TYPE_GOLD || node.refresh_money_type > MONEY_TYPE_LUCK_COIN) return -2;
		if (!GetSubNodeValue(root_element, "manual_price", node.refresh_price) || node.refresh_price < 0) return -3;
		if (!GetSubNodeValue(root_element, "item_first", node.is_priority_use_item) || node.is_priority_use_item < 0) return -4;
		TiXmlElement * item_element = root_element->FirstChildElement("item");
		if (NULL != item_element)
		{
			if (!node.consume_item.ReadConfig(item_element)) return -100;
		}
		m_consume_add_cfg.push_back(node);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int MysteryStoreConfig::InitMysteryStoreLimitCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int item_id, level;
		if (!GetSubNodeValue(root_element, "item_id", item_id) || item_id < 0) return -1;
		if (!GetSubNodeValue(root_element, "dream_notes_level", level) || level < 0) return -2;
		
		m_mystery_store_limit_cfg[item_id] = level;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}
