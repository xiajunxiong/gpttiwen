#include "randactivityfortunemaze.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "gameworld/gamelog.h"

RandActivityFortuneMaze::RandActivityFortuneMaze(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_FORTUNE_MAZE)
{
}

RandActivityFortuneMaze::~RandActivityFortuneMaze()
{
}