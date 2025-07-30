#include "randactivitysanbaichouconfig.hpp"
#include "servercommon/roleactivity/rasanbaichouparam.hpp"

RandActivitySanBaiChouConfig::RandActivitySanBaiChouConfig()
{
}

RandActivitySanBaiChouConfig::~RandActivitySanBaiChouConfig()
{
	std::vector<RASanBaiChouTypeCfg>().swap(m_type_cfg);
	std::vector<ItemConfigData>().swap(m_seq_item_cfg);
}

bool RandActivitySanBaiChouConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("basis_configure", InitTypeCfg);
	LOAD_RA_CONFIG("reward_group", InitGroupCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const RASanBaiChouTypeCfg * RandActivitySanBaiChouConfig::GetTypeCfg(int type) const
{
	if (type < 0 || type >= (int)m_type_cfg.size())
		return NULL;

	return &m_type_cfg[type];
}

const RASanBaiChouGroupCfg * RandActivitySanBaiChouConfig::GetGroupCfg(int group) const
{
	std::map<int, RASanBaiChouGroupCfg>::const_iterator it = m_group_cfg.find(group);
	if (it == m_group_cfg.end())
		return NULL;

	return &it->second;
}

const ItemConfigData * RandActivitySanBaiChouConfig::GetItemCfg(int seq) const
{
	if (seq < 0 || seq >= (int)m_seq_item_cfg.size())
		return NULL;

	return &m_seq_item_cfg[seq];
}

int RandActivitySanBaiChouConfig::InitTypeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		RASanBaiChouTypeCfg cfg;
		if (!GetSubNodeValue(data_element, "level", cfg.level) || cfg.level <= 0 || cfg.level > MAX_ROLE_LEVEL)
			return -__LINE__;

		if (!GetSubNodeValue(data_element, "extract_times", cfg.extract_times) || cfg.extract_times <= 0)
			return -__LINE__;

		if (!GetSubNodeValue(data_element, "reward_group", cfg.reward_group))
			return -__LINE__;

		m_type_cfg.push_back(cfg);
		data_element = data_element->NextSiblingElement();
	}

	if (m_type_cfg.size() > MAX_RA_SANBAI_CHOU_TYPE_NUM)
		return -__LINE__;

	return 0;
}

int RandActivitySanBaiChouConfig::InitGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int group = 0;
		if (!GetSubNodeValue(data_element, "reward_group", group) || group <= 0)
			return -__LINE__;

		RASanBaiChouItem cfg;
		if (!GetSubNodeValue(data_element, "type", cfg.seq) || cfg.seq < 0)
			return -__LINE__;

		if (!GetSubNodeValue(data_element, "rate", cfg.rate) || cfg.rate <= 0)
			return -__LINE__;

		m_group_cfg[group].item_list.push_back(cfg);
		m_group_cfg[group].max_rate += cfg.rate;

		if (!GetSubNodeValue(data_element, "is_guaranteed", cfg.is_baodi))
			return -__LINE__;

		if (cfg.is_baodi)
		{
			m_group_cfg[group].baodi_item_list.push_back(cfg);
			m_group_cfg[group].baodi_rate += cfg.rate;
		}

		ItemConfigData item;
		TiXmlElement * item_element = data_element->FirstChildElement("reward_item");
		if (NULL == item_element)
			return -__LINE__;

		if (!item.ReadConfig(item_element))
			return -__LINE__;


		m_seq_item_cfg.push_back(item);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivitySanBaiChouConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(data_element, "reward_extract_times", m_other_cfg.reward_extract_times) || m_other_cfg.reward_extract_times <= 0)
		return -__LINE__;

	if (!GetSubNodeValue(data_element, "guaranteed_times", m_other_cfg.guaranteed_times) || m_other_cfg.guaranteed_times <= 0)
		return -__LINE__;

	return 0;
}
