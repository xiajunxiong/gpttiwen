#ifndef __RAND_ACTIVITY_SEVEN_COLOR_PICTURE_HPP__
#define __RAND_ACTIVITY_SEVEN_COLOR_PICTURE_HPP__

#include "global/randactivity/randactivity.hpp"

class RandActivitySevenColorPicture : public RandActivity
{
public:
	RandActivitySevenColorPicture(RandActivityManager *activity_manager);
	~RandActivitySevenColorPicture();
	//virtual bool CanSyncActivityDataToCross() { return false; }
};

#endif