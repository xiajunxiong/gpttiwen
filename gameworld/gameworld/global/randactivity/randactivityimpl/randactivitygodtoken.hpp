#ifndef __RAND_ACTIVITY_GOD_TOKEN_HPP__
#define __RAND_ACTIVITY_GOD_TOKEN_HPP__

#include "global/randactivity/randactivity.hpp"

class RandActivityGodToken : public RandActivity
{
public:
	RandActivityGodToken(RandActivityManager *activity_manager);
	~RandActivityGodToken();
	//virtual bool CanSyncActivityDataToCross() { return false; }
	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);

private:

};

#endif

