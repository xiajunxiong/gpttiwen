#include "utaguessingconfig.hpp"

UTAGuessingConfig::UTAGuessingConfig()
{

}

UTAGuessingConfig::~UTAGuessingConfig()
{

}

bool UTAGuessingConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("other", InitOtherConfig);

	return true;
}

int UTAGuessingConfig::InitOtherConfig(TiXmlElement *RootElement, std::string& errormsg)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	if (!GetSubNodeValue(dataElement, "exchange_rate", m_other_cfg.exchange_rate) || m_other_cfg.exchange_rate <= 0 )
	{
		return -1;
	}

	// 策划填的数值是含本金的 , 所以得大于100 , 百分比
	if (!GetSubNodeValue(dataElement, "min_odds", m_other_cfg.min_odds) || m_other_cfg.min_odds <= 100) 
	{
		return -2;
	}

	// 变动的倍率 最大值不应该小于等于最小值
	if (!GetSubNodeValue(dataElement, "max_odds", m_other_cfg.max_odds) || m_other_cfg.max_odds <= m_other_cfg.min_odds)
	{
		return -3;
	}

	return 0;
}
