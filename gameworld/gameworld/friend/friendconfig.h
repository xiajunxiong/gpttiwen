#ifndef __FRIEND_CONFIG_HPP__
#define __FRIEND_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/serverdef.h"
#include "gamedef.h"
#include "servercommon/configcommon.h"

struct FriendOtherConfig
{
	FriendOtherConfig() :max_battle_add_intimacy(0)
	{
	}

	int max_battle_add_intimacy;
};


class FriendConfig : public ILogicConfig
{
public:
	FriendConfig();
	virtual ~FriendConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	
	const FriendOtherConfig &  GetOtherCfg();
private:
	int InitOtherCfg(TiXmlElement *RootElement);

	FriendOtherConfig m_other_cfg;
};

#endif 