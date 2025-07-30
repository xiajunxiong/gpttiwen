#include "randactivitydiscountstore.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"


RandAcitivityDiscountStore::RandAcitivityDiscountStore(RandActivityManager * activity_manager)
	:RandActivity(activity_manager,RAND_ACTIVITY_TYPE_DISCOUNT_STORE)
{
}

RandAcitivityDiscountStore::~RandAcitivityDiscountStore()
{
}
