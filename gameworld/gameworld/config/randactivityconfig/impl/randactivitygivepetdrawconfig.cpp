#include "randactivitygivepetdrawconfig.hpp"
#include "servercommon/activitydef.hpp"

RandActivityGivePetDrawConfig::RandActivityGivePetDrawConfig()
{
}

RandActivityGivePetDrawConfig::~RandActivityGivePetDrawConfig()
{
}

bool RandActivityGivePetDrawConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitGivePetDrawOtherCfg);

	return true;
}

int RandActivityGivePetDrawConfig::InitGivePetDrawOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		RAGivePetDrawOtherCfg & cfg = m_other_cfg;

		TiXmlElement * item_element = data_element->FirstChildElement("reward_item");
		if (NULL != item_element)
		{
			if (!cfg.item.ReadConfig(item_element)) return -100;
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
