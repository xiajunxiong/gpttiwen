#ifndef __RA_HELP_ZE_SLOT_MACHINE_HPP__
#define __RA_HELP_ZE_SLOT_MACHINE_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)

static const int YUN_ZE_SLOT_NACHINE_LOCK_REWARD_TYPE = 3;

struct RAYunZeSlotMachineParam
{
	RAYunZeSlotMachineParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		surplus_key = 0;
		buy_times = 0;

		memset(lock_reward,0,sizeof(lock_reward));
	}

	unsigned int ra_begin_timestamp;
	short surplus_key;
	short buy_times;
	int lock_reward[YUN_ZE_SLOT_NACHINE_LOCK_REWARD_TYPE];
};
UNSTD_STATIC_CHECK(sizeof(RAYunZeSlotMachineParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)

#endif