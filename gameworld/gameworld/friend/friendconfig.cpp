#include "friendconfig.h"

FriendConfig::FriendConfig()
{
}

FriendConfig::~FriendConfig()
{
}

bool FriendConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	//LOAD_CONFIG("other", InitOtherCfg);
	return true;
}

const FriendOtherConfig & FriendConfig::GetOtherCfg()
{
	return m_other_cfg;
}

int FriendConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "max_qinmi", m_other_cfg.max_battle_add_intimacy))
	{
		return -1;
	}

	return 0;
}
