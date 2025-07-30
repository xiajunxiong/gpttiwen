#ifndef __SPECIAL_LOGIC_SAN_XIAN_FB_HPP__
#define __SPECIAL_LOGIC_SAN_XIAN_FB_HPP__

#include "scene/speciallogic.hpp"
#include "protocol/msgscene.h"
#include "servercommon/taskdef.h"
#include <vector>
#include <map>

class SpecialLogicSanXianFb : public SpecialLogic
{
public:
	SpecialLogicSanXianFb(Scene * scene);
	virtual ~SpecialLogicSanXianFb();
	virtual bool CanDestroy() { return true; }
private:
};

#endif