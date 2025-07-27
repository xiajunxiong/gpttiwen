#ifndef __MEDAL_DEF_H__
#define __MEDAL_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

const static int MAX_MEDAL_WEAR_GRID = 3;
const static int MAX_MEDAL_MAX_ATTR_NUM = 4;
const static int MAX_MEDAL_MAX_GRADE = 25;  //[0,MAX_MEDAL_MAX_GRADE)
static const int MEDAL_ONE_KEY_GRADE_MAXNUM = 320;

const static int MAX_SPECIAL_MEDAL_FIRST_UPGREADE_LEVEL = 15;//�״������������
const static int MAX_SPECIAL_MEDAL_SECOND_UPGREADE_LEVEL = 24;//���������������


//---------------------�����Ǵ��ṹ��ע�����--------------------------------------
struct MedalItemAttrInfo
{
	short attr_type;					//!< ��������
	short attr_value;					//!< ������ֵ
};

enum MEDAL_LOTTERY_TIMES_TYPE
{
	MEDAL_LOTTERY_TIMES_TYPE_1 = 1,
	MEDAL_LOTTERY_TIMES_TYPE_10 = 10,
};


struct MedalCommonParam
{
	MedalCommonParam()
	{
		this->Reset();
	}

	void Reset()
	{
		reserve_sh = 0;
	}

	long long reserve_sh;		//������
};

typedef char MedalCommonParamHex[sizeof(MedalCommonParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(MedalCommonParamHex) < 128);

#pragma pack(pop)

#endif
