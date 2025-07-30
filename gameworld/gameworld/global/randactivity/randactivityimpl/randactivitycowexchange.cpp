#include "randactivitycowexchange.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "gameworld/gamelog.h"

RandActivityCowExchange::RandActivityCowExchange(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_COW_EXCHANGE)
{
}

RandActivityCowExchange::~RandActivityCowExchange()
{
}