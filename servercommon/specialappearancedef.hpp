#ifndef __SPECIAL_APPAEARANCE_DEF_HPP__
#define __SPECIAL_APPAEARANCE_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push)
#pragma pack(4)

enum SPECIAL_APPEARANCE
{
	SPECIAL_APPEARANCE_NORMAL = 0,		// 正常外观

	SPECIAL_APPEARANCE_PARTNER = 1,		// 伙伴外观 （机器人需求）

	SPECIAL_APPEARANCE_MAX
};

struct SpecialAppearanceParam
{
	SpecialAppearanceParam()
	{
		this->Reset();
	}

	void Reset()
	{
		reserve_int = 0;
	}

	int reserve_int;	// 保留字  
};
//!< \note 暂未有具体需求用于玩家  但先准备好param

typedef char SpecialAppearanceParamHex[sizeof(SpecialAppearanceParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(SpecialAppearanceParamHex) < 128);

#pragma pack(pop)

#endif