#include "cowreportgoodnewsconfig.hpp"

CowReportGoodNewsConfig::CowReportGoodNewsConfig()
{

}

CowReportGoodNewsConfig::~CowReportGoodNewsConfig()
{

}

bool CowReportGoodNewsConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("set_cow", InitOtherCfg);

	return true;
}

int CowReportGoodNewsConfig::InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -101010;
	}

	if (!GetSubNodeValue(data_element, "current_price", m_other_cfg.current_price) || m_other_cfg.current_price <= 0)
	{
		errormsg = STRING_SPRINTF("[hook_add=%d] error, current_price <= 0", m_other_cfg.current_price);
		return -1;
	}

	{
		// Éê¹º½±Àø
		TiXmlElement * reward_item_list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL == reward_item_list_element)
		{
			errormsg = STRING_SPRINTF("[reward_item_list] node is not exist");
			return -2;
		}

		TiXmlElement * reward_item_element = reward_item_list_element->FirstChildElement("reward_item");
		if (NULL == reward_item_element)
		{
			errormsg = STRING_SPRINTF("[reward_item] node is not exist");
			return -201;
		}

		m_other_cfg.reward_count = 0;
		while (NULL != reward_item_element)
		{
			if (m_other_cfg.reward_count >= MAX_ATTACHMENT_ITEM_NUM)
			{
				errormsg = STRING_SPRINTF("[reward_item] node is not exist");
				return -202;
			}

			if (!m_other_cfg.reward_item[m_other_cfg.reward_count++].ReadConfig(reward_item_element))
			{
				errormsg = STRING_SPRINTF("[reward_item] config is error");
				return -203;
			}

			reward_item_element = reward_item_element->NextSiblingElement();
		}
	}

	if (!GetSubNodeValue(data_element, "return_item", m_other_cfg.return_count) || m_other_cfg.return_count < 0)
	{
		errormsg = STRING_SPRINTF("[return_item=%d] error, return_item < 0", m_other_cfg.return_count);
		return -3;
	}

	if (!GetSubNodeValue(data_element, "start_time", m_other_cfg.start_time) || m_other_cfg.start_time < 0)
	{
		errormsg = STRING_SPRINTF("[start_time=%d] error, start_time < 0", m_other_cfg.start_time);
		return -4;
	}

	if (!GetSubNodeValue(data_element, "until_time", m_other_cfg.until_time) || m_other_cfg.until_time <= 0 )
	{
		errormsg = STRING_SPRINTF("[until_time=%d] error, until_time <= 0 ", m_other_cfg.until_time);
		return -5;
	}

	if (!GetSubNodeValue(data_element, "known_time", m_other_cfg.known_time) || m_other_cfg.known_time < 0 )
	{
		errormsg = STRING_SPRINTF("[known_time=%d] error, known_time < 0 ", m_other_cfg.known_time);
		return -6;
	}

	if (!GetSubNodeValue(data_element, "show_time", m_other_cfg.show_time) || m_other_cfg.show_time <= 0)
	{
		errormsg = STRING_SPRINTF("[show_time=%d] error, show_time <= 0 ", m_other_cfg.show_time);
		return -7;
	}

	if (!GetSubNodeValue(data_element, "timestamp", m_other_cfg.timestamp) || m_other_cfg.timestamp < 0)
	{
		errormsg = STRING_SPRINTF("[timestamp=%d] error, timestamp < 0 ", m_other_cfg.timestamp);
		return -8;
	}

	return 0;
}
