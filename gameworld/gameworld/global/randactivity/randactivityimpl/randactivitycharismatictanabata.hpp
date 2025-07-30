#ifndef __RAND_ACTIVITY_CHARISMATIC_TANABATA_HPP
#define __RAND_ACTIVITY_CHARISMATIC_TANABATA_HPP

#include "servercommon/activitydef.hpp"
#include "global/randactivity/randactivity.hpp"

class RandActivityCharismaticTanabata : public RandActivity
{
public:
	RandActivityCharismaticTanabata(RandActivityManager *activity_manager);
	~RandActivityCharismaticTanabata();
	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);
};


#endif