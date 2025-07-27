#ifndef __RA_SHAN_HAI_MEET_PARAM_HPP__
#define __RA_SHAN_HAI_MEET_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2140 山海相逢  //////////////////////////////////////////

enum RA_SHAN_HAI_MEET_ACT_TYPE
{
	RA_SHAN_HAI_MEET_ACT_TYPE_0 = 0,							// 2122 山海秘宝	点亮x行
	RA_SHAN_HAI_MEET_ACT_TYPE_1,								// 2134 连连看		完成x次
	RA_SHAN_HAI_MEET_ACT_TYPE_2,								// 2124 露月争锋	击败x次boss
	RA_SHAN_HAI_MEET_ACT_TYPE_3,								// 2142 山海问答	参与x次问答
	RA_SHAN_HAI_MEET_ACT_TYPE_4,								// 2140 山海风月	参与x次
	RA_SHAN_HAI_MEET_ACT_TYPE_5,								// 2122 山海秘宝	点亮x张
	RA_SHAN_HAI_MEET_ACT_TYPE_6,								// 2125 山海花灯	参与x次
	RA_SHAN_HAI_MEET_ACT_TYPE_7,								// 2156 山海觅宝	点亮x行
	RA_SHAN_HAI_MEET_ACT_TYPE_8,								// 2156 山海觅宝	点亮x张

	RA_SHAN_HAI_MEET_ACT_TYPE_END,
};

static const int RA_SHAN_HAI_MEET_ACT_TYPE_MAX = 16;			//活动类型最大数量
UNSTD_STATIC_CHECK(RA_SHAN_HAI_MEET_ACT_TYPE_END <= RA_SHAN_HAI_MEET_ACT_TYPE_MAX);
static const int RA_SHAN_HAI_MEET_TASK_ID_MAX = 32;				//活动最大索引
static const int RA_SHAN_HAI_MEET_POKER_MAX_INDEX = 6;			//翻牌最大下标

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAShanHaiMeetParam
{
	RAShanHaiMeetParam() { this->Reset(); }
	void Reset()
	{
		begin_timestamp = 0;
		is_buy = 0;
		sh_server = 0;
		poker_count = 0;

		memset(finish_count, 0, sizeof(finish_count));
		fetch_flag.Reset();
		poker_idx_flag.Reset();
		memset(poker_item_seq, -1, sizeof(poker_item_seq));
	}

	unsigned int	begin_timestamp;
	char			is_buy;												// 1:已购买 0:未购买
	char			sh_server;
	short			poker_count;										// 可翻牌次数(弃用)

	int				finish_count[RA_SHAN_HAI_MEET_ACT_TYPE_MAX];
	BitMap<RA_SHAN_HAI_MEET_TASK_ID_MAX> fetch_flag;

	BitMap<RA_SHAN_HAI_MEET_POKER_MAX_INDEX> poker_idx_flag;
	int poker_item_seq[RA_SHAN_HAI_MEET_POKER_MAX_INDEX];				//翻牌奖励索引(给客户端展示用)
};

UNSTD_STATIC_CHECK(sizeof(RAShanHaiMeetParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_SHAN_HAI_MEET_PARAM_HPP__
