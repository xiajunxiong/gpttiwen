#ifndef __RA_CHAOZHI_FENXIANG_PARAM_HPP__
#define __RA_CHAOZHI_FENXIANG_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 超值纷享 //////////////////////////////////////////
enum RA_CHAOZHI_FENXIANG_TASK_TYPE
{
	RA_CHAOZHI_FENXIANG_TASK_TYPE_LOGIN = 0,				// 任务开启后登录1天
	RA_CHAOZHI_FENXIANG_TASK_TYPE_YUNZEHUAJING = 1,			// 云泽画境 探索X次
	RA_CHAOZHI_FENXIANG_TASK_TYPE_ZAIZHANYUNZE = 2,			// 再战云泽 击败首领X次
	RA_CHAOZHI_FENXIANG_TASK_TYPE_YUNZE_23 = 3,				// 云泽二三事 完成X次
	RA_CHAOZHI_FENXIANG_TASK_TYPE_YUNZEFANHUALU = 4,		// 云泽繁花录 获得X积分
	RA_CHAOZHI_FENXIANG_TASK_TYPE_MOZUZAINALI = 5,			// 魔族在哪里 击败X次魔族
	RA_CHAOZHI_FENXIANG_TASK_TYPE_JINGLING_DAZHUABU = 6,	// 精灵大抓捕 抓捕次数

	RA_CHAOZHI_FENXIANG_TASK_TYPE_MAX
};

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAChaoZhiFenXiangParam
{
	RAChaoZhiFenXiangParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		task_fetch_mark = 0;
		zhigou_mark = 0;

		memset(task_comple, 0, sizeof(task_comple));
		re_sh = 0;
	}

	unsigned int ra_begin_timestamp;
	short task_fetch_mark;
	short zhigou_mark;

	short task_comple[RA_CHAOZHI_FENXIANG_TASK_TYPE_MAX];
	short re_sh;
};

UNSTD_STATIC_CHECK(sizeof(RAChaoZhiFenXiangParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)

#endif	//__RA_CHAOZHI_FENXIANG_PARAM_HPP__
