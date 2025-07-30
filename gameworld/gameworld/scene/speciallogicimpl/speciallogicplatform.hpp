#ifndef __SPECIAL_LOGIC_PLATFORM_BATTLE_HPP__
#define __SPECIAL_LOGIC_PLATFORM_BATTLE_HPP__

#include "scene/speciallogic.hpp"

class SpecialLogicPlatformBattle : public SpecialLogic
{
public:
	SpecialLogicPlatformBattle(Scene * scene);
	virtual ~SpecialLogicPlatformBattle();

	virtual bool CanDestroy() { return false; }
private:

};

#endif