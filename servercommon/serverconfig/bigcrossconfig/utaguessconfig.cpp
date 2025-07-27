#include "utaguessconfig.hpp"

UTAGuessConfig::UTAGuessConfig()
{

}

UTAGuessConfig::~UTAGuessConfig()
{

}

bool UTAGuessConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("other", InitOtherConfig);

	return true;
}

int UTAGuessConfig::InitOtherConfig(TiXmlElement *RootElement, std::string& errormsg)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	if (!GetSubNodeValue(dataElement, "lottery_id", m_other_cfg.lottery_id) || m_other_cfg.lottery_id <= 0 || m_other_cfg.lottery_id >= 65535)
	{
		return -1;
	}

	if (!GetSubNodeValue(dataElement, "exchange_rate", m_other_cfg.exchange_rate) || m_other_cfg.exchange_rate <= 0 )
	{
		return -2;
	}

	// 策划填的数值是含本金的 , 所以得大于100 , 百分比
	if (!GetSubNodeValue(dataElement, "min_odds", m_other_cfg.min_odds) || m_other_cfg.min_odds <= 100) 
	{
		return -3;
	}

	// 变动的倍率 最大值不应该小于等于最小值
	if (!GetSubNodeValue(dataElement, "max_odds", m_other_cfg.max_odds) || m_other_cfg.max_odds <= m_other_cfg.min_odds)
	{
		return -4;
	}

	if (!GetSubNodeValue(dataElement, "once_lottery_limit", m_other_cfg.once_lottery_limit) || m_other_cfg.once_lottery_limit < 0)
	{
		return -5;
	}

	return 0;
}
