#include "unittest/catch.hpp"
#include "other/rolemodulemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/shop/shopconfig.h"

TEST_CASE("Test Calculation of Cloud Arena New Shop Open Timestamp", "[Check Timestamp]")
{
	unsigned int config_time = LOGIC_CONFIG->GetShopConfig()->GetShopOtherCfg().cloud_arena_new_shop_timestamp;
	CHECK(1627833600 == config_time);

	SECTION("Season Ends at 2021-08-02 00:00:00, \
				New Shop Opens at 2021-08-02 00:00:00") {
		unsigned int cur_season_start_timestamp = 1626624000;
		unsigned int cur_season_end_timestamp = 1627833600;
		unsigned int new_shop_time = RoleModuleManager::CalcCloudArenaNewShopOpenTimestamp(
			cur_season_start_timestamp,
			cur_season_end_timestamp,
			config_time);
		CHECK(new_shop_time == 1627833600);
	}	

	SECTION("Season Ends at 2021-08-09 00:00:00, \
				New Shop Opens at 2021-08-09 00:00:00") {
		unsigned int cur_season_start_timestamp = 1627228800;
		unsigned int cur_season_end_timestamp = 1628438400;
		unsigned int new_shop_time = RoleModuleManager::CalcCloudArenaNewShopOpenTimestamp(
			cur_season_start_timestamp,
			cur_season_end_timestamp,
			config_time);
		CHECK(new_shop_time == 1628438400);
	}
}