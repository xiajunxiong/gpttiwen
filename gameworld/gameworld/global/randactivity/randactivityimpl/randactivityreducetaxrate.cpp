#include "randactivityreducetaxrate.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"

RandActivityReduceTaxRate::RandActivityReduceTaxRate(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_REDUCE_TAX_RATE)
{
}

RandActivityReduceTaxRate::~RandActivityReduceTaxRate()
{
}
