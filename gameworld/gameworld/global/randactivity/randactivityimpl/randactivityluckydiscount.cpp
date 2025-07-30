#include "randactivityluckydiscount.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "gameworld/gamelog.h"

RandActivityLuckyDiscount::RandActivityLuckyDiscount(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_LUCKY_DISCOUNT)
{
}

RandActivityLuckyDiscount::~RandActivityLuckyDiscount()
{
}