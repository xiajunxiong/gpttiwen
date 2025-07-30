#ifndef __RAND_ACTIVITY_GIVE_SECRET_KEY_HPP__
#define __RAND_ACTIVITY_GIVE_SECRET_KEY_HPP__

#include "global/randactivity/randactivity.hpp"
#include <vector>

class RandAcitivityGiveSecretKey : public RandActivity
{
public:
	RandAcitivityGiveSecretKey(RandActivityManager *activity_manager);
	~RandAcitivityGiveSecretKey();
};


#endif