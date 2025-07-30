#include "randactivitycowluckyexchange.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "gameworld/gamelog.h"

RandActivityCowLuckyExchange::RandActivityCowLuckyExchange(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_COW_LUCKY_EXCHANGE)
{
}

RandActivityCowLuckyExchange::~RandActivityCowLuckyExchange()
{
}