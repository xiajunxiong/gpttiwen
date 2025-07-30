#ifndef __RANDACTIVITY_HERITAGE_TREASURE_H__
#define __RANDACTIVITY_HERITAGE_TREASURE_H__


#include "global/randactivity/randactivity.hpp"

class RandActivityHeritageTreasure : public RandActivity
{
public:
	RandActivityHeritageTreasure(RandActivityManager *activity_manager);
	virtual ~RandActivityHeritageTreasure();
	//virtual bool CanSyncActivityDataToCross() { return false; }
};

#endif