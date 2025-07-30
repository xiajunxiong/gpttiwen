#include "randactivityhappup.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"

RandActivityHappyUp::RandActivityHappyUp(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_HAPPY_UP)
{
}

RandActivityHappyUp::~RandActivityHappyUp()
{
}
