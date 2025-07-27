#ifndef __COURAGE_CHALLENGE_DEF_H__
#define __COURAGE_CHALLENGE_DEF_H__

#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"

#pragma pack(push, 4)

static const int COURAGE_CHALLENGE_MAX_LEVEL = 150;
static const int COURAGE_CHALLENGE_SEQ_MAX_NUM = 400;
static const int COURAGE_CHALLENGE_FIRST_PASS_BIT_NUM = 7;				// 默认首次通关位运算位置		
static const int COURAGE_CHALLENGE_EXTER_REWARD_BIT_NUM = 6;		// 额外奖励默认位运算位置
static const int COURAGE_CHALLENGE_CONDITION_MAX_NUM = 3;
static const int COURAGE_CHALLENGE_LAYER_LEVEL_MAX_NUM = 20;			// 等级组最大个数
static const int COURAGE_CHALLENGE_LAYER_LEVEL_NEW_ADD_MAX_NUM = 30;	// 新增等级组数量
static const int COURAGE_CHALLENGE_SCORE_LAYER_LEVEL_MAX_NUM = 64;
static const int COURAGE_CHALLENGE_MAX_SCORE_FLAG_BIT = COURAGE_CHALLENGE_SCORE_LAYER_LEVEL_MAX_NUM - 1;		// 满星奖励

static const int MAX_COURAGE_CHALLENGE_SCORE_NUM = 120;					//分层等级最多的数量
static const int MAX_COURAGE_CHALLENGE_LEVEL_SEQ = 15;					//每个分层等级最多的小关卡数量
static const int MAX_COURAGE_CHALLENGE_LEVEL_NUM = MAX_COURAGE_CHALLENGE_SCORE_NUM * MAX_COURAGE_CHALLENGE_LEVEL_SEQ;		//所有分层的小关卡总和数量
UNSTD_STATIC_CHECK(MAX_COURAGE_CHALLENGE_SCORE_NUM == MAX_ROLE_LEVEL);

struct LayerLevelNode
{
	LayerLevelNode() { this->Reset(); }

	void Reset()
	{
		layer_level = -1;
		level_seq = -1;
		flag = 0;
	}
	short layer_level;			// 分层等级段
	char level_seq;				// 该分层等级段中的seq
	unsigned char flag;			// 该层的完成情况  二进制位运算，根据当前配置多少个评分字段 COURAGE_CHALLENGE_CONDITION_MAX_NUM 
};

struct ScoreLayerLevelNode
{
	ScoreLayerLevelNode() { this->Reset(); }

	void Reset()
	{
		layer_level = -1;
		star = 0;
		flag.Reset();
	}

	short layer_level;			// 分层等级段
	short star;					// 改分层等极端的已达成的总星数
	BitMap<COURAGE_CHALLENGE_SCORE_LAYER_LEVEL_MAX_NUM> flag;   // 此时评分奖励领取标识
};

struct CourageChallengeParam
{
	CourageChallengeParam()
	{
		this->Reset();
	}

	void Reset()
	{
		change_reset_flag = 0;
		is_add_rank = 0;
		is_set_new_layer_level_data = 0;
		is_set_old_data_version_flag = 0;
		old_get_timestamp = 0;
		is_change_data_storage_flag = 0;
		reserve_ch = 0;
		pet_helper_num = 0;
		new_get_timestamp = 0;
		reserve_ll = 0;
		memset(reserve_long, 0, sizeof(reserve_long));
		count = 0;
		score_layer_count = 0;
		for (int i = 0; i < COURAGE_CHALLENGE_LAYER_LEVEL_MAX_NUM; i++)
		{
			score_data_node[i].Reset();
		}
		for (int i = 0; i < COURAGE_CHALLENGE_SEQ_MAX_NUM; i++)
		{
			data_node[i].Reset();
		}
		for (int i = 0; i < COURAGE_CHALLENGE_LAYER_LEVEL_NEW_ADD_MAX_NUM; i++)
		{
			score_1_data_node[i].Reset();
		}
		memset(layer_data_node_version, 0, sizeof(layer_data_node_version));
		for (int i = 0; i < MAX_COURAGE_CHALLENGE_SCORE_NUM; i++)
		{
			score_flag[i].Reset();
		}
	}
	void DataReset()
	{
		count = 0;
		score_layer_count = 0;
		for (int i = 0; i < COURAGE_CHALLENGE_SEQ_MAX_NUM; i++)
		{
			data_node[i].Reset();
		}
		for (int i = 0; i < COURAGE_CHALLENGE_LAYER_LEVEL_MAX_NUM; i++)
		{
			score_data_node[i].Reset();
		}
		for (int i = 0; i < COURAGE_CHALLENGE_LAYER_LEVEL_NEW_ADD_MAX_NUM; i++)
		{
			score_1_data_node[i].Reset();
		}
		memset(layer_data_node_version, 0, sizeof(layer_data_node_version));
	}
	
	char change_reset_flag;				// 锢魔之塔大改后重置之前数据  6:已重置 非6未重置
	char is_add_rank;					// 第一次登陆需要在锢魔之塔列表添加玩家信息 4：已添加
	char is_set_new_layer_level_data;	// 是否设置新增等级组数据 0:未设置 1:已设置
	char is_set_old_data_version_flag;	// 是否设置旧数据版本号 1已设置
	unsigned int old_get_timestamp;		// 获得新星数时的时间戳 (已废弃)
	char is_change_data_storage_flag;	// 改变存储数据位置标识 1:已改变
	char reserve_ch;
	short pet_helper_num;				// 宠物助战数量 取值范围[0,4]
	long long new_get_timestamp;		// 获得新星数时的时间戳			
	long long reserve_ll;		
	long long reserve_long[3];		
	short count;					// 当前玩家已拥有的数据数量
	short score_layer_count;		// 当前玩家分层等极段评分数据数量
	ScoreLayerLevelNode score_data_node[COURAGE_CHALLENGE_LAYER_LEVEL_MAX_NUM];	// 玩家在锢魔之塔中每一个分层等极段评分数据
	LayerLevelNode data_node[COURAGE_CHALLENGE_SEQ_MAX_NUM];					// 玩家在锢魔之塔中每一层的数据
	ScoreLayerLevelNode score_1_data_node[COURAGE_CHALLENGE_LAYER_LEVEL_NEW_ADD_MAX_NUM];	// 玩家在锢魔之塔中每一个分层等极段评分数据(新增数量)
	short layer_data_node_version[COURAGE_CHALLENGE_LAYER_LEVEL_MAX_NUM + COURAGE_CHALLENGE_LAYER_LEVEL_NEW_ADD_MAX_NUM];		// 以等级分层为单位,记录版号,修改了则重置该分层通关与评分数据
	BitMap<COURAGE_CHALLENGE_SCORE_LAYER_LEVEL_MAX_NUM> score_flag[MAX_COURAGE_CHALLENGE_SCORE_NUM];			//评分奖励领取标识
};

typedef char CourageChallengeParamHex[sizeof(CourageChallengeParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(CourageChallengeParamHex) < 7168);
#pragma pack(pop)


struct LayerLevelInfo
{
	LayerLevelInfo() { this->Reset(); }

	void Reset()
	{
		change_state = 0;
		info.Reset();
	}

	char change_state;
	LayerLevelNode info;
};

class TLVSerializer;
class TLVUnserializer;
class RoleCourageChallengeLevelParamList
{
public:
	RoleCourageChallengeLevelParamList() : count(0)
	{}

	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);
	
	int count;
	LayerLevelInfo data_list[MAX_COURAGE_CHALLENGE_LEVEL_NUM];
};

#endif
