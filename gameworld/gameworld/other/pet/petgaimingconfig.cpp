#include "petgaimingconfig.hpp"

PetGaiMingConfig::PetGaiMingConfig()
{
}

PetGaiMingConfig::~PetGaiMingConfig()
{
}

bool PetGaiMingConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("att_type", InitPetGaiMingAttrCfg);
	LOAD_CONFIG("remould_level", InitPetGaiMingLevelCfg);
	LOAD_CONFIG("other", InitOtherCfg);

	return true;
}

const PetGaiMingLevelCfg * PetGaiMingConfig::GetPetGaiMingLevelCfg(int level)
{
	return MapValuesConstPtr(m_level_cfg,level);
}

const AttrItemCfg * PetGaiMingConfig::GetAttrItemCfg(int seq, int level)
{
	const PetGaiMingAttrCfg * cfg = MapValuesConstPtr(m_attr_cfg, seq);
	if (!cfg)
	{
		return NULL;
	}

	int index = level - 1;
	if (index < 0 || index >= (int)cfg->level_attr.size())
	{
		return NULL;
	}

	return &cfg->level_attr[index];
}

const PetGaiMingOtherCfg & PetGaiMingConfig::GetOtherCfg() const
{
	return m_other_cfg;
}

int PetGaiMingConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	if (!GetSubNodeValue(root_element, "remould_id", m_other_cfg.up_level_item_id))	return -__LINE__;
	if (!GetSubNodeValue(root_element, "remould_id_exp", m_other_cfg.add_exp))	return -__LINE__;
	if (!GetSubNodeValue(root_element, "up_max", m_other_cfg.attr_max_level))	return -__LINE__;
	if (!GetSubNodeValue(root_element, "fangsheng_fanhuan", m_other_cfg.return_percent)) return -__LINE__;

	return 0;
}

int PetGaiMingConfig::InitPetGaiMingAttrCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		PetGaiMingAttrCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.seq))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "is_pet_attr", cfg.is_pet_attr))	return -__LINE__;

		AttrItemCfg attr_cfg;
		attr_cfg.is_pet_attr = cfg.is_pet_attr;
		if (!GetSubNodeValue(root_element, "att_type", attr_cfg.attr_type))	return -__LINE__;

		for (int i = 0; i < 10; ++i)
		{
			std::string s = STRING_SPRINTF("level_%d", i + 1);
			if (!GetSubNodeValue(root_element, s, attr_cfg.attr_value))	return -__LINE__;
			cfg.level_attr.push_back(attr_cfg);
		}
			
		m_attr_cfg[cfg.seq] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PetGaiMingConfig::InitPetGaiMingLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		PetGaiMingLevelCfg cfg;
		if (!GetSubNodeValue(root_element, "level", cfg.level))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "up_exp", cfg.max_exp))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "break_item", cfg.break_item_id))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "break_item_num", cfg.break_item_num))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "chongzhi_item", cfg.reset_item_id))	return -__LINE__;
		if (!GetSubNodeValue(root_element, "chongzhi_item_num", cfg.reset_item_num))	return -__LINE__;

		cfg.need_break = cfg.break_item_id != 0 && cfg.break_item_num != 0;
		m_level_cfg[cfg.level] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
