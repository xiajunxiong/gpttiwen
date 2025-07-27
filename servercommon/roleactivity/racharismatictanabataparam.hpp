#ifndef __RA_CHARISMATIC_TANABATA_PARAM_HPP__
#define __RA_CHARISMATIC_TANABATA_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

const static int MAX_CHARIS_FLAG = 32;//���������

struct RACharismaticTanabataParam
{
	RACharismaticTanabataParam() { this->Reset(); }

	void Reset()
	{
		open_day = 0;
		admiration_value = 0;
		admiration_map.Reset();
		charm_value = 0;
		charm_map.Reset();
		begin_time = 0;
	}

	int open_day;

	int admiration_value;//��Ľֵ
	BitMap<MAX_CHARIS_FLAG> admiration_map;
	int charm_value;//����ֵ
	BitMap<MAX_CHARIS_FLAG> charm_map;

	unsigned int begin_time;
};

UNSTD_STATIC_CHECK(sizeof(RACharismaticTanabataParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_DISCOUNT_STORE_PARAM_HPP__
