#include "randactivitysaleshop.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"

RandActivitySaleShop::RandActivitySaleShop(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_SALE_SHOP)
{
}

RandActivitySaleShop::~RandActivitySaleShop()
{
}
