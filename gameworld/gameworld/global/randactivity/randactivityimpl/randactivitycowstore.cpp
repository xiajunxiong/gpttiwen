#include "randactivitycowstore.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "gameworld/gamelog.h"

RandActivityCowStore::RandActivityCowStore(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_COW_STORE)
{
}

RandActivityCowStore::~RandActivityCowStore()
{
}