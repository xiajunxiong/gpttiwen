#include "randactivitysealpromote.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "gameworld/gamelog.h"

RandActivitySealPromote::RandActivitySealPromote(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_SEAL_PROMOTE)
{
}

RandActivitySealPromote::~RandActivitySealPromote()
{
}