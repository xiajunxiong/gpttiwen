#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "randactivityjingyanbiaoshengconfig.hpp"

RandActivityJingYanBiaoShengConfig::RandActivityJingYanBiaoShengConfig()
{
}

RandActivityJingYanBiaoShengConfig::~RandActivityJingYanBiaoShengConfig()
{
}

bool RandActivityJingYanBiaoShengConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("exp_configure", InitBuyCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}


const RAJingYanBiaoShengItemCfg * RandActivityJingYanBiaoShengConfig::GetRAJingYanBiaoShengCfg(int role_level) const
{
	for (std::map<int, RAJingYanBiaoShengItemCfg> ::const_reverse_iterator iter = m_item_cfg.rbegin(); iter != m_item_cfg.rend(); ++iter)
	{
		if (role_level >= iter->first)
		{
			return &iter->second;
		}
	}


	return NULL;
}

const RAJingYanBiaoShengOtherCfg & RandActivityJingYanBiaoShengConfig::GetOtherCg() const
{
	return m_other_cfg;
}

int RandActivityJingYanBiaoShengConfig::InitBuyCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	while (NULL != data_element)
	{
		RAJingYanBiaoShengItemCfg item_cfg;
		if (!GetSubNodeValue(data_element, "grade", item_cfg.role_level)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "item_id", item_cfg.comsume_item_id)) return -__LINE__;
		if (!GetSubNodeValue(data_element, "item_num", item_cfg.comsume_num) || item_cfg.comsume_num < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "gold_num", item_cfg.need_gold) || item_cfg.need_gold < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "exp_num", item_cfg.get_exp) || item_cfg.get_exp <= 0) return -__LINE__;
		if (item_cfg.comsume_num == 0 && item_cfg.need_gold == 0)return -__LINE__;
		
		m_item_cfg[item_cfg.role_level] = item_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityJingYanBiaoShengConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "limit_type", m_other_cfg.limit_type)) return -__LINE__;
	if (!GetSubNodeValue(data_element, "buy_times", m_other_cfg.limit_buy_times)) return -__LINE__;
	if (!GetSubNodeValue(data_element, "today_times", m_other_cfg.limit_day_buy_times)) return -__LINE__;

	return 0;
}
