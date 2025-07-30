#include "unittest/catch.hpp"
#include "protocol/msgcloudarena.h"
#include "servercommon/cloudarenadef.hpp"

TEST_CASE("Test CloudArena Formation Req CheckDuplicate", "[CheckDuplicate]")
{
	Protocol::CSCloudArenaSetFormation req;
	memset(req.grid_list, 0, sizeof(req.grid_list));
	memset(req.standby_list, 0, sizeof(req.standby_list));
	req.grid_list[2].id = UidToInt(UserID(1, 1));
	req.grid_list[2].type = Protocol::CSCloudArenaSetFormation::FORMATION_GRID_ROLE;

	SECTION("grid_list pet duplicate") {
		req.grid_list[0].id = 50120;
		req.grid_list[0].unique_id = 12345;
		req.grid_list[0].type = Protocol::CSCloudArenaSetFormation::FORMATION_GRID_PET;

		req.grid_list[1].id = 50120;
		req.grid_list[1].unique_id = 12345;
		req.grid_list[1].type = Protocol::CSCloudArenaSetFormation::FORMATION_GRID_PET;

		CHECK_FALSE(req.CheckDuplicate());
	}

	SECTION("grid_list partner duplicate") {
		req.grid_list[0].id = 1;
		req.grid_list[0].type = Protocol::CSCloudArenaSetFormation::FORMATION_GRID_PARTNER;

		req.grid_list[1].id = 1;
		req.grid_list[1].type = Protocol::CSCloudArenaSetFormation::FORMATION_GRID_PARTNER;

		CHECK_FALSE(req.CheckDuplicate());
	}

	SECTION("grid_list pet duplicate with standby_list") {
		req.grid_list[0].id = 50120;
		req.grid_list[0].unique_id = 12345;
		req.grid_list[0].type = Protocol::CSCloudArenaSetFormation::FORMATION_GRID_PET;

		req.grid_list[1].id = 50121;
		req.grid_list[1].unique_id = 12345;
		req.grid_list[1].type = Protocol::CSCloudArenaSetFormation::FORMATION_GRID_PET;

		CHECK(req.CheckDuplicate());

		req.grid_list[3].id = 50120;
		req.grid_list[3].unique_id = 12345678;
		req.grid_list[3].type = Protocol::CSCloudArenaSetFormation::FORMATION_GRID_PET;

		CHECK(req.CheckDuplicate());

		req.standby_list[0].pet_id = 50120;
		req.standby_list[0].unique_id = 12345678;
		CHECK_FALSE(req.CheckDuplicate());

		req.standby_list[0].unique_id = 12345;
		CHECK_FALSE(req.CheckDuplicate());

		req.standby_list[0].unique_id = 11111;
		CHECK(req.CheckDuplicate());
	}

	SECTION("standby_list duplicate") {
		req.standby_list[0].pet_id = 50120;
		req.standby_list[0].unique_id = 12345;

		req.standby_list[1].pet_id = 50123;
		req.standby_list[1].unique_id = 12345678;

		req.standby_list[2].pet_id = 50120;
		req.standby_list[2].unique_id = 11111;

		req.standby_list[3].pet_id = 50123;
		req.standby_list[3].unique_id = 22222;

		CHECK(req.CheckDuplicate());

		req.standby_list[4].pet_id = 50123;
		req.standby_list[4].unique_id = 12345678;

		CHECK_FALSE(req.CheckDuplicate());

		UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(req.standby_list) == 5);
	}
}