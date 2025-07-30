#include <set>

#include "item/itempool.h"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "medalconfig.hpp"

MedalConfig::MedalConfig()
{
}

MedalConfig::~MedalConfig()
{
	std::vector<MedalUpgradeCfg>().swap(m_upgrade_cfg);
}

bool MedalConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("att_group", InitAttrGroupCfg);
	LOAD_CONFIG("sp_group", InitSpGroupCfg);
	LOAD_CONFIG("medal_resolve", InitDecomposeCfg);
	LOAD_CONFIG("medal_exchange", InitExchangeCfg);
	LOAD_CONFIG("reward_group", InitExchangeRewardGroupCfg);
	LOAD_CONFIG("rank_up", InitUpgradeCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("market_medal", InitTradeCfg);
	LOAD_CONFIG("special_medal_exchange", InitSpecialExchangeCfg);

	return true;
}

const MedalItemAttrsCfg * MedalConfig::GetRandMedalItemAttrsCfg(int attr_group_id)
{
	std::map<int, MedalItemAttrGroupCfg>::iterator iter = m_medal_attr_cfg.find(attr_group_id);
	if (iter == m_medal_attr_cfg.end())
	{
		return NULL;
	}

	const MedalItemAttrGroupCfg & cfg = iter->second;
	int r = RandomNum(cfg.rate_count);
	for (int i = 0; i < (int)cfg.attrs.size(); ++i)
	{
		if (r < cfg.attrs[i].rate)
		{
			return &cfg.attrs[i];
		}

		r -= cfg.attrs[i].rate;
	}

	return NULL;
}

const MedalSpGroupCfg::ItemCfg * MedalConfig::GetRandMedalItemSpItemCfg(int sp_group_id)
{
	std::map<int, MedalSpGroupCfg>::iterator iter = m_medal_sp_cfg.find(sp_group_id);
	if (iter == m_medal_sp_cfg.end())
	{
		return NULL;
	}

	const MedalSpGroupCfg & cfg = iter->second;
	int r = RandomNum(MedalSpGroupCfg::RATE_COUNT);
	for (int i = 0; i < (int)cfg.item_cfgs.size(); ++i)
	{
		if (r < cfg.item_cfgs[i].rate)
		{
			return &cfg.item_cfgs[i];
		}

		r -= cfg.item_cfgs[i].rate;
	}

	return NULL;
}

const MedalDecomposeCfg * MedalConfig::GetMedalDecomposeCfg(int grade)
{
	std::map<int, MedalDecomposeCfg>::iterator iter = m_medal_decompose_cfg.find(grade);
	if (iter != m_medal_decompose_cfg.end())
	{
		return &iter->second;
	}

	return NULL;
}

const MedalExchangeCfg * MedalConfig::GetMedalExchangeCfg(int seq)
{
	std::map<int, MedalExchangeCfg>::iterator it = m_medal_exchange_cfg.find(seq);
	if (it != m_medal_exchange_cfg.end())
	{
		return &it->second;
	}
	return NULL;
}

const MedalExchangeRewardCfg * MedalConfig::GetRandRewardInfoByRandGroup(int group_id)
{
	std::map<int, MedalExchangeRewardListCfg>::iterator it = m_medal_exchange_reward_list_cfg.find(group_id);
	if (it != m_medal_exchange_reward_list_cfg.end())
	{
		if (it->second.weight_count != 0)
		{
			int real_weight = rand() % it->second.weight_count;
			for (std::vector<MedalExchangeRewardCfg>::iterator itor = it->second.reward_list.begin(); itor != it->second.reward_list.end(); ++itor)
			{
				if (itor->rate >= real_weight)
				{
					return &(*itor);
				}
				else
				{
					real_weight -= itor->rate;
				}
			}
		}
	}
	return NULL;
}

const MedalUpgradeCfg * MedalConfig::GetMedalUpgradeCfg(int grade)
{
	if (grade < 0 || grade >= (int)m_upgrade_cfg.size())
	{
		return NULL;
	}

	return &m_upgrade_cfg[grade];
}

int MedalConfig::GetMedalMinGradeByColor(int color)
{
	for (int index  = 0; index < (int)m_upgrade_cfg.size(); ++index)
	{
		if (m_upgrade_cfg[index].color == color)
		{
			return m_upgrade_cfg[index].grade;
		}
	}

	return 0;
}

int MedalConfig::GetUpgradeRate(int main_level, int consume_level)
{
	int index = main_level;
	if (index < 0 || index >= (int)m_upgrade_cfg.size())
	{
		return 0;
	}

	int index2 = consume_level;
	if (index2 < 0 || index2 >= ARRAY_LENGTH(m_upgrade_cfg[index].add_rate))
	{
		return 0;
	}

	return m_upgrade_cfg[index].rate + m_upgrade_cfg[index].add_rate[index2];
}

const MedalOtherCfg & MedalConfig::GetMedalOtherCfg()
{
	return m_other_cfg;
}

const MedalMarketCfg* MedalConfig::GetMedalMarketCfg(int grade)
{
	if (grade < 0 || grade >= ARRAY_LENGTH(m_medal_grade_trade_cfg)) return NULL;

	return &m_medal_grade_trade_cfg[grade];
}

bool MedalConfig::RandMedalParam(NetValueItemParam * param_data, int attr_group, int sp_group, int init_rank)
{
	if (NULL == param_data)
	{
		return false;
	}

	MedalItemParam & data = param_data->medal_param;
	data.has_net_value = 1;
	data.attr_num = 0;
	data.sp_id = 0;

	const MedalItemAttrsCfg * attr_cfg = this->GetRandMedalItemAttrsCfg(attr_group);
	if (attr_cfg != NULL)
	{
		for (int i = 0; i < (int)attr_cfg->attrs.size() && data.attr_num < ARRAY_LENGTH(data.attrs); ++i)
		{
			const MedalItemAttrsCfg::AttrItem & attr_item_cfg = attr_cfg->attrs[i];

			if (attr_item_cfg.attr_type < BATTLE_ATTR_MIN || attr_item_cfg.attr_type >= BATTLE_ATTR_MAX)
			{
				continue;
			}
			int r = RandomNum(attr_item_cfg.attr_min, attr_item_cfg.attr_max + 1);
			if (r == 0)
			{
				continue;
			}

			data.attrs[data.attr_num].attr_type = attr_item_cfg.attr_type;
			data.attrs[data.attr_num].attr_value = r;
			data.attr_num += 1;
		}
	}

	const MedalSpGroupCfg::ItemCfg * sp_cfg = this->GetRandMedalItemSpItemCfg(sp_group);
	if (sp_cfg != NULL)
	{
		data.sp_id = sp_cfg->sp_id;
		const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(data.sp_id);
		if (spe_cfg != NULL)
		{
			data.grade = spe_cfg->medal_grade;
		}
	}
	else
	{
		data.sp_id = 0;
		data.grade = init_rank;
	}

	return true;
}

bool MedalConfig::IsSpeMedalItem(int sp_id)
{
	std::set<int>::iterator it = m_special_medal_exchange_set.find(sp_id);
	if(it == m_special_medal_exchange_set.end()) return false;

	return true;
}

int MedalConfig::InitAttrGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -__LINE__;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "att_group", group_id))
		{
			return -__LINE__;
		}

		MedalItemAttrGroupCfg & cfg = m_medal_attr_cfg[group_id];
		MedalItemAttrsCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -__LINE__;
		}

		for (int i = 0; i < MAX_MEDAL_MAX_ATTR_NUM; ++i)
		{
			MedalItemAttrsCfg::AttrItem attr_item;

			std::string attr_type = STRING_SPRINTF("attr_type_%d", i + 1);
			std::string attr_min = STRING_SPRINTF("attr_min_%d", i + 1);
			std::string attr_max = STRING_SPRINTF("attr_max_%d", i + 1);

			if (!GetSubNodeValue(root_element, attr_type, attr_item.attr_type))
			{
				return -__LINE__;
			}

			if (attr_item.attr_type < 0)
			{
				continue;
			}

			if (!GetSubNodeValue(root_element, attr_min, attr_item.attr_min) || attr_item.attr_min < 0)
			{
				return -__LINE__;
			}

			if (!GetSubNodeValue(root_element, attr_max, attr_item.attr_max) || attr_item.attr_max <= 0 || attr_item.attr_max < attr_item.attr_min)
			{
				return -__LINE__;
			}

			item_cfg.attrs.push_back(attr_item);
		}

		cfg.rate_count += item_cfg.rate;
		cfg.attrs.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int MedalConfig::InitSpGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -__LINE__;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "sp_group", group_id))
		{
			return -__LINE__;
		}

		MedalSpGroupCfg & cfg = m_medal_sp_cfg[group_id];
		MedalSpGroupCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "sp_id", item_cfg.sp_id))
		{
			return -__LINE__;
		}

		cfg.item_cfgs.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int MedalConfig::InitDecomposeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (dataElement)
	{
		MedalDecomposeCfg cfg;
		if (!GetSubNodeValue(dataElement, "rank", cfg.medal_grade))
		{
			return -__LINE__;
		}

		TiXmlElement * item_element = dataElement->FirstChildElement("resolve_item");
		if (!cfg.get_item.ReadConfig(item_element))
		{
			return -__LINE__;
		}

		m_medal_decompose_cfg[cfg.medal_grade] = cfg;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int MedalConfig::InitExchangeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (dataElement)
	{
		int seq;
		if (!GetSubNodeValue(dataElement, "seq", seq) || seq < 0)
		{
			return -__LINE__;
		}
		MedalExchangeCfg &node_cfg = m_medal_exchange_cfg[seq];

		if (!GetSubNodeValue(dataElement, "level_min", node_cfg.level_min))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "level_max", node_cfg.level_max))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "item_id", node_cfg.item_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "one_num", node_cfg.one_num))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "ten_num", node_cfg.ten_num))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "reward_group", node_cfg.reward_group))
		{
			return -__LINE__;
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int MedalConfig::InitExchangeRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (dataElement)
	{
		int reward_group;
		if (!GetSubNodeValue(dataElement, "reward_group", reward_group) || reward_group < 0)
		{
			return -__LINE__;
		}

		MedalExchangeRewardListCfg & node_cfg = m_medal_exchange_reward_list_cfg[reward_group];

		MedalExchangeRewardCfg cfg;
		if (!GetSubNodeValue(dataElement, "rate", cfg.rate))
		{
			return -__LINE__;
		}
		node_cfg.weight_count += cfg.rate;

		TiXmlElement * item_element = dataElement->FirstChildElement("reward");
		if (!cfg.item.ReadConfig(item_element))
		{
			return -__LINE__;
		}
		int is_broadcast = 0;
		if (!GetSubNodeValue(dataElement, "is_broadcast", is_broadcast) || is_broadcast < 0)
		{
			return -__LINE__;
		}
		cfg.is_broadcast = (0 != is_broadcast) ? true : false;

		node_cfg.reward_list.push_back(cfg);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int MedalConfig::InitUpgradeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	std::string attr_str[] = { "maxhp","maxmp","gongji","fangyu","minjie","jingshen","huifu","bisha","bisha_dmg","jianren","fanji",
		"mingzhong","shanduo","def_zhongdu","def_shihua","def_hunshui","def_jiuzui","def_hunluan","mogong","mokang" };

	int next_grade = 0;
	while (dataElement)
	{
		MedalUpgradeCfg cfg;
		if (!GetSubNodeValue(dataElement, "rank", cfg.grade) || cfg.grade != next_grade++)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "rate", cfg.rate))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "color", cfg.color))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(dataElement, "hearsay", cfg.hearsay))
		{
			return -__LINE__;
		}

		for (int i = 0; i < ARRAY_LENGTH(cfg.add_rate); ++i)
		{
			std::string str = STRING_SPRINTF("level%d_add_rate", i);
			if (!GetSubNodeValue(dataElement, str, cfg.add_rate[i]) || cfg.add_rate[i] < 0)
			{
				return -__LINE__;
			}
		}

		for (int i = 0; i < ARRAY_LENGTH(attr_str); ++i)
		{
			int attr_index = GetAttrIndexByName(attr_str[i]);

			if (!GetSubNodeValue(dataElement, attr_str[i], cfg.attrs[attr_index]))
			{
				return -__LINE__;
			}
		}

		m_upgrade_cfg.push_back(cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int MedalConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	if (!GetSubNodeValue(dataElement, "rank_sp_num", m_other_cfg.rand_sp_id_grade))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(dataElement, "sp_group", m_other_cfg.rand_sp_id_group))
	{
		return -__LINE__;
	}

	if (!GetSubNodeValue(dataElement, "medal_exchange_item", m_other_cfg.medal_exchange_item) || NULL == ITEMPOOL->GetItem(m_other_cfg.medal_exchange_item))
	{
		return -__LINE__;
	}

	return 0;
}

int MedalConfig::InitTradeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	int last_rank = -1;
	while (dataElement)
	{
		int rank = 0;
		if (!GetSubNodeValue(dataElement, "rank", rank) || rank < 0 || rank >= MAX_MEDAL_MAX_GRADE || rank != last_rank + 1)
		{
			return -1;
		}
		last_rank = rank;

		MedalMarketCfg& cfg = m_medal_grade_trade_cfg[rank];
		if (!GetSubNodeValue(dataElement, "sellprice", cfg.sold_price_coin) || cfg.sold_price_coin <= 0)
		{
			return -2;
		}

		if (!GetSubNodeIntArrayValueBySeperator(dataElement, "min_price", cfg.trade_min_gold_price, '|') || cfg.trade_min_gold_price.empty())
		{
			return -3;
		}

		if (!GetSubNodeIntArrayValueBySeperator(dataElement, "max_price", cfg.trade_max_gold_price, '|') || cfg.trade_max_gold_price.size() != cfg.trade_min_gold_price.size())
		{
			return -4;
		}

		if (!GetSubNodeIntArrayValueBySeperator(dataElement, "recommended_price", cfg.trade_recommend_price, '|') || cfg.trade_recommend_price.size() != cfg.trade_min_gold_price.size())
		{
			return -5;
		}

		for (size_t i = 0; i < cfg.trade_min_gold_price.size() && i < cfg.trade_max_gold_price.size(); ++i)
		{
			if (cfg.trade_min_gold_price[i] > cfg.trade_max_gold_price[i])
			{
				return -6;
			}
		}

		for (size_t i = 0; i < cfg.trade_recommend_price.size() && i < cfg.trade_min_gold_price.size() && i < cfg.trade_max_gold_price.size(); ++i)
		{
			if (cfg.trade_recommend_price[i] < cfg.trade_min_gold_price[i] || cfg.trade_recommend_price[i] > cfg.trade_max_gold_price[i])
			{
				return -7;
			}
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int MedalConfig::InitSpecialExchangeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int sp_id = 0;
		if (!GetSubNodeValue(dataElement, "sp_id", sp_id) || m_special_medal_exchange_set.end() != m_special_medal_exchange_set.find(sp_id))
		{
			return -1;
		}

		const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(sp_id);
		if (spe_cfg == NULL)
		{	
			return -2;
		}

		m_special_medal_exchange_set.insert(sp_id);
		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

MedalItemAttrsCfg::MedalItemAttrsCfg()
{
	rate = 0;
}

MedalItemAttrsCfg::AttrItem::AttrItem()
{
	attr_type = 0;
	attr_min = 0;
	attr_max = 0;
}

MedalItemAttrGroupCfg::MedalItemAttrGroupCfg()
{
	rate_count = 0;
}

MedalSpGroupCfg::MedalSpGroupCfg()
{
}
