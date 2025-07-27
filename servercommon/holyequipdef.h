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

	long long reserve_sh;		//������

	int first_gain;			// ��һ�λ��ʱָ��flag
	int follow_index;		// ����ʥ��index(�ͻ��˼�¼��Ҫ), -1��ʾû�и���ʥ��
};
typedef char HolyEquipCommonParamHex[sizeof(HolyEquipCommonParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(HolyEquipCommonParamHex) < 128);

#pragma pack(pop)

#endif
