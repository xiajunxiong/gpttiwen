#ifndef __HOLY_EQUIP_DEF_H__
#define __HOLY_EQUIP_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

struct HolyEquipCommonParam
{
	HolyEquipCommonParam()
	{
		this->Reset();
	}

	void Reset()
	{
		reserve_sh = 0;
		first_gain = 0;
		follow_index = -1;
	}

	long long reserve_sh;		//保留字

	int first_gain;			// 第一次获得时指引flag
	int follow_index;		// 跟随圣器index(客户端记录需要), -1表示没有跟随圣器
};
typedef char HolyEquipCommonParamHex[sizeof(HolyEquipCommonParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(HolyEquipCommonParamHex) < 128);

#pragma pack(pop)

#endif
