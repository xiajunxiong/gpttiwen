#ifndef __RA_TIAN_JIANG_HAO_LI_PARAM_HPP__
#define __RA_TIAN_JIANG_HAO_LI_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2064 随机活动 - 天降好礼  //////////////////////////////////////////


#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////

struct RATianJiangHaoLiRandParam
{
	RATianJiangHaoLiRandParam() { this->Reset(); }

	void Reset()
	{
		refresh_box_time = 0;
		close_box_time = 0;
		next_refresh_timestamp = 0;
	}

	unsigned int refresh_box_time;
	unsigned int close_box_time;
	unsigned int next_refresh_timestamp;
};

UNSTD_STATIC_CHECK(sizeof(RATianJiangHaoLiRandParam) <= sizeof(RandActivityData));

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RATianJiangHaoLiParam
{
	RATianJiangHaoLiParam() { this->Reset(); }
	void Reset()
	{
		ra_tian_jiang_hao_li_begin_timestamp = 0;
		ra_tian_jiang_hao_li_gather_count = 0;
	}

	unsigned int ra_tian_jiang_hao_li_begin_timestamp;
	int ra_tian_jiang_hao_li_gather_count;
};

UNSTD_STATIC_CHECK(sizeof(RATianJiangHaoLiParam) <= sizeof(RoleActivityRoleData))

////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RALaoDongHaoLiParam
{
	RALaoDongHaoLiParam() { this->Reset(); }
	void Reset()
	{
		ra_tian_jiang_hao_li_begin_timestamp = 0;
		ra_tian_jiang_hao_li_gather_count = 0;
	}

	unsigned int ra_tian_jiang_hao_li_begin_timestamp;
	int ra_tian_jiang_hao_li_gather_count;
};

UNSTD_STATIC_CHECK(sizeof(RALaoDongHaoLiParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_TIAN_JIANG_HAO_LI_PARAM_HPP__
