#ifndef __ROLE_HUAN_JIE_DEF_H__
#define __ROLE_HUAN_JIE_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/commonsavedef.h"

#pragma pack(push, 4)

const int static HUAN_JIE_TIAN_TI_RECORD_MAX_NUM = 20; 

struct RoleHuanJieParam
{
	RoleHuanJieParam()
	{
		this->Reset();
	}

	void Reset()
	{
		tian_ti_record_index = 0;
		for (int i = 0; i < ARRAY_LENGTH(tian_ti_record); ++i)
		{
			tian_ti_record[i].Reset();			
		}
	}

	unsigned int time;
	int tian_ti_record_index;
	CommonSaveHuanJieTianTiReceordData tian_ti_record[HUAN_JIE_TIAN_TI_RECORD_MAX_NUM];
};

UNSTD_STATIC_CHECK(sizeof(CommonSaveHuanJieTianTiReceordData) == 612);

typedef char RoleHuanJieParamHex[sizeof(RoleHuanJieParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleHuanJieParamHex) < 25000);


#pragma pack(pop)

#endif
