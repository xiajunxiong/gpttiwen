#include "randactivitydropdouble.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "gameworld/gamelog.h"

RandActivityDropDouble::RandActivityDropDouble(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_DROP_DOUBLE)
{
}

RandActivityDropDouble::~RandActivityDropDouble()
{
}