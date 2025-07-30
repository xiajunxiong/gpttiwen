#include "randactivitymonpolygift.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "gameworld/gamelog.h"

RandActivityMonpolyGift::RandActivityMonpolyGift(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_MONPOLY_GIFT)
{
}

RandActivityMonpolyGift::~RandActivityMonpolyGift()
{
}