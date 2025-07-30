#ifndef __MSG_SCHOOL_EXERCISE_H__
#define __MSG_SCHOOL_EXERCISE_H__

#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push)
#pragma pack(4)

namespace Protocol
{
	class SCSchoolExerciseInfo		//!< 3590 校场演练信息
	{
	public:
		SCSchoolExerciseInfo();
		MessageHeader header;

		long long clear_flag;		//!< 通关标记 （以二进制形式表示，第X位为1则seq为X的关卡已通关）
		int has_open_ui_ever;		//!< 是否曾经打开UI  0:否  1:是
	};

	class CSSchoolExerciseChallenge	//!< 3591 校场演练挑战关卡
	{
	public:
		CSSchoolExerciseChallenge();
		MessageHeader header;

		int seq;					//!< 挑战的关卡序号 
	};

	class SCSchoolExerciseChallengeSucc //!< 3592 挑战成功通知
	{
	public:
		SCSchoolExerciseChallengeSucc();
		MessageHeader header;

		int seq;					//!< 挑战的关卡序号 
		int has_reward;				//!< 是否有奖励 0:否 1:是
	};

	class CSSchoolExerciseFirstOpenUI //!< 3593 首次打开UI时请求
	{
	public:
		CSSchoolExerciseFirstOpenUI();
		MessageHeader header;
	};
}

#pragma pack(pop)

#endif