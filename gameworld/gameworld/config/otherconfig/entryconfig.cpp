#include "entryconfig.hpp"
#include "entryconfig.hpp"
#include "entryconfig.hpp"
#include "entryconfig.hpp"
#include "config/otherconfig/entryconfig.hpp"
#include "item/itempool.h"
#include "servercommon/salarydef.hpp"
#include "servercommon/struct/battle/battle_def.hpp"
#include "gameworld/equipment/equipment.h"

EntryConfig::EntryConfig()
{

}

EntryConfig::~EntryConfig()
{
	std::vector<QuenChingAddCfg>().swap(m_add_cfg_vec);
}

bool EntryConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherConfig);
	LOAD_CONFIG("num_group", InitNumGroupConfig);
	LOAD_CONFIG("rand_group", InitRandGroupConfig);
	LOAD_CONFIG("att_value_1", InitAttValueConfig);
	LOAD_CONFIG("att_value_2", InitAttValueConfig2);
	LOAD_CONFIG("att_value_3", InitAttValueConfig3);
	LOAD_CONFIG("att_value_4", InitAttValueConfig4);
	LOAD_CONFIG("max_num", InitMaxNumConfg);
	LOAD_CONFIG("lock_price", InitLockPriceCfg);
	LOAD_CONFIG("smelt_price", InitSmeltPriceCfg);
	LOAD_CONFIG("godequip_num", InitGodEquipCfg);
	LOAD_CONFIG("godequip_num_limit", InitGodEquipVocActiveCfg);
	LOAD_CONFIG("quenching_consume", InitGodEquipVocUpLevelExpendCfg);
	LOAD_CONFIG("equip_num_add", InitGodEquipVocUpLevelAddCfg);

	return true;
}

int EntryConfig::GetEntryNumConfig(int group_id)
{
	std::map<int, NumGroupListCfg>::iterator it = m_num_group_config.find(group_id);
	if (it != m_num_group_config.end() && it->second.weight_count > 0)
	{
		int rand_real = rand() % it->second.weight_count;
		for (std::vector<NumGroupCfg>::iterator it2 = it->second.mun_group_vec.begin(); it2 != it->second.mun_group_vec.end(); it2++)
		{
			if (rand_real < it2->rate)
			{
				return it2->num;
			}
			rand_real -= it2->rate;
		}
	}
	return NULL;
}

int EntryConfig::GetEntryRandConfig(int group_id, int entry_num, RandGroupCfg entry_type[EquipmentParam::EXA_ATTR_NUM_MAX], int entry_aw[EquipmentParam::EXA_ATTR_NUM_MAX]/*= NULL*/)
{
	int real_entry_num = 0;
	std::map<int, RandGroupListCfg>::iterator it = m_rand_group_config.find(group_id);
	if (it != m_rand_group_config.end())
	{
		std::vector<RandGroupCfg> node_vec;
		int weight = it->second.weight_count;
		node_vec.assign(it->second.rand_group_vec.begin(), it->second.rand_group_vec.end());

		if (entry_aw != NULL)
		{
			//循环词条删除已有词条
			for (int i = 0; i < entry_num && i < EquipmentParam::EXA_ATTR_NUM_MAX; i++)
			{
				for (std::vector<RandGroupCfg>::iterator it3 = node_vec.begin(); it3 != node_vec.end();)
				{
					if (it3->att_type == entry_aw[i])
					{
						weight -= it3->rate;
						it3 = node_vec.erase(it3);
					}
					else
					{
						it3++;
					}
				}
			}
		}

		for (int i = 0; i < entry_num && i < EquipmentParam::EXA_ATTR_NUM_MAX; i++)
		{
			if ((int)node_vec.size() <= 0 || weight <= 0)return real_entry_num;
			int rand_dis = rand() % weight;
			for (std::vector<RandGroupCfg>::iterator it2 = node_vec.begin(); it2 != node_vec.end(); it2++)
			{
				if (rand_dis < it2->rate)
				{
					real_entry_num++;
					//记录词条
					entry_type[i].att_type = it2->att_type;
					entry_type[i].quality = it2->quality;
					//循环词条删除同类型词条
					for (std::vector<RandGroupCfg>::iterator it3 = node_vec.begin(); it3 != node_vec.end();)
					{
						if (it3->att_type == entry_type[i].att_type)
						{
							weight -= it3->rate;
							it3 = node_vec.erase(it3);
						}
						else
						{
							it3++;
						}
					}
					//完成随机词条，跳出循环
					break;
				}
				rand_dis -= it2->rate;
			}
		}
	}
	return real_entry_num;
}

bool EntryConfig::EntryGroupHasAttrTypr(int group_id, int attr_type)
{
	std::map<int, RandGroupListCfg>::iterator it = m_rand_group_config.find(group_id);
	if (it != m_rand_group_config.end())
	{
		for (std::vector<RandGroupCfg>::iterator group_it = it->second.rand_group_vec.begin(); group_it != it->second.rand_group_vec.end(); group_it++)
		{
			if (group_it->att_type == attr_type)
			{
				return true;
			}
		}
	}
	return false;
}

const AttValueCfg  * EntryConfig::GetEntryAttrValueConfig(int level, int quality, int attr_type)
{
	int key = this->GetAttValueKeyByLevelAndQuality(level, quality);
	std::map<int, AttValueListCfg>::iterator it = m_attvalue_config.find(key);
	if (it != m_attvalue_config.end() && attr_type >= BATTLE_ATTR_MIN && attr_type < BATTLE_ATTR_MAX)
	{
		return &(it->second.m_att_list[attr_type]);
	}
	return NULL;
}

const AttValueCfg * EntryConfig::GetEntryAttrValueConfig2(int level, int quality, int attr_type)
{
	int key = this->GetAttValueKeyByLevelAndQuality(level, quality);
	std::map<int, AttValueListCfg>::iterator it = m_attvalue_config2.find(key);
	if (it != m_attvalue_config2.end() && attr_type >= BATTLE_ATTR_MIN && attr_type < BATTLE_ATTR_MAX)
	{
		return &(it->second.m_att_list[attr_type]);
	}
	return NULL;
}

const AttValueCfg * EntryConfig::GetEntryAttrValueConfig3(int level, int quality, int attr_type)
{
	int key = this->GetAttValueKeyByLevelAndQuality(level, quality);
	std::map<int, AttValueListCfg>::iterator it = m_attvalue_config3.find(key);
	if (it != m_attvalue_config3.end() && attr_type >= BATTLE_ATTR_MIN && attr_type < BATTLE_ATTR_MAX)
	{
		return &(it->second.m_att_list[attr_type]);
	}
	return NULL;
}

const AttValueCfg * EntryConfig::GetEntryAttrValueConfig4(int level, int quality, int attr_type)
{
	int key = this->GetAttValueKeyByLevelAndQuality(level, quality);
	std::map<int, AttValueListCfg>::iterator it = m_attvalue_config4.find(key);
	if (it != m_attvalue_config4.end() && attr_type >= BATTLE_ATTR_MIN && attr_type < BATTLE_ATTR_MAX)
	{
		return &(it->second.m_att_list[attr_type]);
	}
	return NULL;
}

const AttValueCfg * EntryConfig::GetEntryAttrValueConfigBack(int level, int attr_type, int attr_value)
{
	for (std::map<int, AttValueListCfg>::iterator it = m_attvalue_config.begin(); it != m_attvalue_config.end(); it++)
	{
		for (int i = 0; i < BATTLE_ATTR_MAX; i++)
		{
			if (it->second.m_att_list[i].level == level && it->second.m_att_list[i].att_type == attr_type)
			{
				if (it->second.m_att_list[i].min <= attr_value && it->second.m_att_list[i].max >= attr_value)
				{
					return &it->second.m_att_list[i];
				}
			}
		}
	}
	return NULL;
}

const AttValueCfg * EntryConfig::GetEntryAttrValueConfigBack2(int level, int attr_type, int attr_value)
{
	for (std::map<int, AttValueListCfg>::iterator it = m_attvalue_config2.begin(); it != m_attvalue_config2.end(); it++)
	{
		for (int i = 0; i < BATTLE_ATTR_MAX; i++)
		{
			if (it->second.m_att_list[i].level == level && it->second.m_att_list[i].att_type == attr_type)
			{
				if (it->second.m_att_list[i].min <= attr_value && it->second.m_att_list[i].max >= attr_value)
				{
					return &it->second.m_att_list[i];
				}
			}
		}
	}
	return NULL;
}

const AttValueCfg * EntryConfig::GetEntryAttrValueConfigBack3(int level, int attr_type, int attr_value)
{
	for (std::map<int, AttValueListCfg>::iterator it = m_attvalue_config3.begin(); it != m_attvalue_config3.end(); it++)
	{
		for (int i = 0; i < BATTLE_ATTR_MAX; i++)
		{
			if (it->second.m_att_list[i].level == level && it->second.m_att_list[i].att_type == attr_type)
			{
				if (it->second.m_att_list[i].min <= attr_value && it->second.m_att_list[i].max >= attr_value)
				{
					return &it->second.m_att_list[i];
				}
			}
		}
	}
	return NULL;
}

const AttValueCfg * EntryConfig::GetEntryAttrValueConfigBack4(int level, int attr_type, int attr_value)
{
	for (std::map<int, AttValueListCfg>::iterator it = m_attvalue_config4.begin(); it != m_attvalue_config4.end(); it++)
	{
		for (int i = 0; i < BATTLE_ATTR_MAX; i++)
		{
			if (it->second.m_att_list[i].level == level && it->second.m_att_list[i].att_type == attr_type)
			{
				if (it->second.m_att_list[i].min <= attr_value && it->second.m_att_list[i].max >= attr_value)
				{
					return &it->second.m_att_list[i];
				}
			}
		}
	}
	return NULL;
}

const AttValueCfg * EntryConfig::GetEntryAttrValueConfigByLevelAndTypeAndValue(int level, int attr_type, int attr_value)
{
	for (std::map<int, AttValueListCfg>::iterator it = m_attvalue_config.begin(); it != m_attvalue_config.end(); it++)
	{
		for (int i = 0; i < BATTLE_ATTR_MAX; i++)
		{
			if (it->second.m_att_list[i].level == level &&
				it->second.m_att_list[i].att_type == attr_type &&
				it->second.m_att_list[i].min <= attr_value &&
				it->second.m_att_list[i].max >= attr_value)
			{
				return &it->second.m_att_list[i];
			}
		}
	}
	return NULL;
}

const AttValueCfg * EntryConfig::GetEntryAttrValueConfig2ByLevelAndTypeAndValue(int level, int attr_type, int attr_value)
{
	for (std::map<int, AttValueListCfg>::iterator it = m_attvalue_config2.begin(); it != m_attvalue_config2.end(); it++)
	{
		for (int i = 0; i < BATTLE_ATTR_MAX; i++)
		{
			if (it->second.m_att_list[i].level == level &&
				it->second.m_att_list[i].att_type == attr_type &&
				it->second.m_att_list[i].min <= attr_value &&
				it->second.m_att_list[i].max >= attr_value)
			{
				return &it->second.m_att_list[i];
			}
		}
	}
	return NULL;
}

const AttValueCfg * EntryConfig::GetEntryAttrValueConfig3ByLevelAndTypeAndValue(int level, int attr_type, int attr_value)
{
	for (std::map<int, AttValueListCfg>::iterator it = m_attvalue_config3.begin(); it != m_attvalue_config3.end(); it++)
	{
		for (int i = 0; i < BATTLE_ATTR_MAX; i++)
		{
			if (it->second.m_att_list[i].level == level &&
				it->second.m_att_list[i].att_type == attr_type &&
				it->second.m_att_list[i].min <= attr_value &&
				it->second.m_att_list[i].max >= attr_value)
			{
				return &it->second.m_att_list[i];
			}
		}
	}
	return NULL;
}

const AttValueCfg * EntryConfig::GetEntryAttrValueConfig4ByLevelAndTypeAndValue(int level, int attr_type, int attr_value)
{
	for (std::map<int, AttValueListCfg>::iterator it = m_attvalue_config4.begin(); it != m_attvalue_config4.end(); it++)
	{
		for (int i = 0; i < BATTLE_ATTR_MAX; i++)
		{
			if (it->second.m_att_list[i].level == level &&
				it->second.m_att_list[i].att_type == attr_type &&
				it->second.m_att_list[i].min <= attr_value &&
				it->second.m_att_list[i].max >= attr_value)
			{
				return &it->second.m_att_list[i];
			}
		}
	}
	return NULL;
}

const MaxAttrNumCfg * EntryConfig::GetEntryAttrMaxNumConfig(int level, int quality)
{
	int key = this->GetAttValueKeyByLevelAndQuality(level, quality);
	std::map<int, MaxAttrNumCfg>::iterator it = m_max_attr_num_config.find(key);
	if (it != m_max_attr_num_config.end())
	{
		return &it->second;
	}
	return NULL;
}

const GodEquipAttrNumConfig * EntryConfig::GetGodEquipmentNumCfg(int strengthen_level, int quality)
{
	std::map<int, std::map<int, GodEquipAttrNumConfig> >::iterator quality_it = m_god_equip_cfg.find(quality);
	if (quality_it == m_god_equip_cfg.end())
	{
		return 0;
	}

	//配置读取的时候有做检测
	for (std::map<int, GodEquipAttrNumConfig>::reverse_iterator level_it = quality_it->second.rbegin(); level_it != quality_it->second.rend(); level_it++)
	{
		if (strengthen_level >= level_it->first)
		{
			return &level_it->second;
		}
	}

	return NULL;
}

const GodEquipActiveAttrNumConfig * EntryConfig::GetGodEquipmentAttrVocCfg(int quality, int voc_idx)
{
	if (quality < ItemBase::I_COLOR_INVALID || quality >= ItemBase::I_COLOR_MAX)return NULL;
	if (voc_idx < 0 || voc_idx >= EquipmentParam::EXA_ATTR_NUM_MAX || voc_idx >= JewelryParam::EXA_ATTR_NUM_MAX)return NULL;

	return &m_god_equip_voc_active_cfg[quality][voc_idx];
}

int EntryConfig::InitOtherConfig(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -100;

	if (!GetSubNodeValue(dataElement, "coin_smelt_times", m_other_cfg.coin_smelt_times) || m_other_cfg.coin_smelt_times <= 0)
	{
		return -1;
	}

	return 0;
}

int EntryConfig::InitNumGroupConfig(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		NumGroupCfg node;
		if (!GetSubNodeValue(dataElement, "num_group", node.num_group))
		{
			return -1;
		}
		NumGroupListCfg &node_cfg = m_num_group_config[node.num_group];

		if (!GetSubNodeValue(dataElement, "num", node.num))
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "rate", node.rate))
		{
			return -3;
		}
		node_cfg.weight_count += node.rate;
		node_cfg.mun_group_vec.push_back(node);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int EntryConfig::InitRandGroupConfig(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		RandGroupCfg node;
		if (!GetSubNodeValue(dataElement, "att_group", node.att_group))
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "att_type", node.att_type))
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "rate", node.rate))
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "quality", node.quality))
		{
			return -4;
		}
		RandGroupListCfg &node_cfg = m_rand_group_config[node.att_group];
		node_cfg.weight_count += node.rate;
		node_cfg.rand_group_vec.push_back(node);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int EntryConfig::InitAttValueConfig(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int level = 0;
		if (!GetSubNodeValue(dataElement, "level", level))
		{
			return -1;
		}

		int quality = 0;
		if (!GetSubNodeValue(dataElement, "quality", quality))
		{
			return -2;
		}
		int key = this->GetAttValueKeyByLevelAndQuality(level, quality);
		AttValueListCfg &key_node_cfg = m_attvalue_config[key];

		int att_type = 0;
		if (!GetSubNodeValue(dataElement, "att_type", att_type) || att_type < BATTLE_ATTR_MIN || att_type >= BATTLE_ATTR_MAX)
		{
			return -3;
		}
		AttValueCfg & node_cfg = key_node_cfg.m_att_list[att_type];
		node_cfg.level = level;
		node_cfg.quality = quality;;
		node_cfg.att_type = att_type;
		if (!GetSubNodeValue(dataElement, "max", node_cfg.max))
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "min", node_cfg.min))
		{
			return -5;
		}

		if (node_cfg.max < node_cfg.min)
		{
			return -6;
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int EntryConfig::InitAttValueConfig2(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int level = 0;
		if (!GetSubNodeValue(dataElement, "level", level))
		{
			return -1;
		}

		int quality = 0;
		if (!GetSubNodeValue(dataElement, "quality", quality))
		{
			return -2;
		}
		int key = this->GetAttValueKeyByLevelAndQuality(level, quality);
		AttValueListCfg &key_node_cfg = m_attvalue_config2[key];

		int att_type = 0;
		if (!GetSubNodeValue(dataElement, "att_type", att_type) || att_type < BATTLE_ATTR_MIN || att_type >= BATTLE_ATTR_MAX)
		{
			return -3;
		}
		AttValueCfg & node_cfg = key_node_cfg.m_att_list[att_type];
		node_cfg.level = level;
		node_cfg.quality = quality;;
		node_cfg.att_type = att_type;
		if (!GetSubNodeValue(dataElement, "max", node_cfg.max))
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "min", node_cfg.min))
		{
			return -5;
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int EntryConfig::InitAttValueConfig3(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;
	while (NULL != dataElement)
	{
		int level = 0;
		if (!GetSubNodeValue(dataElement, "level", level))
		{
			return -1;
		}

		int quality = 0;
		if (!GetSubNodeValue(dataElement, "quality", quality))
		{
			return -2;
		}
		int key = this->GetAttValueKeyByLevelAndQuality(level, quality);
		AttValueListCfg &key_node_cfg = m_attvalue_config3[key];

		int att_type = 0;
		if (!GetSubNodeValue(dataElement, "att_type", att_type) || att_type < BATTLE_ATTR_MIN || att_type >= BATTLE_ATTR_MAX)
		{
			return -3;
		}
		AttValueCfg & node_cfg = key_node_cfg.m_att_list[att_type];
		node_cfg.level = level;
		node_cfg.quality = quality;;
		node_cfg.att_type = att_type;
		if (!GetSubNodeValue(dataElement, "max", node_cfg.max))
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "min", node_cfg.min))
		{
			return -5;
		}

		if (node_cfg.max < node_cfg.min)
		{
			return -6;
		}


		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int EntryConfig::InitAttValueConfig4(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;
	while (NULL != dataElement)
	{
		int level = 0;
		if (!GetSubNodeValue(dataElement, "level", level))
		{
			return -1;
		}

		int quality = 0;
		if (!GetSubNodeValue(dataElement, "quality", quality))
		{
			return -2;
		}
		int key = this->GetAttValueKeyByLevelAndQuality(level, quality);
		AttValueListCfg &key_node_cfg = m_attvalue_config4[key];

		int att_type = 0;
		if (!GetSubNodeValue(dataElement, "att_type", att_type) || att_type < BATTLE_ATTR_MIN || att_type >= BATTLE_ATTR_MAX)
		{
			return -3;
		}
		AttValueCfg & node_cfg = key_node_cfg.m_att_list[att_type];
		node_cfg.level = level;
		node_cfg.quality = quality;;
		node_cfg.att_type = att_type;
		if (!GetSubNodeValue(dataElement, "max", node_cfg.max))
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "min", node_cfg.min))
		{
			return -5;
		}


		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int EntryConfig::InitMaxNumConfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int level = 0;
		if (!GetSubNodeValue(dataElement, "level", level))
		{
			return -1;
		}

		int quality = 0;
		if (!GetSubNodeValue(dataElement, "quality", quality))
		{
			return -2;
		}
		int key = this->GetAttValueKeyByLevelAndQuality(level, quality);
		MaxAttrNumCfg &key_node_cfg = m_max_attr_num_config[key];

		int max_num = 0;
		if (!GetSubNodeValue(dataElement, "max_num", max_num) || max_num < 0 || max_num > EquipmentParam::EXA_ATTR_NUM_MAX)
		{
			return -3;
		}
		key_node_cfg.level = level;
		key_node_cfg.quality = quality;
		key_node_cfg.max_num = max_num;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

const WorkshopLockPrice * EntryConfig::GetLockPriceCfg(int lock_num)
{
	if (lock_num < 0 || lock_num >= MAX_EQUIP_LOCK_NUM)return NULL;

	return &lock_list[lock_num];
}

const QuenChingExpendCfg * EntryConfig::GetQuenChingExpendCfg(int voc_color)
{
	if (m_god_equip_expend[voc_color].item_id > 0)
	{
		return &m_god_equip_expend[voc_color];
	}
	return NULL;
}

const QuenChingAddCfg * EntryConfig::GetQuenChingAddCfg(int voc_color, int voc_attr_type)
{
	if (NULL != m_god_equip_add_config[voc_color][voc_attr_type])
	{
		return m_god_equip_add_config[voc_color][voc_attr_type];
	}
	return NULL;
}

int EntryConfig::InitLockPriceCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int level = 0;
		if (!GetSubNodeValue(root_element, "level", level) || level < 0 || level >= 6)
		{
			return -1;
		}

		WorkshopLockPrice &info = lock_list[level];
		info.level = level;

		if (!GetSubNodeValue(root_element, "gold", info.gold) || info.gold < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "coin", info.coin) || info.coin < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "smelt_gold", info.ex_coin) || info.ex_coin < 0)
		{
			return -4;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int EntryConfig::InitSmeltPriceCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "smelt_price", smelt.smelt_price) || smelt.smelt_price < 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "price_type", smelt.price_type) || smelt.price_type < 0 || smelt.price_type >= CURRENCY_TYPE_MAX)
	{
		return -2;
	}

	return 0;
}

int EntryConfig::InitGodEquipCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int strengthen_level = 0;
		if (!GetSubNodeValue(dataElement, "godequip_level", strengthen_level))
		{
			return -1;
		}

		int quality = 0;
		if (!GetSubNodeValue(dataElement, "quality", quality) || quality < Equipment::I_COLOR_STAR || quality > Equipment::I_COLOR_GOD)
		{
			return -2;
		}
		GodEquipAttrNumConfig& node = m_god_equip_cfg[quality][strengthen_level];
		if (!GetSubNodeValue(dataElement, "max_num", node.max_num) || node.max_num < 0 || node.max_num > EquipmentParam::EXA_ATTR_NUM_MAX)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "role_level", node.role_level))
		{
			return -4;
		}

		dataElement = dataElement->NextSiblingElement();
	}

	for (std::map<int, std::map<int, GodEquipAttrNumConfig> >::iterator quality_it = m_god_equip_cfg.begin(); quality_it != m_god_equip_cfg.end(); quality_it++)
	{
		int last_max_num = -1;
		for (std::map<int, GodEquipAttrNumConfig>::iterator level_it = quality_it->second.begin(); level_it != quality_it->second.end(); level_it++)
		{
			if (last_max_num <= level_it->second.max_num)
			{
				last_max_num = level_it->second.max_num;
			}
			else
			{
				return -5;
			}
		}
	}

	return 0;
}

int EntryConfig::InitGodEquipVocActiveCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int voc_idx = 0;
		if (!GetSubNodeValue(dataElement, "hole_place", voc_idx) || voc_idx <= 0 || voc_idx > EquipmentParam::EXA_ATTR_NUM_MAX || voc_idx > JewelryParam::EXA_ATTR_NUM_MAX)
		{
			return -1;
		}

		int quality = 0;
		if (!GetSubNodeValue(dataElement, "quality", quality) || quality < Equipment::I_COLOR_STAR || quality > Equipment::I_COLOR_GOD)
		{
			return -2;
		}
		GodEquipActiveAttrNumConfig& node = m_god_equip_voc_active_cfg[quality][voc_idx - 1];

		if (!GetSubNodeValue(dataElement, "role_level", node.role_level))
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "godequip_level", node.godequip_level))
		{
			return -4;
		}

		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int EntryConfig::InitGodEquipVocUpLevelExpendCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int equip_num_quality = 0;
		if (!GetSubNodeValue(dataElement, "equip_num_quality", equip_num_quality) || equip_num_quality < Equipment::I_COLOR_WHITE || equip_num_quality >= Equipment::I_COLOR_MAX)
		{
			return -1;
		}
		QuenChingExpendCfg& node = m_god_equip_expend[equip_num_quality];
		node.equip_num_quality = equip_num_quality;

		if (!GetSubNodeValue(dataElement, "item_id", node.item_id))
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "consume_num", node.consume_num))
		{
			return -3;
		}

		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int EntryConfig::InitGodEquipVocUpLevelAddCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		QuenChingAddCfg node;

		if (!GetSubNodeValue(dataElement, "equip_num_quality", node.equip_num_quality) || node.equip_num_quality < Equipment::I_COLOR_WHITE || node.equip_num_quality >= Equipment::I_COLOR_MAX)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "att_type", node.att_type))
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "once_add", node.once_add))
		{
			return -3;
		}
		m_add_cfg_vec.push_back(node);
		dataElement = dataElement->NextSiblingElement();
	}
	memset(m_god_equip_add_config, NULL, sizeof(m_god_equip_add_config));
	for (std::vector<QuenChingAddCfg>::iterator it = m_add_cfg_vec.begin(); it != m_add_cfg_vec.end(); it++)
	{
		m_god_equip_add_config[it->equip_num_quality][it->att_type] = &(*it);
	}

	return 0;
}
