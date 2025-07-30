#include "knapsack_config.hpp"
#include "servercommon/struct/itemlistparam.h"

KnapsackConfig::KnapsackConfig()
{
	memset(m_sub_type_prior, 0, sizeof(m_sub_type_prior));

	int prior = ItemNamespace::ITEM_SUB_TYPE_MAX;

	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_SHIZHUANG] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_APPEARANCE] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_SKIN] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_EXP_CRYSTAL] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_WEAPON] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_ARMOR] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_JEWELRY] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_NO_CHECK_EQUIP] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_MEDAL] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_PARTNER_NOREXITEM] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_PARTNER_FRAGMENT] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_PET_FRAGMENT] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_PET_BADGE] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_PET_EQUIPMENT] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_PARTNER_EQUIPMENT] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_FABAO] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_PET_SKILL] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_ZHU_LING] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_ATTR_PELLET] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_GEM] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_ADD_MEDAL_SP] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_GIFT] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_FB_CRYSTAL] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_SHANHAI_KEY] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_KEYS] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_MAGIC_STONE] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_ELEMENT_CRYSTAL] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_SKILL_GRASS] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_ELEMENT_FRAGMENT] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_ATTR_PILL] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_MEDICINE] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_COLLECTION] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_NORMAL_ITEM] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_TASK_ITEM] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_STONE] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_WOOD] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_HERB] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_FISH] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_FABRIC] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_PET_SOUL] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_GOD_PRINT] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_SPIRIT_PACT] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_PEISHI] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_DIANHUASHI] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_PEAK_TOKEN] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_YUAN_HUN_ZHU] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_ZAO_HUA_EQUIP] = prior--;	
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_HOLY_EQUIP] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_RUNE_STONE] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_HALLOWS_SKILL] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_FU_YU] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_SMART_MOUNTS_SKILL] = prior--;
	m_sub_type_prior[ItemNamespace::ITEM_SUB_TYPE_HUAN_SHOU_SKILL] = prior--;

	UNSTD_STATIC_CHECK(ItemNamespace::ITEM_SUB_TYPE_MAX == 53);

#ifdef _DEBUG
	assert(0 == prior);

	for (int i = 0; i < ItemNamespace::ITEM_SUB_TYPE_MAX; ++i)
	{
		assert(m_sub_type_prior[i] > 0);
	}
#endif
}

KnapsackConfig::~KnapsackConfig()
{
	std::vector<ItemReturnConfig>().swap(m_item_vec);
}

bool KnapsackConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("bag", InitBagCfg);
	LOAD_CONFIG("expand_price", InitExtendCfg);
	LOAD_CONFIG("job_bag", InitItemConvertCfg);
	LOAD_CONFIG("item_return", InitItemReturnCfg);

	return true;
}

const KnapsackGridCfg* KnapsackConfig::GetKnapsackGridCfg(int column)
{
	if (column < 0 || column >= ItemNamespace::ITEM_COLUMN_TYPE_MAX)
	{
		return NULL;
	}

	return &m_grid_cfg_list[column];
}

int KnapsackConfig::GetItemIdConvertByJob(int item_id, int job)
{
	std::map<ItemConvertByJobKey, int>::iterator iter = m_item_convert_map.find(ItemConvertByJobKey(item_id, job));
	if (iter != m_item_convert_map.end())
	{
		return iter->second;
	}

	return 0;
}

int KnapsackConfig::GetSubTypePrior(int item_sub_type)
{
	if (item_sub_type >= 0 && item_sub_type < ItemNamespace::ITEM_SUB_TYPE_MAX)
	{
		return m_sub_type_prior[item_sub_type];
	}
	return 0;
}

const ItemReturnConfig * KnapsackConfig::GetItemReturnConfig(int item_id)
{
	if (item_id < 0 || item_id >= MAX_UINT16)return NULL;

	return item_return[item_id];
}

int KnapsackConfig::InitBagCfg(TiXmlElement *RootElement, std::string& errormsg)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	int last_bag_id = -1;
	while (NULL != data_element)
	{
		int bag_id = 0;
		if (!GetSubNodeValue(data_element, "bag_id", bag_id) || bag_id < 0 || bag_id >= ItemNamespace::ITEM_COLUMN_TYPE_MAX || bag_id != last_bag_id + 1)
		{
			errormsg = STRING_SPRINTF("[bag_id=%d] error, is bag_id >= ITEM_COLUMN_TYPE_MAX(%d)?", bag_id, ItemNamespace::ITEM_COLUMN_TYPE_MAX);
			return -1;
		}
		last_bag_id = bag_id;

		KnapsackGridCfg& cfg = m_grid_cfg_list[bag_id];
		cfg.column_id = bag_id;

		if (!GetSubNodeValue(data_element, "start_num", cfg.start_grid_num) || cfg.start_grid_num < 0)
		{
			errormsg = STRING_SPRINTF("[bag_id=%d] start_num error, is start_num(%d) < 0?", bag_id, cfg.start_grid_num);
			return -2;
		}

		if (!GetSubNodeValue(data_element, "add_num", cfg.extend_grid_num) || cfg.extend_grid_num < 0)
		{
			errormsg = STRING_SPRINTF("[bag_id=%d] add_num error, is add_num(%d) < 0?", bag_id, cfg.extend_grid_num);
			return -3;
		}

		if (!GetSubNodeValue(data_element, "max_num", cfg.max_grid_num) || cfg.max_grid_num < 0 
			|| cfg.start_grid_num + cfg.extend_grid_num > cfg.max_grid_num
			|| cfg.max_grid_num > ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX)
		{
			errormsg = STRING_SPRINTF("[bag_id=%d] max_num error, is max_num(%d) > ITEM_COLUMN_GRID_NUM_MAX(%d) ?", 
				bag_id, cfg.max_grid_num, ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX);
			return -4;
		}

		data_element = data_element->NextSiblingElement();
	}

	if (m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_STORAGE].start_grid_num != m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_STORAGE].extend_grid_num)
	{
		// 仓库是按页分的， 所以初始格数和扩展格数要一致
		errormsg = STRING_SPRINTF("[FinalCheck] storage start_num(%d) must equal to add_num(%d)",
			m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_STORAGE].start_grid_num,
			m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_STORAGE].extend_grid_num);
		return -11;
	}

	return 0;
}

int KnapsackConfig::InitExtendCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int times = 0;
		if (!GetSubNodeValue(data_element, "bag_item", times))
		{
			return -1;
		}
		if (times > 0)
		{
			int max_times = m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM].max_extend_times;
			m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM].extend_cost_gold_list[max_times] = times;
			m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_ITEM].max_extend_times += 1;
		}

		if (!GetSubNodeValue(data_element, "bag_material", times))
		{
			return -2;
		}
		if (times > 0)
		{
			int max_times = m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_TASK_MATERIAL].max_extend_times;
			m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_TASK_MATERIAL].extend_cost_gold_list[max_times] = times;
			m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_TASK_MATERIAL].max_extend_times += 1;
		}
		
		
		if (!GetSubNodeValue(data_element, "bag_Illustrations", times))
		{
			return -3;
		}
		if (times > 0)
		{
			int max_times = m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_ILLUSTRATION].max_extend_times;
			m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_ILLUSTRATION].extend_cost_gold_list[max_times] = times;
			m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_ILLUSTRATION].max_extend_times += 1;
		}
	
		if (!GetSubNodeValue(data_element, "bag_fragment", times)) // 已改成碎片
		{
			return -4;
		}
		if (times > 0)
		{
			int max_times = m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_FRAGMENT].max_extend_times;
			m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_FRAGMENT].extend_cost_gold_list[max_times] = times;
			m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_FRAGMENT].max_extend_times += 1;
		}
		

		if (!GetSubNodeValue(data_element, "bag_supply", times))
		{
			return -5;
		}
		if (times > 0)
		{
			int max_times = m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_SUPPLY].max_extend_times;
			m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_SUPPLY].extend_cost_gold_list[max_times] = times;
			m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_SUPPLY].max_extend_times += 1;
		}

		if (!GetSubNodeValue(data_element, "warehouse", times))
		{
			return -6;
		}
		if (times > 0)
		{
			int max_times = m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_STORAGE].max_extend_times;
			m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_STORAGE].extend_cost_gold_list[max_times] = times;
			m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_STORAGE].max_extend_times += 1;
		}

		if (!GetSubNodeValue(data_element, "pet_weapon_bag", times))
		{
			return -7;
		}
		if (times > 0)
		{
			int max_times = m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP].max_extend_times;
			m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP].extend_cost_gold_list[max_times] = times;
			m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_PET_SOUL_EQIOP].max_extend_times += 1;
		}

		if (!GetSubNodeValue(data_element, "peishi_bag", times))
		{
			return -8;
		}
		if (times > 0)
		{
			int max_times = m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_ACCESSORIES].max_extend_times;
			m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_ACCESSORIES].extend_cost_gold_list[max_times] = times;
			m_grid_cfg_list[ItemNamespace::ITEM_COLUMN_TYPE_ACCESSORIES].max_extend_times += 1;
		}

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int KnapsackConfig::InitItemConvertCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		ItemConvertByJobKey key;
		if (!GetSubNodeValue(data_element, "old_id", key.item_id) )
		{
			return -1;
		}
		if (!GetSubNodeValue(data_element, "job_type", key.job))
		{
			return -2;
		}
		int new_id;
		if (!GetSubNodeValue(data_element, "new_id", new_id))
		{
			return -3;
		}
		m_item_convert_map[key] = new_id;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int KnapsackConfig::InitItemReturnCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		ItemReturnConfig node;

		if (!GetSubNodeValue(data_element, "item", node.item_id))
		{
			return -1;
		}
		if (!GetSubNodeValue(data_element, "price_type", node.price_type))
		{
			return -2;
		}
		if (!GetSubNodeValue(data_element, "price", node.price))
		{
			return -3;
		}
		if (!GetSubNodeValue(data_element, "mail_type", node.mail_type) || node.mail_type <= ITEM_RETURN_MAIL_TYPE_BEGIN || node.mail_type >= ITEM_RETURN_MAIL_TYPE_MAX)
		{
			return -4;
		}
		if (!GetSubNodeValue(data_element, "mail_sort", node.mail_sort))
		{
			return -5;
		}

		TiXmlElement *Item_list_element = data_element->FirstChildElement("return_item_list");
		if (NULL != Item_list_element)
		{
			TiXmlElement *return_item_element = Item_list_element->FirstChildElement("return_item");
			while (NULL != return_item_element)
			{
				ItemConfigData reward_item;
				if (!reward_item.ReadConfig(return_item_element))
				{
					return -5;
				}
				node.return_item.push_back(reward_item);
				return_item_element = return_item_element->NextSiblingElement();
			}
		}

		m_item_vec.push_back(node);

		data_element = data_element->NextSiblingElement();
	}
	memset(item_return, NULL, sizeof(item_return));
	for (std::vector<ItemReturnConfig>::iterator it = m_item_vec.begin(); it != m_item_vec.end(); it++)
	{
		if (it->item_id >= 0 && it->item_id < MAX_UINT16)
		{
			item_return[it->item_id] = &(*it);
		}
	}

	return 0;
}

