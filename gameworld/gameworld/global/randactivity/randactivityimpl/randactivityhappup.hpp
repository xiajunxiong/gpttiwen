#ifndef __RAND_ACTIVITY_HAPPY_UP_HPP__
#define __RAND_ACTIVITY_HAPPY_UP_HPP__

#include "global/randactivity/randactivity.hpp"

class RandActivityHappyUp : public RandActivity
{
public:
	RandActivityHappyUp(RandActivityManager *activity_manager);
	~RandActivityHappyUp();
	//virtual bool CanSyncActivityDataToCross() { return false; }
};




#endif