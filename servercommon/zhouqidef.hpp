#ifndef __ZHOU_QI_DEF_HPP__
#define __ZHOU_QI_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

const static int DAYS_PER_ZHOU_QI = 10;
const static int SECOND_PER_ZHOU_QI = DAYS_PER_ZHOU_QI * SECOND_PER_DAY;
const static int ZHOU_QI_MAX = 2;

enum ZHOU_QI_PLAY_TYPE
{
	ZHOU_QI_PLAY_TYPE_INVAILD = 0,	
	ZHOU_QI_PLAY_TYPE_QING_YUAN_DUI_DUI_PENG = 1,		//情缘对对碰
	ZHOU_QI_PLAY_TYPE_WHERE_IS_THE_FAIRY = 2,			//仙子去哪儿
	ZHOU_QI_PLAY_TYPE_CONCENTRIC_KNOT = 3,				//缘牵同心结
	ZHOU_QI_PLAY_TYPE_SAN_XIAN_GUI_DONG = 4,			//三仙归洞
	ZHOU_QI_PLAY_TYPE_TAO_QUAN_NA_JI = 5,				//套圈纳吉
	ZHOU_QI_PLAY_TYPE_LANTERN_RIDDIE = 6,				//山海灯会
	ZHOU_QI_PLAY_TYPE_TIAN_GONG_XUN_BAO = 7,			//天宫寻宝
	ZHOU_QI_PLAY_TYPE_TU_ZI_KUAI_PAO = 8,			//兔子快跑

	ZHOU_QI_PLAY_TYPE_MAX
};

#pragma pack(pop)

#endif // !__BABY_DEF_HPP__
