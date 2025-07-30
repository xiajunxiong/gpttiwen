#include "petgodprintequipmentconfig.hpp"
#include "item/itempool.h"
#include "item/itembase.h"

PetGodPrintEquipMentConfig::PetGodPrintEquipMentConfig()
{
	m_suit_map.clear();
	m_base_attr_map.clear();
	m_deputy_attr_base_list.clear();
	m_main_attr_map.clear();
}

PetGodPrintEquipMentConfig::~PetGodPrintEquipMentConfig()
{
	std::vector<GodPrintItemToExpenseItem>().swap(m_exchange_item_id_vec);
	std::vector<GodPrintIntensifyItemCfg>().swap(m_god_print_intensify_vec);
}

bool PetGodPrintEquipMentConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("pet_god_print_cfg", InitBaseCfg);
	LOAD_CONFIG("pet_god_suit", InitSuitCfg);
	LOAD_CONFIG("deputy_attribute_num", InitDeputyAttrNumGroupCfg);
	LOAD_CONFIG("deputy_attribute", InitDeputyAttrValueCfg);
	LOAD_CONFIG("att_group", InitMainAttrBaseGroupCfg);
	LOAD_CONFIG("intensify_att", InitAttrIntensiftCfg);
	LOAD_CONFIG("pet_god_intensify", InitAttrIntensiftInfoCfg);
	LOAD_CONFIG("resolve_exp", InitResolveExpCfg);
	LOAD_CONFIG("pray", InitWishCfg);
	LOAD_CONFIG("pray_item_group", InitWishGroupCfg);
	LOAD_CONFIG("exchange_god_print", InitWishExchangeCfg);
	LOAD_CONFIG("exchange_group", InitWishExchangeGroupCfg);
	LOAD_CONFIG("pet_god_expense", InitGodPrintIdExchangeExpenseId);
	LOAD_CONFIG("pet_god_intensify_item", InitGodPrintIntensifyItemCfg);

	return true;
}

const PetGodPrintEquipMentBaseQualityCfg * PetGodPrintEquipMentConfig::GetBaseCfg(int seq, int quality)
{
	if (seq <= 0 || seq > GOD_PRINT_INDEX_TYPE_MAX)return NULL;
	if (quality <= 0 || quality >= ItemBase::I_COLOR_MAX)return NULL;

	return &m_base_cfg[seq][quality];
}

const PetGodPrintEquipMentSuitCfg * PetGodPrintEquipMentConfig::GetSuitListCfg(int suit_id, int star_level, int suit_num)
{
	std::map<int, PetGodPrintEquipMentSuitListListCfg>::iterator it = m_suit_map.find(suit_id);
	if (it != m_suit_map.end())
	{
		if (star_level > ItemBase::I_COLOR_INVALID && star_level <= ItemBase::I_COLOR_MAX)
		{
			PetGodPrintEquipMentSuitListCfg &node = it->second.suilt_list[star_level];
			if (suit_num >= 0 && suit_num < GOD_PRINT_INDEX_TYPE_MAX)
			{
				return &node.suit_list[suit_num];
			}
		}
	}
	return NULL;
}

const PetGodPrintEquipMentBaseAttrNumCfg * PetGodPrintEquipMentConfig::RandDeputyAttrNum(int num_group)
{
	std::map<int, PetGodPrintEquipMentBaseAttrNumGroupCfg>::iterator it = m_base_attr_map.find(num_group);
	if (it != m_base_attr_map.end())
	{
		if (it->second.weight > 0)
		{
			int rand_num = rand() % it->second.weight;
			for (std::vector<PetGodPrintEquipMentBaseAttrNumCfg>::iterator itor = it->second.m_attr_vec.begin(); itor != it->second.m_attr_vec.end(); itor++)
			{
				if (itor->rate >= rand_num)
				{
					return &(*itor);
				}
				rand_num -= itor->rate;
			}
		}
	}
	return NULL;
}

bool PetGodPrintEquipMentConfig::GetMaxDeputyAttrNum(int num_group, int* attr_max, int* attr_min)
{
	int min_attr_num = 0;
	int max_attr_num = 0;
	bool ret = false;
	std::map<int, PetGodPrintEquipMentBaseAttrNumGroupCfg>::iterator it = m_base_attr_map.find(num_group);
	if (it != m_base_attr_map.end())
	{
		for (std::vector<PetGodPrintEquipMentBaseAttrNumCfg>::iterator itor = it->second.m_attr_vec.begin(); itor != it->second.m_attr_vec.end(); itor++)
		{
			if (max_attr_num == 0 || max_attr_num < itor->num)
			{
				max_attr_num = itor->num;
				ret = true;
			}

			if (min_attr_num == 0 || min_attr_num > itor->num)
			{
				min_attr_num = itor->num;
				ret = true;
			}
		}
	}
	if (NULL != attr_min)*attr_min = min_attr_num;
	if (NULL != attr_max)*attr_max = max_attr_num;
	return ret;
}

const PetGodPrintdeputyAttrInfoCfg * PetGodPrintEquipMentConfig::RandDeputyAttrBaseInfo(int seq, int attr_list[MAX_PET_GOD_PRONT_ATTR_NUM])
{
	std::map<int, PetGodPrintdeputyAttrListCfg>::iterator it = m_deputy_attr_base_list.find(seq);
	if (it != m_deputy_attr_base_list.end())
	{
		if (it->second.m_attr_vec.size() <= 0)return NULL;
		std::vector<PetGodPrintdeputyAttrInfoCfg*> real_attr_vec;
		for (std::vector<PetGodPrintdeputyAttrInfoCfg>::iterator it2 = it->second.m_attr_vec.begin(); it2 != it->second.m_attr_vec.end(); it2++)
		{
			if (attr_list[0] >= 0 && attr_list[0] == it2->att_type)continue;
			if (attr_list[1] >= 0 && attr_list[1] == it2->att_type)continue;
			if (attr_list[2] >= 0 && attr_list[2] == it2->att_type)continue;
			if (attr_list[3] >= 0 && attr_list[3] == it2->att_type)continue;
			real_attr_vec.push_back(&(*it2));
		}

		if (real_attr_vec.size() > 0)
		{
			int rand_idx = rand() % real_attr_vec.size();
			return real_attr_vec[rand_idx];
		}
	}
	return NULL;
}

const PetGodPrintdeputyAttrInfoCfg * PetGodPrintEquipMentConfig::GetDeputyAttrBaseInfo(int seq, int attr_type)
{
	std::map<int, PetGodPrintdeputyAttrListCfg>::iterator it = m_deputy_attr_base_list.find(seq);
	if (it != m_deputy_attr_base_list.end())
	{
		if (it->second.m_attr_vec.size() <= 0)return NULL;
		for (std::vector<PetGodPrintdeputyAttrInfoCfg>::iterator it2 = it->second.m_attr_vec.begin(); it2 != it->second.m_attr_vec.end(); it2++)
		{
			if (it2->att_type == attr_type)
			{
				return &(*it2);
			}
		}
	}
	return NULL;
}

const PetGodPrintMainAttrBaseCfg * PetGodPrintEquipMentConfig::RandMainBaseAttrCfg(int group_id)
{
	std::map<int, PetGodPrintMainAttrBaseGroupCfg>::iterator it = m_main_attr_map.find(group_id);
	if (it != m_main_attr_map.end())
	{
		if (it->second.weight > 0)
		{
			int rand_num = rand() % it->second.weight;
			for (std::vector<PetGodPrintMainAttrBaseCfg>::iterator itor = it->second.m_main_attr_vec.begin(); itor != it->second.m_main_attr_vec.end(); itor++)
			{
				if (itor->rate_10000 >= rand_num)
				{
					return &(*itor);
				}
				rand_num -= itor->rate_10000;
			}
		}
	}
	return NULL;
}

const PetGodPrintIntensifyDetailCfg * PetGodPrintEquipMentConfig::GetIntensifyCfg(int god_print_type, int quality, int attr_type)
{
	if (god_print_type < PetGodPrintIntensifyDetailCfg::PET_GOD_PRINT_TYPE_MAIN || god_print_type > PetGodPrintIntensifyDetailCfg::PET_GOD_PRINT_TYPE_DEPUTY)return NULL;
	if (quality < ItemBase::I_COLOR_INVALID || quality >= ItemBase::I_COLOR_MAX)return NULL;
	if (attr_type < 0 || attr_type >= BATTLE_ATTR_MAX)return NULL;

	return &m_intensify_cfg[god_print_type - 1][quality][attr_type];
}

const PetGodPrintIntensifyInfoCfg * PetGodPrintEquipMentConfig::GetIntensifyInfoCfg(int quality, int level)
{
	if (quality < ItemBase::I_COLOR_INVALID || quality >= ItemBase::I_COLOR_MAX)return NULL;
	for (std::vector<PetGodPrintIntensifyInfoCfg>::iterator it = m_intensify_info_cfg[quality].m_intensify_vec.begin(); it != m_intensify_info_cfg[quality].m_intensify_vec.end(); it++)
	{
		if (it->level == level)
		{
			return &(*it);
		}
	}
	return NULL;
}

const PetGodPrintResolveExpCfg * PetGodPrintEquipMentConfig::GetResolveExpCfg(int quality)
{
	if(quality < 0 || quality >= ItemBase::I_COLOR_MAX)return NULL;

	return &m_resolve_exp[quality];
}

const PetGodPrintWishCfg * PetGodPrintEquipMentConfig::GetWishCfg(int seq)
{
	std::map<int, PetGodPrintWishCfg>::iterator it = m_wish_cfg.find(seq);
	if (it != m_wish_cfg.end())
	{
		return &it->second;
	}
	return NULL;
}

const PetGodPrintWishRewardCfg * PetGodPrintEquipMentConfig::RandRewardByGroup(int group_id)
{
	std::map<int, PetGodPrintWishRewardGroupCfg>::iterator it = m_wish_group_map.find(group_id);
	if (it != m_wish_group_map.end())
	{
		int rate_num = 1;
		if (it->second.weight > 0)rate_num = rand() % it->second.weight;
		for (std::vector<PetGodPrintWishRewardCfg>::iterator it2 = it->second.m_group_vec.begin(); it2 != it->second.m_group_vec.end(); it2++)
		{
			if (it2->rate_10000 >= rate_num)
			{
				return &(*it2);
			}
			rate_num -= it2->rate_10000;
		}
	}
	return NULL;
}

const PetGodPrintWishExchangeCfg * PetGodPrintEquipMentConfig::GetExchangeSeq(int seq)
{
	std::map<int, PetGodPrintWishExchangeCfg>::iterator it = m_wish_exchange_map.find(seq);
	if (it != m_wish_exchange_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const PetGodPrintWishExchangeItemCfg * PetGodPrintEquipMentConfig::RandExchangeReward(int group_id)
{
	std::map<int, PetGodPrintWishExchangeGroupCfg>::iterator it = m_wish_exchange_group_map.find(group_id);
	if (it != m_wish_exchange_group_map.end())
	{
		int rate_num = 1;
		if (it->second.weight > 0)rate_num = rand() % it->second.weight;
		
		for (std::vector<PetGodPrintWishExchangeItemCfg>::iterator it2 = it->second.m_group_vec.begin(); it2 != it->second.m_group_vec.end(); it2++)
		{
			if (rate_num <= it2->rate_10000)
			{
				return &(*it2);
			}
			rate_num -= it2->rate_10000;
		}
	}
	return NULL;
}

const int * PetGodPrintEquipMentConfig::GetExpenseIdByGodPrintId(int item_id)
{
	if (item_id >= 0 && item_id < MAX_UINT16)
	{
		return m_god_print_item_id[item_id];
	}
	return NULL;
}

const int * PetGodPrintEquipMentConfig::GetGodPrintIdByExpenseId(int item_id)
{
	if (item_id >= 0 && item_id < MAX_UINT16)
	{
		return m_expense_item_id[item_id];
	}
	return NULL;
}

const GodPrintIntensifyItemCfg * PetGodPrintEquipMentConfig::GetGodPrintIntensifyItem(int item_id)
{
	for (std::vector<GodPrintIntensifyItemCfg>::iterator it = m_god_print_intensify_vec.begin(); it != m_god_print_intensify_vec.end(); it++)
	{
		if (it->intensify_item_id == item_id)
		{
			return &(*it);
		}
	}
	return NULL;
}

int PetGodPrintEquipMentConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "exp_attenuation_100", m_other_cfg.exp_attenuation_100) || m_other_cfg.exp_attenuation_100 < 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "need_coin_100_exp", m_other_cfg.need_coin_100_exp) || m_other_cfg.need_coin_100_exp < 0)
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "exchange_item_id", m_other_cfg.exchange_item_id) || NULL == ITEMPOOL->GetItem(m_other_cfg.exchange_item_id))
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "free_num", m_other_cfg.free_num) || m_other_cfg.free_num < 0)
	{
		return -4;
	}

	if (!GetSubNodeValue(root_element, "exchange_get_item_id", m_other_cfg.exchange_get_item_id) || NULL == ITEMPOOL->GetItem(m_other_cfg.exchange_get_item_id))
	{
		return -5;
	}

	if (!GetSubNodeValue(root_element, "exchange_get_item_id_1", m_other_cfg.exchange_get_item_id_1) || NULL == ITEMPOOL->GetItem(m_other_cfg.exchange_get_item_id_1))
	{
		return -6;
	}

	if (!GetSubNodeValue(root_element, "swallow_gold_cost", m_other_cfg.swallow_gold_cost))
	{
		return -7;
	}

	if (!GetSubNodeValue(root_element, "pray_cost_item", m_other_cfg.pray_cost_item) || NULL == ITEMPOOL->GetItem(m_other_cfg.pray_cost_item))
	{
		return -8;
	}
	
	return 0;
}

int PetGodPrintEquipMentConfig::InitBaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (root_element != NULL)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq <= 0 || seq > GOD_PRINT_INDEX_TYPE_MAX)
		{
			return -1;
		}
		int quality = 0;
		if (!GetSubNodeValue(root_element, "quality", quality) || quality <= 0 || quality >= ItemBase::I_COLOR_MAX)
		{
			return -2;
		}

		PetGodPrintEquipMentBaseQualityCfg &node_cfg = m_base_cfg[seq][quality];

		if (!GetSubNodeValue(root_element, "main_attribute_group", node_cfg.main_attribute_group) || node_cfg.main_attribute_group < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "deputy_attribute_num", node_cfg.deputy_attribute_num) || node_cfg.deputy_attribute_num < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "deputy_attribute_att", node_cfg.deputy_attribute_att) || node_cfg.deputy_attribute_att < 0)
		{
			return -5;
		}

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PetGodPrintEquipMentConfig::InitSuitCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (root_element != NULL)
	{
		int pet_god_suit = 0;
		if (!GetSubNodeValue(root_element, "pet_god_suit", pet_god_suit) || pet_god_suit < 0)
		{
			return -1;
		}
		PetGodPrintEquipMentSuitListListCfg & node = m_suit_map[pet_god_suit];

		
		int suit_num = 0;
		if (!GetSubNodeValue(root_element, "suit_num", suit_num) || suit_num <= 0 || suit_num > GOD_PRINT_INDEX_TYPE_MAX)
		{
			return -2;
		}

		int star_level = 0;
		if (!GetSubNodeValue(root_element, "star_level", star_level) || star_level <= ItemBase::I_COLOR_INVALID || star_level >= ItemBase::I_COLOR_MAX)
		{
			return -22;
		}

		PetGodPrintEquipMentSuitListCfg &list_node_cfg = node.suilt_list[star_level];

		PetGodPrintEquipMentSuitCfg &node_cfg = list_node_cfg.suit_list[suit_num - 1];

		if (!GetSubNodeValue(root_element, "effect_type", node_cfg.effect_type) ||
			node_cfg.effect_type < PetGodPrintEquipMentSuitCfg::EFFECT_TYPE_NO ||
			node_cfg.effect_type >= PetGodPrintEquipMentSuitCfg::EFFECT_TYPE_MAX)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "param1", node_cfg.param1) || node_cfg.param1 < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "param2", node_cfg.param2) || node_cfg.param2 < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "score", node_cfg.score) || node_cfg.score < 0)
		{
			return -6;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetGodPrintEquipMentConfig::InitDeputyAttrNumGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;
	m_base_attr_map.clear();
	while (root_element != NULL)
	{
		int num_group = 0;
		if (!GetSubNodeValue(root_element, "num_group", num_group) || num_group < 0)
		{
			return -1;
		}
		PetGodPrintEquipMentBaseAttrNumGroupCfg & node = m_base_attr_map[num_group];

		PetGodPrintEquipMentBaseAttrNumCfg node_cfg;

		if (!GetSubNodeValue(root_element, "num", node_cfg.num) || node_cfg.num < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "rate", node_cfg.rate) || node_cfg.rate < 0)
		{
			return -3;
		}

		node.weight += node_cfg.rate;
		node.m_attr_vec.push_back(node_cfg);
		
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetGodPrintEquipMentConfig::InitDeputyAttrValueCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (root_element != NULL)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq < 0 || seq >= ItemBase::I_COLOR_MAX)
		{
			return -1;
		}

		int att_type = 0;
		if (!GetSubNodeValue(root_element, "att_type", att_type) || att_type < 0 || att_type >= BATTLE_ATTR_MAX)
		{
			return -1;
		}
		PetGodPrintdeputyAttrListCfg & node = m_deputy_attr_base_list[seq];
		PetGodPrintdeputyAttrInfoCfg node_cfg;
		node_cfg.att_type = att_type;
		if (!GetSubNodeValue(root_element, "att_min", node_cfg.att_min) || node_cfg.att_min < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "att_max", node_cfg.att_max) || node_cfg.att_max < 0)
		{
			return -3;
		}
		node.m_attr_vec.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetGodPrintEquipMentConfig::InitMainAttrBaseGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (root_element != NULL)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq < 0)
		{
			return -1;
		}
		PetGodPrintMainAttrBaseGroupCfg & node = m_main_attr_map[seq];

		PetGodPrintMainAttrBaseCfg node_cfg;

		if (!GetSubNodeValue(root_element, "att_type", node_cfg.att_type) || node_cfg.att_type < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "att_min", node_cfg.att_min) || node_cfg.att_min < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "att_max", node_cfg.att_max) || node_cfg.att_max < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "rate_10000", node_cfg.rate_10000) || node_cfg.rate_10000 < 0)
		{
			return -5;
		}

		node.weight += node_cfg.rate_10000;
		node.m_main_attr_vec.push_back(node_cfg);

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetGodPrintEquipMentConfig::InitAttrIntensiftCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (root_element != NULL)
	{
		int main_deputy = 0;
		if (!GetSubNodeValue(root_element, "main_deputy", main_deputy) || main_deputy < PetGodPrintIntensifyDetailCfg::PET_GOD_PRINT_TYPE_MAIN || main_deputy > PetGodPrintIntensifyDetailCfg::PET_GOD_PRINT_TYPE_DEPUTY)
		{
			return -1;
		}

		int quality = 0;
		if (!GetSubNodeValue(root_element, "quality", quality) || quality < 0 || quality >= ItemBase::I_COLOR_MAX)
		{
			return -2;
		}

		int att_type = 0;
		if (!GetSubNodeValue(root_element, "att_type", att_type) || att_type < 0 || att_type >= BATTLE_ATTR_MAX)
		{
			return -3;
		}

		PetGodPrintIntensifyDetailCfg &node = m_intensify_cfg[main_deputy - 1][quality][att_type];

		if (!GetSubNodeValue(root_element, "intensify_att_min", node.intensify_att_min) || node.intensify_att_min < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "intensify_att_max", node.intensify_att_max) || node.intensify_att_max < 0)
		{
			return -5;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetGodPrintEquipMentConfig::InitAttrIntensiftInfoCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (root_element != NULL)
	{
		int quality = 0;
		if (!GetSubNodeValue(root_element, "quality", quality) || quality < 0 || quality >= ItemBase::I_COLOR_MAX)
		{
			return -1;
		}

		PetGodPrintIntensifyLevelCfg &node = m_intensify_info_cfg[quality];
		PetGodPrintIntensifyInfoCfg node_cfg;

		if (!GetSubNodeValue(root_element, "levels", node_cfg.level) || node_cfg.level < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "exp", node_cfg.exp) || node_cfg.exp < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "main_attribute_up", node_cfg.main_attribute_up) || node_cfg.main_attribute_up < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "deputy_attribute_up", node_cfg.deputy_attribute_up) || node_cfg.deputy_attribute_up < 0)
		{
			return -5;
		}
		node.m_intensify_vec.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetGodPrintEquipMentConfig::InitResolveExpCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (root_element != NULL)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq < 0 || seq >= ItemBase::I_COLOR_MAX)
		{
			return -1;
		}

		PetGodPrintResolveExpCfg &node = m_resolve_exp[seq];

		if (!GetSubNodeValue(root_element, "item_exp", node.item_exp) || node.item_exp < 0)
		{
			return -2;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetGodPrintEquipMentConfig::InitWishCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (root_element != NULL)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "pray_seq", seq) || seq < 0)
		{
			return -1;
		}

		PetGodPrintWishCfg &node = m_wish_cfg[seq];
		node.pray_seq = seq;
		if (!GetSubNodeValue(root_element, "item_group", node.item_group) || node.item_group < 0)
		{
			return -2;
		}
		if (!GetSubNodeValue(root_element, "expend_coin", node.expend_coin) || node.expend_coin < 0)
		{
			return -3;
		}
		if (!GetSubNodeValue(root_element, "rate_10000", node.rate_10000) || node.rate_10000 < 0)
		{
			return -4;
		}
		if (!GetSubNodeValue(root_element, "expend_gold", node.expend_gold) || node.expend_gold < 0)
		{
			return -5;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetGodPrintEquipMentConfig::InitWishGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (root_element != NULL)
	{
		int item_group = 0;
		if (!GetSubNodeValue(root_element, "item_group", item_group) || item_group < 0)
		{
			return -1;
		}

		PetGodPrintWishRewardGroupCfg &group = m_wish_group_map[item_group];
		PetGodPrintWishRewardCfg node;
		node.item_group = item_group;
		if (!GetSubNodeValue(root_element, "rate_10000", node.rate_10000) || node.rate_10000 < 0)
		{
			return -2;
		}
		
		if (!GetSubNodeValue(root_element, "god_print_item", node.god_print_item) || NULL == ITEMPOOL->GetItem(node.god_print_item))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "item_seq", node.item_seq) || node.item_seq < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "is_broadcast", node.is_broadcast) || node.is_broadcast < 0)
		{
			return -5;
		}

		TiXmlElement * item_list_element = root_element->FirstChildElement("item_list_list");
		if (NULL != item_list_element)
		{
			TiXmlElement * item_element = item_list_element->FirstChildElement("item_list");
			while (NULL != item_element)
			{
				ItemConfigData reward_node;
				if (!reward_node.ReadConfig(item_element))
				{
					return -6;
				}
				node.m_reward_vec.push_back(reward_node);
				item_element = item_element->NextSiblingElement();
			}
		}
		
		group.weight += node.rate_10000;
		group.m_group_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetGodPrintEquipMentConfig::InitWishExchangeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (root_element != NULL)
	{
		int exchange_seq = 0;
		if (!GetSubNodeValue(root_element, "exchange_seq", exchange_seq) || exchange_seq < 0)
		{
			return -1;
		}

		PetGodPrintWishExchangeCfg &group = m_wish_exchange_map[exchange_seq];
		group.exchange_seq = exchange_seq;
		if (!GetSubNodeValue(root_element, "expend", group.expend) || group.expend < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "exchange_group", group.exchange_group))
		{
			return -3;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetGodPrintEquipMentConfig::InitWishExchangeGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (root_element != NULL)
	{
		int exchange_group_seq = 0;
		if (!GetSubNodeValue(root_element, "exchange_group_seq", exchange_group_seq) || exchange_group_seq < 0)
		{
			return -1;
		}

		PetGodPrintWishExchangeGroupCfg &group = m_wish_exchange_group_map[exchange_group_seq];
		PetGodPrintWishExchangeItemCfg node;
		if (!GetSubNodeValue(root_element, "rate_10000", node.rate_10000) || node.rate_10000 < 0)
		{
			return -2;
		}
		if (!GetSubNodeValue(root_element, "is_broadcast", node.is_broadcast))
		{
			return -3;
		}
		group.weight += node.rate_10000;
		
		TiXmlElement * item_list_element = root_element->FirstChildElement("item_list_list");
		if (NULL != item_list_element)
		{
			TiXmlElement * item_element = item_list_element->FirstChildElement("item_list");
			while (NULL != item_element)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(item_element))
				{
					return -3;
				}
				node.reward_vec.push_back(reward);
				item_element = item_element->NextSiblingElement();
			}
		}
		group.m_group_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetGodPrintEquipMentConfig::InitGodPrintIdExchangeExpenseId(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (root_element != NULL)
	{
		GodPrintItemToExpenseItem node;
		
		if (!GetSubNodeValue(root_element, "pet_god_item_id", node.pet_god_item_id) || node.pet_god_item_id < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "expense_item_id", node.expense_item_id) || node.expense_item_id < 0)
		{
			return -2;
		}
		m_exchange_item_id_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}
	memset(m_god_print_item_id, NULL, sizeof(m_god_print_item_id));
	memset(m_expense_item_id, NULL, sizeof(m_expense_item_id));
	for (std::vector<GodPrintItemToExpenseItem>::iterator it = m_exchange_item_id_vec.begin(); it != m_exchange_item_id_vec.end(); it++)
	{
		if ((it->expense_item_id >= 0 || it->expense_item_id < MAX_UINT16) && (it->pet_god_item_id >= 0 || it->pet_god_item_id < MAX_UINT16))
		{
			m_expense_item_id[it->expense_item_id] = &it->pet_god_item_id;
			m_god_print_item_id[it->pet_god_item_id] = &it->expense_item_id;
		}
	}

	return 0;
}

int PetGodPrintEquipMentConfig::InitGodPrintIntensifyItemCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (root_element != NULL)
	{
		GodPrintIntensifyItemCfg node;

		if (!GetSubNodeValue(root_element, "intensify_item_id", node.intensify_item_id) || NULL == ITEMPOOL->GetItem(node.intensify_item_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "intensify_item_exp", node.intensify_item_exp) || node.intensify_item_exp < 0)
		{
			return -2;
		}
		m_god_print_intensify_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
