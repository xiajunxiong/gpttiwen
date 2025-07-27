#ifndef __RA_XUNMENG_HUDIE_PARAM_HPP__
#define __RA_XUNMENG_HUDIE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// Ѱ�κ��� //////////////////////////////////////////


#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////


struct RAXunMengHuDieParam
{
	RAXunMengHuDieParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		ra_frequency_num = 0;
	}

	unsigned int ra_begin_timestamp;
	int ra_frequency_num;				// ÿ�մ���
};

UNSTD_STATIC_CHECK(sizeof(RAXunMengHuDieParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_XUNMENG_HUDIE_PARAM_HPP__
