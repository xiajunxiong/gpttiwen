#ifndef __ADVANCE_CAREER_PARAM_HPP__
#define __ADVANCE_CAREER_PARAM_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#pragma pack(push, 4)

static const int MAX_ADVANCE_CAREER_SUB_COUNT = 9;	// 职业晋阶子阶数

/////////////  晋阶

struct AdvanceCareerParam								//职业晋阶
{
	AdvanceCareerParam()
	{
		this->Reset();
	}

	void Reset()
	{
		is_history_data = 0;
		sub_level = 0;
		memset(cond_complete_flag, 0, sizeof(cond_complete_flag));
	}

	int is_history_data;												// 是否对历史数据做过统计
	int sub_level;														// 当前子阶级
	int cond_complete_flag[PROFESSION_ADVANCE_TIMES + 1];				// 每阶的完成条件否完成的记录[按位]

};

typedef char AdvanceCareerParamHex[sizeof(AdvanceCareerParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(AdvanceCareerParamHex) < 128);


#pragma pack(pop)

#endif	// __ADVANCE_CAREER_PARAM_HPP__
