#include "randactivitylinkagegiftconfig.hpp"

RandActivityLinkageGiftConfig::RandActivityLinkageGiftConfig()
{

}

RandActivityLinkageGiftConfig::~RandActivityLinkageGiftConfig()
{
	std::vector<LinkageGiftRewardCfg>().swap(m_reward_cfg);
}

bool RandActivityLinkageGiftConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("linkage_reward", InitRewardCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

int RandActivityLinkageGiftConfig::GetLinkageGiftRewardCfgSeq(int level)const
{
	if (level <= 0 || level > MAX_ROLE_LEVEL) return -1;

	for (int i = 0; i < (int)m_reward_cfg.size(); i++)
	{
		if (m_reward_cfg[i].grade_start <= level && m_reward_cfg[i].grade_end >= level)
		{
			return i;
		}
	}
	return -1;
}

const LinkageGiftRewardCfg * RandActivityLinkageGiftConfig::GetLinkageGiftRewardCfgBySeq(int seq)const
{
	if (seq < 0 || seq >= (int)m_reward_cfg.size()) return NULL;

	return &m_reward_cfg[seq];
}

int RandActivityLinkageGiftConfig::InitRewardCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	while (NULL != data_element)
	{
		LinkageGiftRewardCfg node;
		if (!GetSubNodeValue(data_element, "grade_start", node.grade_start) || node.grade_start < 0 || node.grade_start > MAX_ROLE_LEVEL) return -1;
		if (!GetSubNodeValue(data_element, "grade_end", node.grade_end) || node.grade_end < 0 || node.grade_end > MAX_ROLE_LEVEL) return -2;

		if (node.grade_end == 0)
		{
			node.grade_end = MAX_ROLE_LEVEL;
		}
		TiXmlElement *item_element = data_element->FirstChildElement("reward_item");
		if (NULL != item_element)
		{
			if (!node.item.ReadConfig(item_element)) return -100;
		}

		m_reward_cfg.push_back(node);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityLinkageGiftConfig::InitOtherCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(data_element, "receive_time", m_other_cfg.receive_time) || m_other_cfg.receive_time < 0) return -1;

	return 0;
}
