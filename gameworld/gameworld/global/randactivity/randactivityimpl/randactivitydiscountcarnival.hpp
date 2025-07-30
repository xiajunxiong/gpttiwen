#ifndef __RAND_ACTIVITY_DISCOUNT_CARNIVAL_HPP__
#define __RAND_ACTIVITY_DISCOUNT_CARNIVAL_HPP__

#include "global/randactivity/randactivity.hpp"
#include <map>
#include <vector>

class RandActivityDiscountCarnival : public RandActivity
{
public:
	RandActivityDiscountCarnival(RandActivityManager *activity_manager);
	~RandActivityDiscountCarnival();
	//virtual bool CanSyncActivityDataToCross() { return false; }
private:

};

#endif