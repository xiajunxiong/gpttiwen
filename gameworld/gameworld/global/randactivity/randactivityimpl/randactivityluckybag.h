#ifndef __RANDACTIVITY_LUCKY_BAG_H__
#define __RANDACTIVITY_LUCKY_BAG_H__


#include "global/randactivity/randactivity.hpp"

class RandActivityLuckyBag : public RandActivity
{
public:
	RandActivityLuckyBag(RandActivityManager *activity_manager);
	virtual ~RandActivityLuckyBag();
};

#endif