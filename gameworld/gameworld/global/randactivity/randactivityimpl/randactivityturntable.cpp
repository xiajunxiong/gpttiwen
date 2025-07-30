#include "randactivityturntable.h"
#include "world.h"
#include "scene/scenemanager.hpp"

RandActivityTurnTable::RandActivityTurnTable(RandActivityManager * activity_manager)
	:RandActivity(activity_manager,RAND_ACTIVITY_TYPE_TRUN_TABLE)
{
}

RandActivityTurnTable::~RandActivityTurnTable()
{
}
