#ifndef _ROLE_GIVE_GIFT_CONFIG_HPP_
#define _ROLE_GIVE_GIFT_CONFIG_HPP_

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"

#include <vector>
#include <map>

struct RoleGiveGiftCfg
{
	RoleGiveGiftCfg() : times_group_id(0), item_id(0)
	{}

	int times_group_id;
	int item_id;
};

struct RoleGiveTimesCfg
{
	RoleGiveTimesCfg() : times_group_id(0), give_times(0), is_day_return(0)
	{}

	int times_group_id;
	int give_times;
	bool is_day_return; 
};


class RoleGiveGiftConfig : public ILogicConfig
{
public:
	RoleGiveGiftConfig();
	~RoleGiveGiftConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const RoleGiveGiftCfg * GetRoleGiveGiftCfg(int item_id); 
	const RoleGiveTimesCfg * GetRoleGiveTimesCfg(int times_group_id);
private:
	int InitRoleGiveGiftCfg(TiXmlElement * RootElement);
	int InitRoleGiveTimesCfg(TiXmlElement * RootElement);

	std::map<int, RoleGiveGiftCfg> m_give_gift_cfg;
	std::map<int, RoleGiveTimesCfg> m_give_times_cfg;
};

#endif

