#include "randactivitymazesummer.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "gameworld/gamelog.h"

RandActivityMazeSummer::RandActivityMazeSummer(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_MAZE_SUMMER)
{
}

RandActivityMazeSummer::~RandActivityMazeSummer()
{
}