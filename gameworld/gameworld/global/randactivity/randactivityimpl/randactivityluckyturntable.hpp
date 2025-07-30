#ifndef __RAND_ACTIVITY_LUCKY_TURNTABLE_HPP__
#define __RAND_ACTIVITY_LUCKY_TURNTABLE_HPP__

#include "global/randactivity/randactivity.hpp"

class RandActivityLuckyTable : public RandActivity
{
public:
	RandActivityLuckyTable(RandActivityManager *activity_manager);
	~RandActivityLuckyTable();
	//virtual bool CanSyncActivityDataToCross() { return false; }
};

#endif
