#ifndef __LEVEL_COMPLEMENT_DEF_HPP__
#define __LEVEL_COMPLEMENT_DEF_HPP__

#include "servercommon/servercommon.h"

static const int MAX_LEVEL_COMPLEMENT_RANK_NUM = 50;	//最多取本服前50名
static const int MAX_LEVEL_COMPLEMENT_MULTIPLE_NUM = 3;	

enum LEVEL_COMPLEMENT_MULTIPLE_TYPE
{
	LEVEL_COMPLEMENT_MULTIPLE_TYPE_0 = 0,		//正常获取倍率
	LEVEL_COMPLEMENT_MULTIPLE_TYPE_1 = 1,		//第一天没做,第二天增加的倍率
	LEVEL_COMPLEMENT_MULTIPLE_TYPE_2 = 2,		//前二天没做,第三天增加的倍率

	LEVEL_COMPLEMENT_MULTIPLE_TYPE_MAX
};

UNSTD_STATIC_CHECK(MAX_LEVEL_COMPLEMENT_MULTIPLE_NUM >= LEVEL_COMPLEMENT_MULTIPLE_TYPE_MAX);

#pragma pack(push, 4)

struct RoleNewLevelComplementParam
{
	RoleNewLevelComplementParam() { this->Reset(); }

	void Reset()
	{
		ArrayFillZero(reserve_ll);
		accumulation_exp = 0;
		day_supplement_exp = 0;
		today_last_get_daily_exp_timestamp = 0;
		last_get_daily_exp_timestamp = 0;
		day_buy_exp_count = 0;
		is_first_achieve_fun_open_flag = 0;
		reserve_ch = 0;
	}
	
	long long reserve_ll[3];
	long long accumulation_exp;							//累积经验
	long long day_supplement_exp;						//当天凌晨补充的经验数值
	unsigned int today_last_get_daily_exp_timestamp;	//上次获得日常经验时间戳(当天)
	unsigned int last_get_daily_exp_timestamp;			//上次获得日常经验时间戳(不是当天)
	short day_buy_exp_count;							//当天购买经验次数
	char is_first_achieve_fun_open_flag;				//是否是首次达到该功能开启  0:是 1:不是
	char reserve_ch;

};
typedef char RoleNewLevelComplementParamHex[sizeof(RoleNewLevelComplementParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleNewLevelComplementParamHex) < 256);


#pragma pack(pop)

#endif