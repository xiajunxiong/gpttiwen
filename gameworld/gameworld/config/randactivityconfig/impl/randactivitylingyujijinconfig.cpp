#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "randactivitylingyujijinconfig.hpp"

RandActivityLingYuJiJinConfig::RandActivityLingYuJiJinConfig()
{
}

RandActivityLingYuJiJinConfig::~RandActivityLingYuJiJinConfig()
{
	std::vector<RALingYuJiJinCfg>().swap(m_fetch_reward_cfg);
}

bool RandActivityLingYuJiJinConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("phase_configure", InitBuyCfg);
	LOAD_RA_CONFIG("gift_configure", InitFetchCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);
	return true;
}

const RALingYuJiJinPhareCfg * RandActivityLingYuJiJinConfig::GetRALingYuJiJinPhareCfg(int phase) const
{
	return MapValuesConstPtr(m_buy_phase_cfg, phase);
}

const RALingYuJiJinCfg * RandActivityLingYuJiJinConfig::GetRALingYuJiJinCfg(int type) const
{
	if (type < 0 || type >= (int)m_fetch_reward_cfg.size())
	{
		return NULL;
	}

	return &m_fetch_reward_cfg[type];
}

const RALingYuJiJinOtherCfg & RandActivityLingYuJiJinConfig::GetnOtherCfg() const
{
	return m_other_cfg;
}

int RandActivityLingYuJiJinConfig::GetConfigVer() const
{
	return m_other_cfg.cfg_ver;
}

int RandActivityLingYuJiJinConfig::InitFetchCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	int next_type = 0;
	while (NULL != data_element)
	{
		RALingYuJiJinCfg item_cfg;
		if (!GetSubNodeValue(data_element, "type", item_cfg.type) || item_cfg.type != next_type)
		{
			return -__LINE__;
		}
		if (!GetSubNodeValue(data_element, "seq", item_cfg.seq) || item_cfg.seq < 0 || item_cfg.seq >= RA_LING_YU_JI_JIN_MAX_ITEM)return -__LINE__;
		if (!GetSubNodeValue(data_element, "phase", item_cfg.need_active_phase) || item_cfg.need_active_phase < 0 || item_cfg.need_active_phase >= RA_LING_YU_JI_JIN_MAX_PHASE) return -__LINE__;
		if (!GetSubNodeValue(data_element, "reward_level", item_cfg.need_role_level)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "reward_immortal_coin", item_cfg.reward_ling_yu_num) || item_cfg.reward_ling_yu_num <= 0) return -__LINE__;

		next_type += 1;
		m_fetch_reward_cfg.push_back(item_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityLingYuJiJinConfig::InitBuyCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	int next_type = 0;
	while (NULL != data_element)
	{
		RALingYuJiJinPhareCfg item_cfg;
		if (!GetSubNodeValue(data_element, "phase", item_cfg.phase) || item_cfg.phase < 0 || item_cfg.phase >= RA_LING_YU_JI_JIN_MAX_PHASE)return -__LINE__;
		if (!GetSubNodeValue(data_element, "reward_buy", item_cfg.reward_ling_yu_num) || item_cfg.reward_ling_yu_num <= 0)return -__LINE__;
		if (!GetSubNodeValue(data_element, "buy_money", item_cfg.need_chong_zhi) || item_cfg.need_chong_zhi <= 0) return -__LINE__;

		next_type += 1;
		m_buy_phase_cfg[item_cfg.phase] = item_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityLingYuJiJinConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "cfg_ver", m_other_cfg.cfg_ver))return -__LINE__;
	return 0;
}
