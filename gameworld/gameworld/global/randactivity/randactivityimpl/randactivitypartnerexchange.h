#ifndef __RANDACTIVITY_PARTNET_EXCHANGE_H__
#define __RANDACTIVITY_PARTNET_EXCHANGE_H__


#include "global/randactivity/randactivity.hpp"

class RandActivityPartnerExchange : public RandActivity
{
public:
	RandActivityPartnerExchange(RandActivityManager *activity_manager);
	virtual ~RandActivityPartnerExchange();
	//virtual bool CanSyncActivityDataToCross() { return false; }
};

#endif