#include "randactivitygivepetdraw.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "gameworld/gamelog.h"

RandActivityGivePetDraw::RandActivityGivePetDraw(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_GIVE_PET_DRAW)
{
}

RandActivityGivePetDraw::~RandActivityGivePetDraw()
{
}