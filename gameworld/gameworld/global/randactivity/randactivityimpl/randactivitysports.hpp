#ifndef __RAND_ACTIVITY_SPORTS_HPP__
#define __RAND_ACTIVITY_SPORTS_HPP__

#include "global/randactivity/randactivity.hpp"

class RandAcitivitySports : public RandActivity
{
public:
	RandAcitivitySports(RandActivityManager *activity_manager);
	~RandAcitivitySports();
	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);
};


#endif