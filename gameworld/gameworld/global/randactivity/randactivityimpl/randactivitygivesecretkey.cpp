#include "randactivitygivesecretkey.hpp"

RandAcitivityGiveSecretKey::RandAcitivityGiveSecretKey(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_GIVE_SECRET_KEY)
{
}

RandAcitivityGiveSecretKey::~RandAcitivityGiveSecretKey()
{
}
