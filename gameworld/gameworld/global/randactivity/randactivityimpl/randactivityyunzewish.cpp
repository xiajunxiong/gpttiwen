#include "randactivityyunzewish.hpp"
#include "config/randactivityconfig/impl/randactivityyunzewishconfig.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/uniqueidgenerator.hpp"

RandActivityYunZeWish::RandActivityYunZeWish(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_YUN_ZE_WISH), next_refresh_timestamp(0)
{
}

RandActivityYunZeWish::~RandActivityYunZeWish()
{
}

void RandActivityYunZeWish::Update(unsigned long interval, time_t now_second)
{
	RandActivity::Update(interval, now_second);

	if (m_rand_activity_manager->GetRandActivityBeginTime(RAND_ACTIVITY_TYPE_YUN_ZE_WISH) == 0)
	{
		return;
	}

	if (this->GetStatus() == ACTIVITY_STATUS_OPEN && now_second >= next_refresh_timestamp)
	{
		const RandActivityYunZeWishConfig * config = static_cast<const RandActivityYunZeWishConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_YUN_ZE_WISH));
		if (NULL == config)
		{
			return;
		}

		next_refresh_timestamp = m_rand_activity_manager->GetRandActivityBeginTime(RAND_ACTIVITY_TYPE_YUN_ZE_WISH);		
		next_refresh_timestamp = next_refresh_timestamp - ((next_refresh_timestamp + 8 * 3600) % 86400);

		while (now_second >= next_refresh_timestamp)
		{
			next_refresh_timestamp += config->GetOtherCfg().time * 3600;
		}

	}

}
