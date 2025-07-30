#ifndef __RAND_ACTIVITY_YUN_ZE_WISH_HPP__
#define __RAND_ACTIVITY_YUN_ZE_WISH_HPP__

#include "global/randactivity/randactivity.hpp"

class RandActivityYunZeWish :public RandActivity
{
public:
	RandActivityYunZeWish(RandActivityManager *activity_manager);
	~RandActivityYunZeWish();

	virtual void Update(unsigned long interval, time_t now_second);

	unsigned int GetNextTimeStamp()const { return next_refresh_timestamp; }

private:

	unsigned int next_refresh_timestamp;
};


#endif