#include "tianmingconfig.hpp"


TianMingConfig::TianMingConfig()
{

}

TianMingConfig::~TianMingConfig()
{

}

bool TianMingConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("tianming", InitTianMingCfg);

	return true;
}

const TianMingCfg * TianMingConfig::GetTianMingCfg(int _level)
{
	if (_level <= 0 || _level >= MAX_TIAN_MING_LEVEL_COUNT)
		return NULL;

	if (m_tian_ming_cfg_arr[_level].need_exp <= 0)return NULL;

	return &m_tian_ming_cfg_arr[_level];
}

int TianMingConfig::InitTianMingCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int last_level = 0;
	while (NULL != root_element)
	{
		int level = 0;
		if (!GetSubNodeValue(root_element, "tianming_level", level) || level <= 0 || level >= MAX_TIAN_MING_LEVEL_COUNT)
		{
			return -1;
		}

		if (0 == last_level && 1 != level)
			return -101;

		if (0 != last_level && last_level + 1 != level)
			return -102;

		TianMingCfg & cfg = m_tian_ming_cfg_arr[level];

		if (!GetSubNodeValue(root_element, "exp", cfg.need_exp) || cfg.need_exp <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "att_type", cfg.att_type) || cfg.att_type <= TIAN_MING_ADD_ATTR_SYS_TYPE_INVALID ||
			cfg.att_type >= TIAN_MING_ADD_ATTR_SYS_TYPE_MAX)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "att_per", cfg.att_per) || cfg.att_per <= 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "att_per_tol", cfg.att_per_tol) || cfg.att_per_tol <= 0)
		{
			return -5;
		}

		last_level = level;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
