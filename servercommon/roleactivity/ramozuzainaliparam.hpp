#ifndef __RA_MO_ZU_ZAI_NA_LI_PARAM_HPP__
#define __RA_MO_ZU_ZAI_NA_LI_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// ħ�������� //////////////////////////////////////////

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RAMoZuZaiNaLiParam
{
	RAMoZuZaiNaLiParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		ra_frequency_num = 0;
	}

	unsigned int ra_begin_timestamp;
	int ra_frequency_num;								// ��ħ����
};

UNSTD_STATIC_CHECK(sizeof(RAMoZuZaiNaLiParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)

#endif	//__RA_MO_ZU_ZAI_NA_LI_PARAM_HPP__
