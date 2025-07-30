#ifndef __SPECIAL_LOGIC_GUILD_STATION_HPP__
#define __SPECIAL_LOGIC_GUILD_STATION_HPP__

#include "scene/speciallogic.hpp"

class SpecialLogicGuildStation : public SpecialLogic
{
public:
	SpecialLogicGuildStation(Scene * scene);		// 家族驻地，场景ID 即为 家族ID
	virtual ~SpecialLogicGuildStation();

	virtual bool CanDestroy();
	virtual void OnRoleEnterScene(Role *role);
};

#endif