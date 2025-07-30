#include "randactivitytujiantreasureconfig.hpp"

RandActivityTuJianTreasureConfig::RandActivityTuJianTreasureConfig()
{

}

RandActivityTuJianTreasureConfig::~RandActivityTuJianTreasureConfig()
{
	std::vector<RATuJianTreasureLoginCfg>().swap(m_login_cfg);
	std::vector<RATuJianTreasureRewardCfg>().swap(m_reward_cfg);
}

bool RandActivityTuJianTreasureConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitRATuJianTreasureOtherCfg);
	LOAD_RA_CONFIG("login_reward", InitRATuJianTreasureLoginCfg);
	LOAD_RA_CONFIG("treasure_reward", InitRATuJianTreasureRewardCfg);

	return true;
}

const RATuJianTreasureLoginCfg * RandActivityTuJianTreasureConfig::GetLoginCfg(int seq) const
{
	if (0 > seq || seq >= (int)m_login_cfg.size())
	{
		return NULL;
	}
	return &m_login_cfg[seq];
}

const RATuJianTreasureRewardCfg * RandActivityTuJianTreasureConfig::GetRewardCfg(int seq) const
{
	if (0 > seq || seq >= (int)m_reward_cfg.size())
	{
		return NULL;
	}
	return &m_reward_cfg[seq];
}

int RandActivityTuJianTreasureConfig::InitRATuJianTreasureOtherCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		RATuJianTreasureOtherCfg &cfg = m_other_cfg;

		if (!GetSubNodeValue(data_element, "open_grade", cfg.open_grade) || cfg.open_grade <= 0) return -1;
		if (!GetSubNodeValue(data_element, "open_time", cfg.open_time) || cfg.open_time <= 0) return -2;

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityTuJianTreasureConfig::InitRATuJianTreasureLoginCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		RATuJianTreasureLoginCfg cfg;

		if (!GetSubNodeValue(data_element, "seq", cfg.seq) || cfg.seq < 0) return -1;
		if (!GetSubNodeValue(data_element, "sign_day", cfg.sign_day) || cfg.sign_day <= 0) return -2;

		TiXmlElement * item_element = data_element->FirstChildElement("item");
		if (NULL != item_element)
		{
			if (!cfg.item.ReadConfig(item_element)) return -100;
		}

		m_login_cfg.push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityTuJianTreasureConfig::InitRATuJianTreasureRewardCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		RATuJianTreasureRewardCfg cfg;

		if (!GetSubNodeValue(data_element, "seq", cfg.seq) || cfg.seq < 0) return -1;
		if (!GetSubNodeValue(data_element, "type", cfg.type) || cfg.type <= 0) return -2;
		if (!GetSubNodeValue(data_element, "parameter1", cfg.param1) || cfg.param1 < 0) return -3;
		if (!GetSubNodeValue(data_element, "parameter2", cfg.param2) || cfg.param2 < 0) return -4;
		if (!GetSubNodeValue(data_element, "parameter3", cfg.param3) || cfg.param3 < 0) return -5;

		TiXmlElement * item_element = data_element->FirstChildElement("item");
		if (NULL != item_element)
		{
			if (!cfg.item.ReadConfig(item_element)) return -100;
		}

		m_reward_cfg.push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}
