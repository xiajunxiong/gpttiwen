#ifndef __SPECIAL_LOGIC_GUAJI_HPP__
#define __SPECIAL_LOGIC_GUAJI_HPP__

#include "scene/speciallogic.hpp"

class SpecialLogicGuaji : public SpecialLogic
{
public:
	SpecialLogicGuaji(Scene* scene);
	virtual ~SpecialLogicGuaji();

	virtual bool CanDestroy() { return false; }
	virtual bool IsLevelOneMonsterOccur() { return false; }
	virtual bool IsDropCard() { return false; }

private:
};

#endif