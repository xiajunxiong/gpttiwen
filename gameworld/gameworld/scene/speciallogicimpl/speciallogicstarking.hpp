#ifndef __SPECIAL_LOGIC_STAR_KING_FB_HPP__
#define __SPECIAL_LOGIC_STAR_KING_FB_HPP__

#include "scene/speciallogic.hpp"
#include "protocol/msgscene.h"
#include "servercommon/taskdef.h"
#include <vector>
#include <map>

class SpecialLogicStarKingFb : public SpecialLogic
{
public:
	SpecialLogicStarKingFb(Scene * scene);
	virtual ~SpecialLogicStarKingFb();
	virtual bool CanDestroy() { return true; }
private:
};

#endif