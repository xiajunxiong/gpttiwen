#ifndef __MSG_RA_YUN_ZE_CUTE_PET_HPP__
#define __MSG_RA_YUN_ZE_CUTE_PET_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rayunzecutepetparam.hpp"


#pragma pack(push, 4)

namespace Protocol
{
	
	class SCRAYunZeCutePetInfo		//!< 3134 随机活动-萌宠驾到信息下发
	{
	public:
		SCRAYunZeCutePetInfo();
		MessageHeader header;

		int pet_combat_num;					// 宠物战斗力
		int today_fraction[RAYZCutePet::ACTIVITY_DAY_MAX];	 //历史每日获得战力[活动最长持续时间--12]															
		int task_param_list[RAYZCutePet::TASK_TYPE_MAX];	 //任务参数[任务类型], 每日任务得分															
		BitMap<RAYZCutePet::DAY_TASK_COUNT_MAX> task_status; //任务完成状态
		
	};

	class SCRAYunZeCutePetChallengeInfo		//!< 3136 随机活动-萌宠驾到挑战信息下发
	{
	public:
		SCRAYunZeCutePetChallengeInfo();
		MessageHeader header;

		int is_challenge_status;		// 本次是否挑战成功, -1--登录下发,0--失败，1--成功，2--平局
		BitMap<RAYZCutePet::MONSTER_MAX_NUM + 1> challenge_status; //萌宠挑战成功状态[1-10]--当前下标位是否挑战成功过
	};

}


#pragma pack(pop)

#endif
