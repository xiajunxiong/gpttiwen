#ifndef __NEW_COURAGE_CHALLENGE_DEF_HPP__
#define __NEW_COURAGE_CHALLENGE_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"

#pragma pack(push, 4)

static const int MAX_NEW_COURAGE_CHALLENGE_LIST_NUM = 1000;
static const int MAX_NEW_COURAGE_CHALLENGE_REWARD_NUM = 512;

static const int NEW_COURAGE_CHALLENGE_FIRST_PASS_BIT_NUM = 7;		// 默认首次通关位运算位置	
static const int NEW_COURAGE_CHALLENGE_EXTER_REWARD_BIT_NUM = 6;	// 额外奖励默认位运算位置

struct NewLayerLevelNode
{
	NewLayerLevelNode() { this->Reset(); }

	void Reset()
	{
		layer_level = -1;
		level_seq = -1;
		flag = 0;
	}
	short layer_level;			// 分层等级段
	char level_seq;				// 该分层等级段中的seq
	unsigned char flag;			// 该层的完成情况  二进制位运算 
};

struct NewCourageChallengeParam
{
	NewCourageChallengeParam() { this->Reset(); }

	void Reset()
	{
		token_is_buy = 0;
		memset(sh_reserve, 0, sizeof(sh_reserve));
		common_reward_flag.Reset();
		token_reward_flag.Reset();
	}

	char token_is_buy;
	char sh_reserve[3];

	BitMap<MAX_NEW_COURAGE_CHALLENGE_REWARD_NUM> common_reward_flag;
	BitMap<MAX_NEW_COURAGE_CHALLENGE_REWARD_NUM> token_reward_flag;
};
typedef char NewCourageChallengeParamHex[sizeof(NewCourageChallengeParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(NewCourageChallengeParamHex) < 512);
#pragma pack(pop)


struct NewLayerLevelInfo
{
	NewLayerLevelInfo() { this->Reset(); }

	void Reset()
	{
		change_state = 0;
		info.Reset();
	}

	char change_state;
	NewLayerLevelNode info;
};

class TLVSerializer;
class TLVUnserializer;
class NewCourageChallengeLevelParamList
{
public:
	NewCourageChallengeLevelParamList() : count(0)
	{}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	int count;
	NewLayerLevelInfo data_list[MAX_NEW_COURAGE_CHALLENGE_LIST_NUM];
};

#endif
