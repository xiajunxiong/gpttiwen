#include "randactivitydiscountcarnival.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "gameworld/gamelog.h"

RandActivityDiscountCarnival::RandActivityDiscountCarnival(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_DISCOUNT_CARNIVAL)
{
}

RandActivityDiscountCarnival::~RandActivityDiscountCarnival()
{
}