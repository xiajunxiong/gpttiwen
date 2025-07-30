#include "petinbornconfig.hpp"

#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"

PetInbornConfig::PetInbornConfig()
{
}

PetInbornConfig::~PetInbornConfig()
{
}

bool PetInbornConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("other", InitOtherCfg);
	LOAD_CONFIG_2("pet_inborn_level", InitPetInbornLevelCfg);		//天赋等级配置读取需在基础配置读取之前
	LOAD_CONFIG_2("pet_inborn_base", InitPetInbornBaseCfg);

	return true;
}

const PetInbornLevelCfg * PetInbornConfig::GetLevelCfg(int inborn_id, int inborn_level)
{
	std::map<int, std::map<int, PetInbornLevelCfg> >::const_iterator inborn_it = m_level_cfg.find(inborn_id);
	if (inborn_it == m_level_cfg.end())
	{
		return NULL;
	}
	std::map<int, PetInbornLevelCfg>::const_iterator it = inborn_it->second.find(inborn_level);
	if (it == inborn_it->second.end())
	{
		return NULL;
	}

	return &it->second;
}

void PetInbornConfig::GetLevelConsumeByReset(int inborn_id, int inborn_level, std::vector<ItemConfigData>& return_list) const
{
	std::map<int, std::map<int, PetInbornLevelCfg> >::const_iterator inborn_it = m_level_cfg.find(inborn_id);
	if (inborn_it == m_level_cfg.end() || inborn_it->second.empty())
	{
		return;
	}

	for (std::map<int, PetInbornLevelCfg>::const_iterator it = inborn_it->second.begin(); it != inborn_it->second.end(); ++it)
	{
		if (it->second.consume_list.empty()) continue;
		if (inborn_level < it->first) break;

		return_list.insert(return_list.end(), it->second.consume_list.begin(), it->second.consume_list.end());
	}

}

const PetInbornBaseCfg * PetInbornConfig::GetBaseCfg(int inborn_type, int inborn_group_type)
{
	int inborn_key = 1000 * inborn_type + inborn_group_type;
	std::map<int, PetInbornBaseCfg>::const_iterator it = m_inborn_cfg.find(inborn_key);
	if (it == m_inborn_cfg.end())
	{
		return NULL;
	}

	return &it->second;
}

int PetInbornConfig::InitOtherCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}
	
	if (!GetSubNodeValue(data_element, "replace_consume", m_other_cfg.replace_consume) || m_other_cfg.replace_consume <= 0)
	{
		errormsg = STRING_SPRINTF("[replace_consume=%d] error, is [replace_consume] <= 0?", m_other_cfg.replace_consume);
		return -1;
	}
	if (!GetSubNodeValue(data_element, "return_consume", m_other_cfg.return_consume) || m_other_cfg.return_consume <= 0)
	{
		errormsg = STRING_SPRINTF("[return_consume=%d] error, is [return_consume] <= 0?", m_other_cfg.return_consume);
		return -2;
	}

	return 0;
}

int PetInbornConfig::InitPetInbornBaseCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}
	while (NULL != data_element)
	{
		PetInbornBaseCfg node_cfg;
		if (!GetSubNodeValue(data_element, "inborn_type", node_cfg.inborn_type) || node_cfg.inborn_type <= 0)
		{
			errormsg = STRING_SPRINTF("[inborn_type=%d] error, is [inborn_type] <= 0?", node_cfg.inborn_type);
			return -1;
		}
		if (!GetSubNodeValue(data_element, "inborn_group_type", node_cfg.inborn_group_type) || node_cfg.inborn_group_type <= 0 || 
			node_cfg.inborn_group_type > MAX_PET_TALENT_GROUP_NUM)
		{
			errormsg = STRING_SPRINTF("[inborn_type=%d, group_type:%d] error, is [inborn_group_type] <= 0 or > %d?", node_cfg.inborn_type, 
				node_cfg.inborn_group_type, MAX_PET_TALENT_GROUP_NUM);
			return -2;
		}
		int inborn_key = node_cfg.inborn_type * 1000 + node_cfg.inborn_group_type;
		UNSTD_STATIC_CHECK(MAX_PET_TALENT_GROUP_NUM <= 1000);

		if (m_inborn_cfg.end() != m_inborn_cfg.find(inborn_key))
		{
			errormsg = STRING_SPRINTF("[inborn_type=%d, group_type:%d] error, is repeat?", node_cfg.inborn_type,
				node_cfg.inborn_group_type);
			return -3;
		}

		if (!GetSubNodeValue(data_element, "inborn_group_lock", node_cfg.unlock_group_str_level) || node_cfg.unlock_group_str_level <= 0 ||
			node_cfg.unlock_group_str_level > PET_STRENGTHEN_MAX_LEVEL)
		{
			errormsg = STRING_SPRINTF("[inborn_type=%d, group_type:%d] error, is [inborn_group_lock:%d] <= 0 or > %d?", node_cfg.inborn_type,
				node_cfg.inborn_group_type, node_cfg.unlock_group_str_level, PET_STRENGTHEN_MAX_LEVEL);
			return -4;
		}
		
		for (int i = 0; i < ARRAY_LENGTH(node_cfg.inborn_id_list); ++i)
		{
			std::string buf = STRING_SPRINTF("inborn_%d", i + 1);
			if (!GetSubNodeValue(data_element, buf, node_cfg.inborn_id_list[i]) || NULL == this->GetLevelCfg(node_cfg.inborn_id_list[i], 1))
			{
				errormsg = STRING_SPRINTF("[inborn_type=%d, group_type:%d] error, is [%s:%d] nof find?", node_cfg.inborn_type,
					node_cfg.inborn_group_type, buf.c_str(), node_cfg.inborn_id_list[i]);
				return -(10 + i);
			}
		}

		m_inborn_cfg[inborn_key] = node_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int PetInbornConfig::InitPetInbornLevelCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}
	int next_inborn_id = 0, next_inborn_level = 0;
	while (NULL != data_element)
	{
		int inborn_id = 0, inborn_level = 0;
		if (!GetSubNodeValue(data_element, "inborn_id", inborn_id) || inborn_id <= 0)
		{
			errormsg = STRING_SPRINTF("[inborn_id=%d] error, is [inborn_id] <= 0?", inborn_id);
			return -1;
		}
		if (!GetSubNodeValue(data_element, "inborn_level", inborn_level) || inborn_level <= 0)
		{
			errormsg = STRING_SPRINTF("[inborn_id=%d, inborn_level=%d] error, is [inborn_level] <= 0?", inborn_id, inborn_level);
			return -2;
		}

		if (NULL != this->GetLevelCfg(inborn_id, inborn_level))
		{
			errormsg = STRING_SPRINTF("[inborn_id=%d, inborn_level=%d] error, is repeat?", inborn_id, inborn_level);
			return -3;
		}

		if (next_inborn_id != inborn_id)
		{
			next_inborn_id = inborn_id;
			next_inborn_level = inborn_level;
		}
		else if (next_inborn_level != inborn_level)
		{
			errormsg = STRING_SPRINTF("[inborn_id=%d, inborn_level=%d] error, last[id:%d, level:%d], is lack some config?", inborn_id, inborn_level, 
				next_inborn_id, next_inborn_level - 1);
			return -4;
		}
		next_inborn_level += 1;

		PetInbornLevelCfg node_cfg;
		if (!GetSubNodeValue(data_element, "sp_id", node_cfg.sp_id) || NULL == BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(node_cfg.sp_id))
		{
			errormsg = STRING_SPRINTF("[inborn_id=%d, inborn_level:%d] error, is [sp_id:%d] not find?", inborn_id, inborn_level, node_cfg.sp_id);
			return -5;
		}

		if (!GetSubNodeValue(data_element, "inborn_grade", node_cfg.inborn_score) || node_cfg.inborn_score <= 0)
		{
			errormsg = STRING_SPRINTF("[inborn_id=%d, inborn_level:%d] error, is [inborn_grade:%d] <= 0?", inborn_id, inborn_level, node_cfg.inborn_score);
			return -6;
		}

		TiXmlElement *item_list_element = data_element->FirstChildElement("strengthen_consume_item_list");
		if (NULL == item_list_element)
		{
			return -100;
		}
		TiXmlElement *item_element = item_list_element->FirstChildElement("strengthen_consume_item");
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
			{
				return -200;
			}

			node_cfg.consume_list.push_back(item);
			item_element = item_element->NextSiblingElement();
		}

		m_level_cfg[inborn_id][inborn_level] = node_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
