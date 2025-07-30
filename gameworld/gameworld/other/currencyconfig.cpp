#include "currencyconfig.hpp"

Currencyconfig::Currencyconfig()
{
}

Currencyconfig::~Currencyconfig()
{
}

bool Currencyconfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("exp_crystal", InitExpCrystalCfg);
	return true;
}

int Currencyconfig::InitExpCrystalCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		if (!GetSubNodeValue(root_element, "id", m_exp_crystal_cfg.id))return -99;
		if (!GetSubNodeValue(root_element, "every_reply_crystal", m_exp_crystal_cfg.exp_crystal_day_count))return -1;
		if (!GetSubNodeValue(root_element, "max_crystal", m_exp_crystal_cfg.exp_crystal_max_count))return -2;
		if (!GetSubNodeValue(root_element, "expend_num", m_exp_crystal_cfg.expend_num))return -3;
		if (!GetSubNodeValue(root_element, "is_open", m_exp_crystal_cfg.is_open))return -4;
		if (!GetSubNodeValue(root_element, "bonus_exp_num_percentage", m_exp_crystal_cfg.bonus_exp_num_percentage))return -5;
		
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
