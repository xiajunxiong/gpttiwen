#ifndef __MSG_NEW_COURAGE_CHALLENGE_H__
#define __MSG_NEW_COURAGE_CHALLENGE_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/newcouragechallengedef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{

	class CSNewCourageChallengeOp					//!< 5330 弑神之塔请求
	{
	public:
		CSNewCourageChallengeOp();
		MessageHeader		header;

		enum OP_TYPE
		{
			OP_TYPE_LEVEL_INFO = 0,			//!< 关卡列表信息
			OP_TYPE_REWARD_INFO = 1,		//!< 奖励信息
			OP_TYPE_FETCH_COMMON = 2,		//!< 领取普通奖励 param:seq
			OP_TYPE_FETCH_TOKEN = 3,		//!< 领取战令奖励 param:seq
			OP_TYPE_FETCH_ALL = 4,			//!< 一键领取
		};

		int op_type;
		int param;
	};

	class SCNewCourageChallengeLevelListInfo		//!< 5331 弑神之塔关卡列表信息
	{
	public:
		SCNewCourageChallengeLevelListInfo();
		MessageHeader		header;

		int count;
		NewLayerLevelNode list[MAX_NEW_COURAGE_CHALLENGE_LIST_NUM];
	};

	class SCNewCourageChallengeLevelSignleInfo		//!< 5332 弑神之塔关卡单个信息
	{
	public:
		SCNewCourageChallengeLevelSignleInfo();
		MessageHeader header;

		short layer_level;					// 分层等级段
		char layer_level_seq;				// 该分层等级段的seq
		unsigned char flag;					// 二进制位运算[0,7],0-3 条件完成标识, 7默认是是首杀标识
	};

	class SCNewCourageChallengeRewardInfo			//!< 5333 弑神之塔奖励信息
	{
	public:
		SCNewCourageChallengeRewardInfo();
		MessageHeader header;

		char token_is_buy;					// 1：已购买
		char sh_reserve[3];					// 无用
		BitMap<MAX_NEW_COURAGE_CHALLENGE_REWARD_NUM> common_reward_flag;
		BitMap<MAX_NEW_COURAGE_CHALLENGE_REWARD_NUM> token_reward_flag;
	};
	UNSTD_STATIC_CHECK(MAX_NEW_COURAGE_CHALLENGE_REWARD_NUM == 512);
}

#pragma pack(pop)

#endif


