#ifndef __RA_DROP_DOUBLE_PARAM_HPP__
#define __RA_DROP_DOUBLE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2066 ����� - ���䷭��  //////////////////////////////////////////


#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RADropDoubleParam
{
	RADropDoubleParam() { this->Reset(); }
	void Reset()
	{
		ra_drop_double_begin_timestamp = 0;
	}

	unsigned int ra_drop_double_begin_timestamp;

};

UNSTD_STATIC_CHECK(sizeof(RADropDoubleParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_DROP_DOUBLE_PARAM_HPP__
