#ifndef __RA_JIAN_DAN_QIN_XIN_PARAM_HPP__
#define __RA_JIAN_DAN_QIN_XIN_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2073 随机活动 - 剑胆琴心  //////////////////////////////////////////

const static int MAX_RA_JIANDNAQINXIN_DAY_NUM = 7;					//一天开一个关卡,总共七天
const static int MAX_RA_JIANDANQINXIN_CHECKPOINT_NUM = 14;			//总共关卡为7,一个关卡又划分高低难度
const static int MAX_RA_JIANDANQINXIN_REWARD_NUM = 5;				//最大的道具奖励数量

enum JIAN_DAN_QIN_XIN_TYPE
{
	JIAN_DAN_QIN_XIN_TYPE_COMMON = 0,		//普通难度
	JIAN_DAN_QIN_XIN_TYPE_DIFFICULT = 1,	//困难难度

	JIAN_DAN_QIN_XIN_TYPE_MAX,
};
UNSTD_STATIC_CHECK(JIAN_DAN_QIN_XIN_TYPE_MAX * MAX_RA_JIANDNAQINXIN_DAY_NUM == MAX_RA_JIANDANQINXIN_CHECKPOINT_NUM)

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAJianDanQinXinParam
{
	RAJianDanQinXinParam() { this->Reset(); }
	void Reset()
	{
		ra_jiandanqinxin_begin_timestamp = 0;
		ra_jiandanqinxin_flag.Reset();
	}

	unsigned int ra_jiandanqinxin_begin_timestamp;
	//前七个为低级难度,后七个为高难度 (下标为配置seq1)
	BitMap<MAX_RA_JIANDANQINXIN_CHECKPOINT_NUM> ra_jiandanqinxin_flag;				
};

UNSTD_STATIC_CHECK(sizeof(RAJianDanQinXinParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_JIAN_DAN_QIN_XIN_PARAM_HPP__
