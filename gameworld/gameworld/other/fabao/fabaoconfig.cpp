#include "fabaoconfig.hpp"
#include "item/itempool.h"
#include "monster/monster_group_pool.hpp"
#include "item/itembase.h"

FaBaoConfig::FaBaoConfig()
{
}

FaBaoConfig::~FaBaoConfig()
{
	std::vector<FaBaoSecondBackCfg>().swap(m_back_item_vec);
}

bool FaBaoConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitFaBaoOtherCfg);
	LOAD_CONFIG("talisman_attribute", InitFaBaoInfoCfg);
	LOAD_CONFIG("talisman_upgrade", InitFaBaoLevelInfoCfg);
	LOAD_CONFIG("talisman_advance", InitFaBaoJinJieCfg);
	LOAD_CONFIG("talisman_upgrade_new", InitNewFaBaoLevelInfoCfg);
	LOAD_CONFIG("talisman_advance_new", InitNewFaBaoJinJieCfg);
	LOAD_CONFIG("talisman_lottery", InitFaBaoDrawCfg);
	LOAD_CONFIG("consume_reward", InitFaBaoDrawRewardCfg);
	LOAD_CONFIG("talisman_fetter", InitFaBaoJiBanCfg);
	LOAD_CONFIG("talisman_return", InitFaBaoSecondReturnCfg);
	LOAD_CONFIG("return_num", InitFaBaoSecondBackItemCfg);

	return true;
}

const FaBaoInfoCfg * FaBaoConfig::GetFaBaoInfoCfg(int fabao_index)
{
	std::map<int, FaBaoInfoCfg>::iterator it = m_fabao_cfg.find(fabao_index);
	if (it != m_fabao_cfg.end())
	{
		return &it->second;
	}
	return NULL;
}

const FaBaoLevelItemCfg * FaBaoConfig::GetFaBaoLevelItemCfg(int fabao_id, int fabao_level)
{
	FaBaoLevelCfg * cfg = MapValuesPtr(m_new_level_cfg, fabao_id);
	if (cfg == NULL)
	{
		return NULL;
	}

	return MapValuesPtr(cfg->level_item, fabao_level);
}

const FaBaoJinJieItemCfg * FaBaoConfig::GetFaBaoJinJieItemCfg(int fabao_id, int jin_jie_level)
{
	FaBaoJinJieCfg * cfg = MapValuesPtr(m_new_jin_jie_cfg, fabao_id);
	if (cfg == NULL)
	{
		return NULL;
	}

	return MapValuesPtr(cfg->level_item, jin_jie_level);
}

const FaBaoLevelItemCfg * FaBaoConfig::GetOldFaBaoLevelItemCfg(int fabao_id, int fabao_level)
{
	FaBaoLevelCfg * cfg = MapValuesPtr(m_level_cfg, fabao_id);
	if (cfg == NULL)
	{
		return NULL;
	}

	return MapValuesPtr(cfg->level_item, fabao_level);
}

const FaBaoJinJieItemCfg * FaBaoConfig::GetOldFaBaoJinJieItemCfg(int fabao_id, int jin_jie_level)
{
	FaBaoJinJieCfg * cfg = MapValuesPtr(m_jin_jie_cfg, fabao_id);
	if (cfg == NULL)
	{
		return NULL;
	}

	return MapValuesPtr(cfg->level_item, jin_jie_level);
}

const FaBaoJiBanGroupCfg * FaBaoConfig::GetFaBaoJiBanByGroupCfg(int group_id)
{
	std::map<int, FaBaoJiBanGroupCfg>::iterator it = m_fabao_jiban_cfg.find(group_id);
	if (it != m_fabao_jiban_cfg.end())
	{
		return &it->second;
	}

	return NULL;
}

const FaBaoJiBanGroupCfg * FaBaoConfig::GetFaBaoJiBanByIdCfg(int fabao_id)
{
	if (fabao_id < 0 || fabao_id >= MAX_FABAO_COUNT)return NULL;
	return fabao_jiban_by_fabao_id[fabao_id];
}

const FaBaoDrawCfg * FaBaoConfig::GetFaBaoDrawCfg(int item_id)
{
	return MapValuesPtr(m_draw_cfg, item_id);
}

const FaBaoDrawRewardItemCfg * FaBaoConfig::GetFaBaoDrawRewardCfg(int item_id, int reward_type)
{
	FaBaoDrawRewardCfg * cfg = MapValuesPtr(m_draw_reward_cfg, item_id);
	if (cfg == NULL)
	{
		return NULL;
	}

	std::map<int, int> ::iterator iter2 = cfg->rand_count.find(reward_type);
	if (iter2 == cfg->rand_count.end() || iter2->second <= 0)
	{
		return NULL;
	}

	std::map<int, std::vector<FaBaoDrawRewardItemCfg> >::iterator iter = cfg->rand_item.find(reward_type);
	if (iter == cfg->rand_item.end())
	{
		return NULL;
	}

	int r = RandomNum(iter2->second);
	std::vector<FaBaoDrawRewardItemCfg> & vec = iter->second;

	for (int i = 0; i < (int)vec.size(); ++i)
	{
		if (r < vec[i].rate)
		{
			return &vec[i];
		}

		r -= vec[i].rate;
	}

	return NULL;
}

const FaBaoSecondReturnBackListCfg * FaBaoConfig::GetUserFaBaoSecondRetrunCfg(int uid)
{
	std::map<int, FaBaoSecondReturnBackListCfg>::iterator it = m_back_user_vec.find(uid);
	if (m_back_user_vec.end() != it)
	{
		return &it->second;
	}
	return NULL;
}

const FaBaoSecondBackCfg * FaBaoConfig::GetFaBaoItemIdBackCfg(int item_id)
{
	if (item_id < 0 || item_id >= MAX_INT16)return NULL;
	return back_item_list[item_id];
}

int FaBaoConfig::InitFaBaoOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	if (!GetSubNodeValue(dataElement, "coin_exp", m_other_cfg.coin_exp) || m_other_cfg.coin_exp < 0)return -1;
	if (!GetSubNodeValue(dataElement, "xianqi_item", m_other_cfg.xianqi_item) || NULL == ITEMPOOL->GetItem(m_other_cfg.xianqi_item) )return -2;
	if (!GetSubNodeValue(dataElement, "item_exp", m_other_cfg.item_exp) || m_other_cfg.item_exp < 0)return -3;
	if (!GetSubNodeValue(dataElement, "talisman_monster", m_other_cfg.guide_monster_group) || NULL == MonsterGroupPool::GetInstance().GetMonsterGroupCfg(m_other_cfg.guide_monster_group)) return -5;
	if (!GetSubNodeValue(dataElement, "speed_up_item", m_other_cfg.speed_up_item)) return -6;
	if (!GetSubNodeValue(dataElement, "speed_up_time", m_other_cfg.speed_up_time)) return -7;

	for (int luzi_index = 1; luzi_index <= 3; ++luzi_index)
	{
		std::string str = STRING_SPRINTF("pre_condition_%d", luzi_index);
		int lun_hui_wang_chuang_seq = 0;
		if (!GetSubNodeValue(dataElement, str, lun_hui_wang_chuang_seq) )return -__LINE__;
		m_other_cfg.fa_bao_draw_active_index_to_seq[luzi_index] = lun_hui_wang_chuang_seq;
	}

	dataElement = dataElement->NextSiblingElement();

	return 0;
}

int FaBaoConfig::InitFaBaoInfoCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	while (dataElement != NULL)
	{
		int id = 0;
		if (!GetSubNodeValue(dataElement, "id", id) || id < 0)return -1;
		if (m_fabao_cfg.find(id) != m_fabao_cfg.end())return -2;
		FaBaoInfoCfg & cfg = m_fabao_cfg[id];
		cfg.fabao_id = id;
		if (!GetSubNodeValue(dataElement, "type", cfg.fabao_type) || cfg.fabao_type < 0)return -3;
		if (!GetSubNodeValue(dataElement, "effect", cfg.effect) || cfg.effect < 0)return -4;

		switch (cfg.fabao_type)
		{
		case FABAO_TYPE_ACTIVE:
		{
			if(cfg.effect <= BATTLE_FABAO_SKILL_TYPE_INVALID || cfg.effect >= BATTLE_FABAO_SKILL_TYPE_MAX) return -30;
		}
		break;
		case FABAO_TYPE_PASSIVE:
		{
			if(cfg.effect < BATTLE_ATTR_MIN || cfg.effect >= BATTLE_ATTR_MAX) return -31;
		}
		break;
		case FABAO_TYPE_BATTLE_PASSIVE:
			{
				if (cfg.effect <= BATTLE_FABAO_SKILL_TYPE_INVALID || cfg.effect >= BATTLE_FABAO_SKILL_TYPE_MAX) return -32;
			}
			break;
		default:
			return -3200;
		}

		if (!GetSubNodeValue(dataElement, "target", cfg.target) || cfg.target < 0) return -5;
		if (!GetSubNodeValue(dataElement, "max_advanced_addition", cfg.max_advanced_addition)) return -9;
		if (!GetSubNodeValue(dataElement, "max_advanced_addition2", cfg.max_advanced_addition_2)) return -99;
		if (!GetSubNodeValue(dataElement, "talisman_fragment_item", cfg.active_item_id)) return -10;
		if (!GetSubNodeValue(dataElement, "compose_item_num", cfg.active_item_num)) return -11;
		if (!GetSubNodeValue(dataElement, "max_advanced", cfg.max_jinjie_level)) return -12;

		int is_open = 0;
		if (!GetSubNodeValue(dataElement, "is_open", is_open) || is_open < 0 || is_open > 1) return -13;
		if (!GetSubNodeValue(dataElement, "color", cfg.color) || cfg.color <= ItemBase::I_COLOR_INVALID || cfg.color > ItemBase::I_COLOR_RED) return -14;

		
		cfg.is_open = is_open > 0;

		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int FaBaoConfig::InitFaBaoLevelInfoCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	while (dataElement != NULL)
	{
		int fabao_id = 0;
		if (!GetSubNodeValue(dataElement, "id", fabao_id) || fabao_id < 0 || fabao_id >= MAX_FABAO_COUNT)return -__LINE__;

		FaBaoLevelCfg & cfg = m_level_cfg[fabao_id];
		cfg.fabao_id = fabao_id;

		FaBaoLevelItemCfg item_cfg;
		if (!GetSubNodeValue(dataElement, "level", item_cfg.level))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "need_advanced_level", item_cfg.need_advanced_level))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "consume_item", item_cfg.consume_item))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "consume_num", item_cfg.consume_num) )return -__LINE__;
		if (!GetSubNodeValue(dataElement, "consume_coin", item_cfg.consume_coin))return -__LINE__;

		for (int i = 0; i < 2; ++i)
		{
			std::string attr_type = STRING_SPRINTF("attr_type_%d", i + 1);
			std::string attr_value = STRING_SPRINTF("attr_num_%d", i + 1);

			FaBaoLevelItemCfg::FaBaoAttrItem atte_item;
			if (!GetSubNodeValue(dataElement, attr_type, atte_item.attr_type))return -__LINE__;
			if (!GetSubNodeValue(dataElement, attr_value, atte_item.attr_value))return -__LINE__;

			item_cfg.attr_cfg.push_back(atte_item);
		}

		cfg.level_item[item_cfg.level] = item_cfg;
		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int FaBaoConfig::InitFaBaoJinJieCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	while (dataElement != NULL)
	{
		int fabao_id = 0;
		if (!GetSubNodeValue(dataElement, "id", fabao_id) || fabao_id < 0 || fabao_id >= MAX_FABAO_COUNT)return -__LINE__;

		FaBaoJinJieCfg & cfg = m_jin_jie_cfg[fabao_id];
		cfg.fabao_id = fabao_id;

		FaBaoJinJieItemCfg item_cfg;
		if (!GetSubNodeValue(dataElement, "advanced_level", item_cfg.jin_jie_level))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "is_percentage", item_cfg.is_percentage))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "advanced_addition", item_cfg.advanced_addition))return -__LINE__;
		//if (!GetSubNodeValue(dataElement, "advanced_addition2", item_cfg.advanced_addition_2))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "consume_coin", item_cfg.consume_coin))return -__LINE__;

		for (int i = 0; i < 2; ++i)
		{
			std::string item_id = STRING_SPRINTF("consume_item_%d", i + 1);
			std::string item_num = STRING_SPRINTF("consume_num_%d", i + 1);

			FaBaoJinJieItemCfg::FaBaoJinJieConsumeItem consume_item;
			if (!GetSubNodeValue(dataElement, item_id, consume_item.consume_item_id))return -__LINE__;
			if (!GetSubNodeValue(dataElement, item_num, consume_item.consume_item_num))return -__LINE__;

			item_cfg.consume_item.push_back(consume_item);
		}

		cfg.level_item[item_cfg.jin_jie_level] = item_cfg;
		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int FaBaoConfig::InitNewFaBaoLevelInfoCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	while (dataElement != NULL)
	{
		int fabao_id = 0;
		if (!GetSubNodeValue(dataElement, "id", fabao_id) || fabao_id < 0 || fabao_id >= MAX_FABAO_COUNT)return -__LINE__;

		FaBaoLevelCfg & cfg = m_new_level_cfg[fabao_id];
		cfg.fabao_id = fabao_id;

		FaBaoLevelItemCfg item_cfg;
		if (!GetSubNodeValue(dataElement, "level", item_cfg.level))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "need_advanced_level", item_cfg.need_advanced_level))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "consume_item", item_cfg.consume_item))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "consume_num", item_cfg.consume_num))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "consume_coin", item_cfg.consume_coin))return -__LINE__;

		for (int i = 0; i < 2; ++i)
		{
			std::string attr_type = STRING_SPRINTF("attr_type_%d", i + 1);
			std::string attr_value = STRING_SPRINTF("attr_num_%d", i + 1);

			FaBaoLevelItemCfg::FaBaoAttrItem atte_item;
			if (!GetSubNodeValue(dataElement, attr_type, atte_item.attr_type))return -__LINE__;
			if (!GetSubNodeValue(dataElement, attr_value, atte_item.attr_value))return -__LINE__;

			item_cfg.attr_cfg.push_back(atte_item);
		}

		cfg.level_item[item_cfg.level] = item_cfg;
		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int FaBaoConfig::InitNewFaBaoJinJieCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	while (dataElement != NULL)
	{
		int fabao_id = 0;
		if (!GetSubNodeValue(dataElement, "id", fabao_id) || fabao_id < 0 || fabao_id >= MAX_FABAO_COUNT)return -__LINE__;

		FaBaoJinJieCfg & cfg = m_new_jin_jie_cfg[fabao_id];
		cfg.fabao_id = fabao_id;

		FaBaoJinJieItemCfg item_cfg;
		if (!GetSubNodeValue(dataElement, "advanced_level", item_cfg.jin_jie_level))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "is_percentage", item_cfg.is_percentage))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "advanced_addition", item_cfg.advanced_addition))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "advanced_addition2", item_cfg.advanced_addition_2))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "consume_coin", item_cfg.consume_coin))return -__LINE__;

		for (int i = 0; i < 2; ++i)
		{
			std::string item_id = STRING_SPRINTF("consume_item_%d", i + 1);
			std::string item_num = STRING_SPRINTF("consume_num_%d", i + 1);

			FaBaoJinJieItemCfg::FaBaoJinJieConsumeItem consume_item;
			if (!GetSubNodeValue(dataElement, item_id, consume_item.consume_item_id))return -__LINE__;
			if (!GetSubNodeValue(dataElement, item_num, consume_item.consume_item_num))return -__LINE__;

			item_cfg.consume_item.push_back(consume_item);
		}

		cfg.level_item[item_cfg.jin_jie_level] = item_cfg;
		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int FaBaoConfig::InitFaBaoDrawCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	while (dataElement != NULL)
	{
		FaBaoDrawCfg cfg;
		if (!GetSubNodeValue(dataElement, "store_item", cfg.item_id))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "consume_time", cfg.consume_time))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "probability_up", cfg.rate))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "transform_num", cfg.transform_price_num))return -__LINE__;

		m_draw_cfg[cfg.item_id] = cfg;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int FaBaoConfig::InitFaBaoDrawRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	while (dataElement != NULL)
	{
		
		int item_id = 0;
		if (!GetSubNodeValue(dataElement, "store_itme", item_id))return -__LINE__;

		FaBaoDrawRewardCfg & cfg=m_draw_reward_cfg[item_id];

		FaBaoDrawRewardItemCfg item_cfg;
		if (!GetSubNodeValue(dataElement, "reward_type", item_cfg.reward_type))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "reward_id", item_cfg.reward_id))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "reward_num", item_cfg.reward_num))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "rate", item_cfg.rate) || item_cfg.rate <= 0)return -__LINE__;

		cfg.rand_count[item_cfg.reward_type] += item_cfg.rate;
		cfg.rand_item[item_cfg.reward_type].push_back(item_cfg);
	
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int FaBaoConfig::InitFaBaoJiBanCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	memset(fabao_jiban_by_fabao_id, NULL, sizeof(fabao_jiban_by_fabao_id));
	while (dataElement != NULL)
	{
		int fetter_group = 0;
		if (!GetSubNodeValue(dataElement, "fetter_group", fetter_group))return -__LINE__;

		FaBaoJiBanGroupCfg & cfg = m_fabao_jiban_cfg[fetter_group];
		cfg.group = fetter_group;
		FaBaoJiBanCfg item_cfg;
		if (!GetSubNodeValue(dataElement, "fetter_seq", item_cfg.fetter_seq))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "id", item_cfg.id))return -__LINE__;

		cfg.m_jiban_vec.push_back(item_cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	for (std::map<int, FaBaoJiBanGroupCfg>::iterator it = m_fabao_jiban_cfg.begin(); it != m_fabao_jiban_cfg.end(); it++)
	{
		for(std::vector<FaBaoJiBanCfg>::iterator itor = it->second.m_jiban_vec.begin(); itor != it->second.m_jiban_vec.end(); itor++)
		{
			if (itor->id < 0 || itor->id >= MAX_FABAO_COUNT)continue;
			fabao_jiban_by_fabao_id[itor->id] = &it->second;
		}
	}

	return 0;
}

int FaBaoConfig::InitFaBaoSecondReturnCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");

	while (dataElement != NULL)
	{
		int uid = 0;
		if (!GetSubNodeValue(dataElement, "role_id", uid))return -__LINE__;

		FaBaoSecondReturnBackListCfg &node = m_back_user_vec[uid];

		FaBaoSecondReturnBackCfg node_cfg;
		node_cfg.uid = uid;
		if (!GetSubNodeValue(dataElement, "item_id", node_cfg.item_id) || NULL == ITEMPOOL->GetItem(node_cfg.item_id))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "item_num", node_cfg.item_num) || node_cfg.item_num < 0)return -__LINE__;
		if (!GetSubNodeValue(dataElement, "mail_serial_numbe", node_cfg.mail_id))return -__LINE__;

		node.m_user_back_vec.push_back(node_cfg);

		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int FaBaoConfig::InitFaBaoSecondBackItemCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	memset(back_item_list, NULL, sizeof(back_item_list));
	while (dataElement != NULL)
	{
		FaBaoSecondBackCfg node_cfg;
		if (!GetSubNodeValue(dataElement, "item_id", node_cfg.item_id) || NULL == ITEMPOOL->GetItem(node_cfg.item_id))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "present_price", node_cfg.present_price))return -__LINE__;
		if (!GetSubNodeValue(dataElement, "original_price", node_cfg.original_price))return -__LINE__;

		m_back_item_vec.push_back(node_cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	for (std::vector<FaBaoSecondBackCfg>::iterator it = m_back_item_vec.begin(); it != m_back_item_vec.end(); it++)
	{
		if (it->item_id < 0 || it->item_id >= MAX_UINT16)continue;
		back_item_list[it->item_id] = &(*it);
	}

	return 0;
}

