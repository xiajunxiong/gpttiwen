#include "randactivityluckyturntable.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"

RandActivityLuckyTable::RandActivityLuckyTable(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE)
{
}

RandActivityLuckyTable::~RandActivityLuckyTable()
{
}
