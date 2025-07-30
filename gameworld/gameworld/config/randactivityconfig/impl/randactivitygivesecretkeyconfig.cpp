#include "randactivitygivesecretkeyconfig.hpp"

RandActivityGiveSecretKeyConfig::RandActivityGiveSecretKeyConfig()
{

}

RandActivityGiveSecretKeyConfig::~RandActivityGiveSecretKeyConfig()
{

}

bool RandActivityGiveSecretKeyConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("target", InitGiveSecretKeyCfg);

	return true;
}



const GiveSecretKeyCfg * RandActivityGiveSecretKeyConfig::GetRewardCfg(int seq)const
{
	std::map<int, GiveSecretKeyCfg>::const_iterator it = m_cfg_map.find(seq);
	if (it != m_cfg_map.end())
	{
		return &it->second;
	}
	return NULL;
}

int RandActivityGiveSecretKeyConfig::InitGiveSecretKeyCfg(TiXmlElement *RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq < 0 || seq >= (sizeof(int) * 8)) return -1;
		GiveSecretKeyCfg &node_cfg = m_cfg_map[seq];
		node_cfg.seq = seq;
		if (!GetSubNodeValue(root_element, "type", node_cfg.type)) return -2;
		if (!GetSubNodeValue(root_element, "parameter", node_cfg.parameter)) return -3;

		TiXmlElement *node_element = root_element->FirstChildElement("item");
		if (node_element == NULL)return -3;

		node_cfg.reward_item.ReadConfig(node_element);

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}