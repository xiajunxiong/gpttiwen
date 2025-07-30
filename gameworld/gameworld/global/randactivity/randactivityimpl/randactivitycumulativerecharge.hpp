#ifndef __RAND_ACTIVITY_CUMULATIVE_CHARGE_HPP__
#define __RAND_ACTIVITY_CUMULATIVE_CHARGE_HPP__

#include "global/randactivity/randactivity.hpp"
#include <vector>

class RandAcitivityCumulativeCharge : public RandActivity
{
public:
	RandAcitivityCumulativeCharge(RandActivityManager *activity_manager);
	~RandAcitivityCumulativeCharge();
};


#endif