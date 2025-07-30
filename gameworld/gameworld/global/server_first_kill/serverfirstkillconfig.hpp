#ifndef __SERVER_FIRST_KILL_CONFIG_HPP__
#define __SERVER_FIRST_KILL_CONFIG_HPP__

#include <map>

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"


struct ServerFirstKillCfg
{
	ServerFirstKillCfg():seq(0), type(0), type_seq(0), level_group(0), monster_group_id(0), coin(0)
	{}

	int seq;
	int type;
	int type_seq;
	int level_group;
	int monster_group_id;
	ItemConfigData reward_item;
	int coin;
};


class ServerFirstKillConfig :public ILogicConfig
{
public:
	ServerFirstKillConfig();
	virtual ~ServerFirstKillConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ServerFirstKillCfg* GetServerFirstKillCfg(int monster_group_id);

private:
	int InitServerFirstKillCfg(TiXmlElement *RootElement);

	
	std::map<int, ServerFirstKillCfg> m_server_first_kill;				//ÒÔmonster_group_idÎª¼ü 

};


#endif