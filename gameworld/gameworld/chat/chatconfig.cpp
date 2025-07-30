#include "chatconfig.h"
#include "servercommon/servercommon.h"

ChatConfig::ChatConfig()
{
}

ChatConfig::~ChatConfig()
{
}

bool ChatConfig::Init(const std::string & configname, std::string * err)
{
		PRE_LOAD_CONFIG;

		LOAD_CONFIG("other", IniOtherCfg);
		LOAD_CONFIG("channel_info", IniChatChanelCfg);

		return true;
}

const ChatChannelCfg & ChatConfig::GetChatChanelLimitLevel(int chanel)
{
	return m_chat_level_limit_cfg[chanel];
}

const ChatOtherCfg & ChatConfig::GetOtherCfg()
{
	return m_other_cfg;
}

int ChatConfig::IniChatChanelCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		int channel_id = 0;
		if (!GetSubNodeValue(root_element, "channel_id", channel_id) )
		{
			return -1;
		}

		ChatChannelCfg cfg;
		if (!GetSubNodeValue(root_element, "open_level", cfg.limit_level))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "interval_time", cfg.interval_time))
		{
			return -3;
		}

		m_chat_level_limit_cfg[channel_id] = cfg;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ChatConfig::IniOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)return -999;

	while (NULL != data_element)
	{
		if (!GetSubNodeValue(data_element, "horn", m_other_cfg.speaker_consume_item_id)) return -1;
		if (!GetSubNodeValue(data_element, "server_horn", m_other_cfg.cross_speaker_consume_item_id)) return -2;
		if (!GetSubNodeValue(data_element, "group_time", m_other_cfg.create_chat_group_cd_seconds)) return -3;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
