#ifndef _CHAT_CONFIG_HPP_
#define _CHAT_CONFIG_HPP_

#include "servercommon/configcommon.h"

#include<map>

struct ChatChannelCfg
{
	ChatChannelCfg()
	{
		limit_level = 0;
		interval_time = 10;
	}

	int limit_level;
	int interval_time;
};

struct ChatOtherCfg
{
	ChatOtherCfg():speaker_consume_item_id(0), cross_speaker_consume_item_id(0), create_chat_group_cd_seconds(0) {}

	int speaker_consume_item_id;
	int cross_speaker_consume_item_id;	
	int create_chat_group_cd_seconds;
};

class ChatConfig : public ILogicConfig
{
public:
	ChatConfig();
	~ChatConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ChatChannelCfg & GetChatChanelLimitLevel(int chanel);
	const ChatOtherCfg & GetOtherCfg();
private:
	int IniChatChanelCfg(TiXmlElement *RootElement);
	int IniOtherCfg(TiXmlElement *RootElement);

	std::map<int, ChatChannelCfg> m_chat_level_limit_cfg;
	ChatOtherCfg m_other_cfg;
};



#endif 