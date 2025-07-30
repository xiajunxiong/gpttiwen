#ifndef __MSG_RA_DUAN_YANG_XIANG_NANG_HPP__
#define __MSG_RA_DUAN_YANG_XIANG_NANG_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

#include "servercommon/roleactivity/raduanyangxiangnangparam.hpp"

#pragma  pack(push, 4)

namespace Protocol
{

	////////////////////////////////  端阳香囊  /////////////////////////////////////
	struct SCRADuanYangXiangNang		//<!3183 随机活动-端阳香囊
	{
	public:
		SCRADuanYangXiangNang();
		MessageHeader header;

		BitMap<DUAN_WU_ACTIVITY_MAX_BITMAP_NUM> today_task_finish;		// 每日任务是否完成
		BitMap<DUAN_WU_ACTIVITY_MAX_BITMAP_NUM> is_receive_reward;		// 每日任务奖励是否已领取
		int today_task_count[MAX_TASK_NUM_DUAN_YANG_XIANG_NANG];		// 每日任务进度
	};


	////////////////////////////////  苍龙赐福  /////////////////////////////////////
	struct SCRACangLongCiFuInfo		//<!3184 随机活动-苍龙赐福
	{
	public:
		SCRACangLongCiFuInfo();
		MessageHeader header;

		int all_light_reward_flag;		// 全部点亮奖励标识, 0--不可领取, 1--可领取, 2--已领取
		BitMap<CLCF_BITMAP_MAX_VALUE> light_up_flag;
	};

	////////////////////////////////  兰汤初沐  /////////////////////////////////////
	struct SCRALanTangChuMuInfo		//<!3186 随机活动-兰汤初沐
	{
	public:
		SCRALanTangChuMuInfo();
		MessageHeader header;

		BitMap<DUAN_WU_ACTIVITY_MAX_BITMAP_NUM> is_receive_reward;	// 每日任务奖励是否已领取
		int today_task_count[MAX_TASK_NUM_LAN_TANG_CHU_MU];			// 每日任务进度

		int last_reward_flag;		// 最终奖励, 0--不可领取, 1--可领取, 2--已领取
		short add_herbs_num[MAX_HERBS_TYPE_LAN_TANG_CHU_MU];	// 已加入药材数量
	};

}

#pragma  pack(pop)

#endif
