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
	XIU_WEI_DAN_TYPE_NORMAL = 0,			// 普通修为丹
	XIU_WEI_DAN_TYPE_HIGH = 1,				// 高级修为丹
	XIU_WEI_DAN_TYPE_MAX,
};

enum XIU_WEI_TYPE
{
	XIU_WEI_TYPE_ROLE_ATK = 0,			// 人物攻修
	XIU_WEI_TYPE_ROLE_DEF = 1,			// 人物防修
	XIU_WEI_TYPE_PET_ATK = 2,			// 宠物攻修
	XIU_WEI_TYPE_PET_DEF = 3,			// 宠物防修
	XIU_WEI_TYPE_PARTNER_ATK = 4,		// 伙伴攻修
	XIU_WEI_TYPE_PARTNER_DEF = 5,		// 伙伴防修
	XIU_WEI_TYPE_MAX,
};

/////////////  修为

struct XiuWeiInfo
{
	XiuWeiInfo() { this->Reset(); }
	void Reset()
	{
		level = 0;
		reserve_sh = 0;
		exp = 0;
	}

	short level;								// 等级
	short reserve_sh;							// 备用
	int exp;									// 经验
};

struct XiuWeiParam								//职业晋阶
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

	int normal_dan_day_times;					// 低级丹药每日用次数
	int high_dan_day_times;						// 高级丹药每日使用次数
	XiuWeiInfo info_arr[XIU_WEI_TYPE_MAX];
};

typedef char XiuWeiParamHex[sizeof(XiuWeiParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(XiuWeiParamHex) < 512);

#pragma pack(pop)

#endif	// __XIU_WEI_PARAM_HPP__
