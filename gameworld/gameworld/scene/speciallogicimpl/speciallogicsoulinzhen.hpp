#ifndef __SPECIAL_LOGIC_SOUL_IN_ZHEN_HPP__
#define __SPECIAL_LOGIC_SOUL_IN_ZHEN_HPP__

#include "scene/speciallogic.hpp"

class SpecialLogicSoulInZhen : public SpecialLogic
{
public:
	SpecialLogicSoulInZhen(Scene* scene);
	virtual ~SpecialLogicSoulInZhen();

	virtual bool CanDestroy() { return true; }
private:
};

#endif