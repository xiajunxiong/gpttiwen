#ifndef __MSG_COURSE_OF_GROWTH_H__
#define __MSG_COURSE_OF_GROWTH_H__

#include "gamedef.h"
#include "servercommon/courseofgrowthdef.h"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{
	class CSCOGOperatorRequest
	{
	public:
		CSCOGOperatorRequest();			//!< 1780 - 成长之路 操作请求
		MessageHeader header;

		enum COGOperatorType
		{
			COG_OP_RECEIVES_AWARD = 0,		// 领取任务奖励
			COG_OP_FIGHT_THE_BOSS = 1,		// 打守星BOSS
			COG_OP_GET_BOSS_AWARD = 2,		// 领取BOSS奖励
			COG_OP_FETCH_LEVEL_REWARD = 3,	// 领取历程等级奖励
			COG_OP_SERVER_COURSE_INFO = 4,	// 请求获取历程全服完成情况
		};
		
		UInt16 op_type;			//!< 请求	|  0  |  1  |  2  |   3   |
		UInt16 param;			//!< 参数	| seq |  /  |  /  | level |
	};

	class SCCOGAllInformations
	{
	public:
		SCCOGAllInformations();			//!< 1781 - 成长之路 所有信息
		MessageHeader header;

		UInt16 cur_prog;				//!< 当前阶级
		UInt16 cur_star;				//!< 当前星级
		UInt16 cur_reward_prog;			//!< 当前奖励阶级
		UInt16 cur_reward_star;			//!< 当前奖励星级

		UInt32 task_prog_list[GROWTH_TASK_NUM_PER_STAR];	//!< 任务进度
		BitMap<GROWTH_TASK_NUM_PER_STAR> award_fetch_flag;	//!< 奖励领取状态标记(二进制列表)
	};

	class SCCOGSingleTaskProgs
	{
	public:
		SCCOGSingleTaskProgs();			//!< 1782 - 成长之路 单个进度通知
		MessageHeader header;

		int seq;				//!< 游标 - [0, 8]
		UInt32 prog_num;		//!< 进度数值
	};

	class SCCOGRewardFetchFlag
	{
	public:
		SCCOGRewardFetchFlag();			//!< 1783 - 成长之路 奖励领取通知
		MessageHeader header;

		UInt16 seq;				//!< 游标 - [0, 8]
		UInt16 flag;			//!< 领取状态
	};

	class SCCOGStarAwardStatus
	{
	public:
		SCCOGStarAwardStatus();			//!< 1784 - 成长之路 当前星级奖励阶段
		MessageHeader header;

		UInt16 cur_reward_prog;			//!< 当前星级奖励阶段
		UInt16 cur_reward_star;			//!< 当前星级奖励星级
	};

	class SCCOGSingleSeqNotify
	{
	public:
		SCCOGSingleSeqNotify();			//!< 1785 - 完成的任务提示
		MessageHeader header;

		int notify_seq;				//!< 当前阶段的任务的游标 [0, 8]
	};

	class SCRoleCourseAllInfo		//!< 1786 成长之路 历程所有信息下发
	{
	public:
		SCRoleCourseAllInfo();
		MessageHeader header;

		int course_level;						//历程等级
		int course_cur_fraction;				//历程点
		BitMap<MAX_COURSE_ENTRY_LIMIT_NUM> finish_record_flag;					// 历程条目完成记录
		BitMap<MAX_COURSE_LEVEL_NUM> level_reward_fetch_flag;					// 历程等级奖励领取记录

		int count;						
		UInt32 course_prog_list[MAX_COURSE_ENTRY_LIMIT_NUM];					// 历程条目进度
	};

	class SCRoleCourseSingleInfo		//!< 1787  成长之路 历程单条信息下发
	{
	public:
		SCRoleCourseSingleInfo();
		MessageHeader header;
		
		int seq;				
		int course_prog;
	};

	class SCRoleCourseNoticeInfo			//!< 1788 成长之路 历程信息通知下发
	{
	public:
		SCRoleCourseNoticeInfo();
		MessageHeader header;

		enum ROLE_COURSE_NOTICE_TYPE
		{
			ROLE_COURSE_NOTICE_TYPE_FINISH_ENTRY = 0,		// 通知条目完成标识  param1 - seq  param2 - 0:未完成 1:已完成
			ROLE_COURSE_NOTICE_TYPE_LEVEL_FECHT = 1,		// 通知等级领取标识  param1 - level param2 - 0:未领取 1:已领取

			ROLE_COURSE_NOTICE_TYPE_MAX
		};

		int notice_type;
		int course_level;						//历程等级
		int course_cur_fraction;				//历程点
		int param1;
		int param2;
	};

	class SCServerCourseAllInfo			//!< 1789	成长之路 历程完成信息下发	
	{
	public:
		SCServerCourseAllInfo();
		MessageHeader header;

		int server_role_count;											//该服玩家人数
		int count;		
		int course_finish_count_list[MAX_COURSE_ENTRY_LIMIT_NUM];		//每一条完成人数
	};

	class SCServerCourseSingleInfo			//!< 2027	成长之路 历程完成单条信息下发
	{
	public:
		SCServerCourseSingleInfo();
		MessageHeader header;

		int server_role_count;											//该服玩家人数
		int seq;														
		int course_finish_count;										
	};
}

#pragma pack(pop)
#endif