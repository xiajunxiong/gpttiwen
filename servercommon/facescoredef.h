#ifndef __FACE_SCORE_DEF_H__
#define __FACE_SCORE_DEF_H__

#include "servercommon/bitmapfunction.hpp"
#include "servercommon/struct/battle/battle_def.hpp"

#pragma pack(push, 4)

static const int FACE_ADDITION_TYPE_MAX = 10;		// 颜值加成类别
static const int FACE_RESOURCE_NUM_MAX = 1023;		// 颜值系统 资源数量 最大 1023 条
static const int MAX_FACE_RECORD_LEN = sizeof(short) + (sizeof(int) + sizeof(int)) * FACE_RESOURCE_NUM_MAX;	// 记录长度

enum FACE_SCORE_TYPE
{
	FACE_CHATFRAME = 0,			// 聊天框 | 冒泡框
	FACE_SIGNATURE = 1,			// 签名档 | 尾标
	FACE_TITLENAME = 2,			// 头衔	  此时此处的头衔跟称号是合并的

	FACE_TYPE_MAX
};
UNSTD_STATIC_CHECK(FACE_TYPE_MAX <= FACE_ADDITION_TYPE_MAX)		// 最多拓展到 10 种

struct FaceScoreParam
{
	FaceScoreParam() { this->Reset(); }

	void Reset()
	{
		memset(face_equip_list, -1, sizeof(face_equip_list));		//seq 的起始值为0,所以重置需要修改为-1	
		active_flag.Reset();
		is_first_change_equip_list_reset = 0;
		reserve_char = 0;
		reserve_short = 0;
		face_level = -1;
	}

	int face_equip_list[FACE_ADDITION_TYPE_MAX];	// 已装备的（索引seq，详情查看颜值系统配置表）
	BitMap<FACE_RESOURCE_NUM_MAX> active_flag;		// 激活标记
	char is_first_change_equip_list_reset;			// 是否是第一次更改重置为-1, 0:未重置 1:先前的重置 2:新重置(重置装备信息) 
	char reserve_char;
	short reserve_short;	
	int face_level;									// 当前等级
	
	
};
typedef char FaceScoreParamHex[sizeof(FaceScoreParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(FaceScoreParamHex) < 512);

/****************** 时间戳库存 ***************************/
struct FaceScoreTimeParam 
{
	FaceScoreTimeParam() { this->Reset(); }

	void Reset()
	{
		memset(times, 0, sizeof(times));
	}

	unsigned int times[FACE_RESOURCE_NUM_MAX];		// [seq] = 到期时间戳
};
typedef char FaceScoreTimeParamHex[sizeof(FaceScoreTimeParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(FaceScoreTimeParamHex) < 9216);

#pragma pack(pop)
#endif