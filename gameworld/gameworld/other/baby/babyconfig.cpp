#include "babyconfig.hpp"

BabyConfig::BabyConfig()
{
}

BabyConfig::~BabyConfig()
{
	std::vector<BabyLevelCfg>().swap(m_baby_lvl_cfg);
}

bool BabyConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("children_add_attribute", InitBabyLevelCfg);
	LOAD_CONFIG("chouqian", InitDrawCfg);

	return true;
}

const BabyOtherCfg & BabyConfig::GetOtherCfg()
{
	return m_other_cfg;
}

const BabyLevelCfg * BabyConfig::GetBabyLevelCfg(int level)
{
	if (0 <= level && level <= (int)m_baby_lvl_cfg.size())
	{
		return &m_baby_lvl_cfg[level];
	}

	return NULL;
}

int BabyConfig::GetBabyMaxLevel()
{
	return (int)m_baby_lvl_cfg.size();
}

BabyDrawCfg::ItemCfg * BabyConfig::GetRandDrawItem()
{
	int r = rand() % m_draw_cfg.rate_count;

	for (int i = 0; i < (int)m_draw_cfg.item_cfg.size(); ++i)
	{
		if (r < m_draw_cfg.item_cfg[i].rate)
		{
			return &m_draw_cfg.item_cfg[i];
		}
	}

	return NULL;
}

int BabyConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "limit_level", m_other_cfg.create_need_role_level) || m_other_cfg.create_need_role_level >= MAX_ROLE_LEVEL)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "upgrade_item_id", m_other_cfg.uplvl_item_id) || m_other_cfg.uplvl_item_id <= 0)
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "add_exp", m_other_cfg.add_exp) || m_other_cfg.add_exp <= 0)
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "qiuzi_jifen", m_other_cfg.single_need_ji_fen) || m_other_cfg.single_need_ji_fen <= 0)
	{
		return -4;
	}

	if (!GetSubNodeValue(root_element, "coin_num", m_other_cfg.contribute_coin_num) || m_other_cfg.contribute_coin_num <= 0)
	{
		return -5;
	}

	if (!GetSubNodeValue(root_element, "coin_jifen", m_other_cfg.contribute_coin_jifen) || m_other_cfg.contribute_coin_jifen <= 0)
	{
		return -6;
	}

	if (!GetSubNodeValue(root_element, "xianyu_num", m_other_cfg.contribute_xianyu_num) || m_other_cfg.contribute_xianyu_num <= 0)
	{
		return -7;
	}

	if (!GetSubNodeValue(root_element, "xianyu_jifen", m_other_cfg.contribute_xianyu_jifen) || m_other_cfg.contribute_xianyu_jifen <= 0)
	{
		return -8;
	}

	if (!GetSubNodeValue(root_element, "gold_num", m_other_cfg.contribute_gold_num) || m_other_cfg.contribute_gold_num <= 0)
	{
		return -9;
	}

	if (!GetSubNodeValue(root_element, "gold_jifen", m_other_cfg.contribute_gold_jifen) || m_other_cfg.contribute_gold_jifen <= 0)
	{
		return -10;
	}

	if (!GetSubNodeValue(root_element, "item_name_change", m_other_cfg.rename_item_id) || m_other_cfg.rename_item_id <= 0)
	{
		return -11;
	}

	for (int i = 0; i < CREATE_BABY_STAGE_MAX; ++i)
	{
		char stage_condition[64] = { 0 };
		SNPRINTF(stage_condition, sizeof(stage_condition), "stage_%d", i);
		if (!GetSubNodeValue(root_element, stage_condition, m_other_cfg.stage_condition[i]) || m_other_cfg.stage_condition[i] <= 0)
		{
			return -12;
		}
	}

	return 0;
}

int BabyConfig::InitBabyLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		BabyLevelCfg cfg;

		if (!GetSubNodeValue(root_element, "children_level", cfg.level))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "exp", cfg.exp))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "maxhp", cfg.maxhp))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "maxmp", cfg.maxmp))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "gongji", cfg.gongji))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "fangyu", cfg.fangyu))
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "minjie", cfg.minjie))
		{
			return -7;
		}

		if (!GetSubNodeValue(root_element, "jingshen", cfg.jingshen))
		{
			return -8;
		}

		if (!GetSubNodeValue(root_element, "huifu", cfg.huifu))
		{
			return -9;
		}

		m_baby_lvl_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int BabyConfig::InitDrawCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		BabyDrawCfg::ItemCfg cfg;

		if (!GetSubNodeValue(root_element, "jifen", cfg.add_ji_fen) || cfg.add_ji_fen <= 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "rate", cfg.rate) || cfg.rate <= 0)
		{
			return -2;
		}

		m_draw_cfg.rate_count += cfg.rate;
		cfg.rate = m_draw_cfg.rate_count;

		m_draw_cfg.item_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
