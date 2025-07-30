#ifndef __RANDACTIVITY_PET_EXCHANGE_H__
#define __RANDACTIVITY_PET_EXCHANGE_H__


#include "global/randactivity/randactivity.hpp"

class RandActivityPetExchange : public RandActivity
{
public:
	RandActivityPetExchange(RandActivityManager *activity_manager);
	virtual ~RandActivityPetExchange();
	//virtual bool CanSyncActivityDataToCross() { return false; }
};

#endif