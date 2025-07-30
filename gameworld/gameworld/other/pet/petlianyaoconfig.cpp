#include "petlianyaoconfig.hpp"

PetLianYaoConfig::PetLianYaoConfig()
{
}

PetLianYaoConfig::~PetLianYaoConfig()
{
}

bool PetLianYaoConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("furnace", InitPetFurnaceCfg);
	LOAD_CONFIG("other", InitOtherCfg);

	return true;
}

const PetFurnaceCfg * PetLianYaoConfig::GetPetFurnaceCfg(int furnace_id)
{
	return MapValuesConstPtr(m_pet_furnace_cfg, furnace_id);
}

const PetFurnaceOtherCfg & PetLianYaoConfig::GetOtherCfg()
{
	return m_other_cfg;
}

int PetLianYaoConfig::InitPetFurnaceCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		PetFurnaceCfg cfg;
		if (!GetSubNodeValue(root_element, "furnace_id", cfg.furnace_id))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "open_level", cfg.lian_yao_unlock_need_role_level))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "feisheng_level", cfg.lian_yao_unlock_need_role_fei_sheng_times))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "pet_quality", cfg.lian_yao_unlock_need_pet_quality))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "pet_strengthen", cfg.lian_yao_unlock_need_pet_strengthen_level))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "main_max_quality", cfg.main_pet_max_quality))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "main_min_strengthen", cfg.main_pet_min_strengthen))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "second_max_quality", cfg.second_pet_max_quality))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "second_max_strengthen", cfg.second_pet_max_strengthen))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "coin_cost", cfg.need_coin))
		{
			ret_line;
		}
	
		m_pet_furnace_cfg[cfg.furnace_id] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetLianYaoConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;


	if (!GetSubNodeValue(root_element, "addskill_rate", m_other_cfg.lian_yao_add_passive_rate))
	{
		ret_line;
	}

	if (!GetSubNodeValue(root_element, "addskill_max", m_other_cfg.lian_yao_add_passive_max_num))
	{
		ret_line;
	}

	if (!GetSubNodeValue(root_element, "addskill_unlock", m_other_cfg.lian_yao_comprehend_passive_skill_need_item_id))
	{
		ret_line;
	}

	for (int i = 0; i < 3; ++i)
	{
		std::string str = STRING_SPRINTF("stuff_num%d", i);
		int t = 0;
		if (!GetSubNodeValue(root_element, str, t))
		{
			ret_line;
		}

		m_other_cfg.lian_yao_comprehend_passive_skill_need_num.push_back(t);
	}

	TiXmlElement * passive_group_id_element = root_element->FirstChildElement("passive_skill");
	if (NULL != passive_group_id_element)
	{
		TiXmlElement * node_element = passive_group_id_element->FirstChildElement("node");
		while (node_element)
		{
			int passive_skill_id = 0;
			if (!GetNodeValue(node_element, passive_skill_id) || passive_skill_id <= 0)ret_line;
			m_other_cfg.lian_yao_comprehend_passive_skill_pool.push_back(passive_skill_id);
			node_element = node_element->NextSiblingElement();
		}
	}


	return 0;
}
