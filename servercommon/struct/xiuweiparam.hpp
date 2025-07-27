#ifndef __XIU_WEI_PARAM_HPP__
#define __XIU_WEI_PARAM_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#pragma pack(push, 4)

enum XIU_WEI_CHARACTER_TYPE
{
	XIU_WEI_CHARACTER_TYPE_ROLE = 0,
	XIU_WEI_CHARACTER_TYPE_PET = 1,
	XIU_WEI_CHARACTER_TYPE_PARTNER = 2,

	XIU_WEI_CHARACTER_TYPE_MAX
};

enum XIU_WEI_DAN_TYPE
{
	XIU_WEI_DAN_TYPE_NORMAL = 0,			// ��ͨ��Ϊ��
	XIU_WEI_DAN_TYPE_HIGH = 1,				// �߼���Ϊ��
	XIU_WEI_DAN_TYPE_MAX,
};

enum XIU_WEI_TYPE
{
	XIU_WEI_TYPE_ROLE_ATK = 0,			// ���﹥��
	XIU_WEI_TYPE_ROLE_DEF = 1,			// �������
	XIU_WEI_TYPE_PET_ATK = 2,			// ���﹥��
	XIU_WEI_TYPE_PET_DEF = 3,			// �������
	XIU_WEI_TYPE_PARTNER_ATK = 4,		// ��鹥��
	XIU_WEI_TYPE_PARTNER_DEF = 5,		// ������
	XIU_WEI_TYPE_MAX,
};

/////////////  ��Ϊ

struct XiuWeiInfo
{
	XiuWeiInfo() { this->Reset(); }
	void Reset()
	{
		level = 0;
		reserve_sh = 0;
		exp = 0;
	}

	short level;								// �ȼ�
	short reserve_sh;							// ����
	int exp;									// ����
};

struct XiuWeiParam								//ְҵ����
{
	XiuWeiParam()
	{
		this->Reset();
	}

	void Reset()
	{
		normal_dan_day_times = 0;
		high_dan_day_times = 0;
		for (int i = 0; i < XIU_WEI_TYPE_MAX; ++i)
		{
			info_arr[i].Reset();
		}
	}

	int normal_dan_day_times;					// �ͼ���ҩÿ���ô���
	int high_dan_day_times;						// �߼���ҩÿ��ʹ�ô���
	XiuWeiInfo info_arr[XIU_WEI_TYPE_MAX];
};

typedef char XiuWeiParamHex[sizeof(XiuWeiParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(XiuWeiParamHex) < 512);

#pragma pack(pop)

#endif	// __XIU_WEI_PARAM_HPP__
