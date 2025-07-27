#ifndef __RA_JOY_SEEKS_FAVORS_PARAM_HPP__
#define __RA_JOY_SEEKS_FAVORS_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2067 ���� - ����Ѱ��  //////////////////////////////////////////

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RAJoySeeksFavorsParam
{
	RAJoySeeksFavorsParam() { this->Reset(); }
	void Reset()
	{
		ra_joy_seeks_favors_begin_timestamp = 0;
		ra_joy_seeks_favors_role_level = 0;
	}

	unsigned int ra_joy_seeks_favors_begin_timestamp;
	int ra_joy_seeks_favors_role_level;//�����ʱ��ҵȼ�
};

UNSTD_STATIC_CHECK(sizeof(RAJoySeeksFavorsParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_JOY_SEEKS_FAVORS_PARAM_HPP__
