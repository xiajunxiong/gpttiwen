#ifndef __RA_GIVE_PET_DRAW_PARAM_HPP__
#define __RA_GIVE_PET_DRAW_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

///////////////////////////////////// 2090 随机活动 - 赠30连抽（送寻宠连抽）  /////////////////////////////////

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  /////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
struct RAGivePetDrawParam
{
	RAGivePetDrawParam() { this->Reset(); }
	void Reset()
	{
		begin_time = 0;
		give_mark = 0;
	}

	unsigned int begin_time;
	int give_mark;
};

UNSTD_STATIC_CHECK(sizeof(RAGivePetDrawParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_GIVE_PET_DRAW_PARAM_HPP__
