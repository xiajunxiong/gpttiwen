#ifndef __YIN_YANG_EYE_DEF_H__
#define __YIN_YANG_EYE_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"

#pragma pack(push, 4)

static const int YIN_YANG_EYE_BOX_MAXNUM = 512;			//宝箱最大索引
static const int YIN_YANG_EYE_NPC_MAXNUM = 512;			//NPC最大索引

enum YIN_YANG_EYE_TYPE
{
	YIN_YANG_EYE_TYPE_BOX = 0,			// 宝箱
	YIN_YANG_EYE_TYPE_NPC = 1,			// npc

	YIN_YANG_EYE_TYPE_END,
};

enum YIN_YANG_EYE_NPC_TYPE
{
	YIN_YANG_EYE_NPC_TYPE_GOOD = 0,		// 善
	YIN_YANG_EYE_NPC_TYPE_BAD = 1,		// 恶

	YIN_YANG_EYE_NPC_TYPE_END,
};

struct YinYangEyeParam
{
	YinYangEyeParam()
	{
		this->Reset();
	}

	void Reset()
	{
		open_mark = 0;
		sh_reserve = 0;
		ch_reserve = 0;

		cur_box_finish_seq = -1;
		cur_npc_finish_seq = -1;

		box_flag.Reset();
		npc_flag.Reset();
	}

	char open_mark;										//阴阳眼开启状态
	char sh_reserve;
	short ch_reserve;

	short cur_box_finish_seq;							//记录当前正完成的索引 对话完成之后客户端再请求领取
	short cur_npc_finish_seq;

	BitMap<YIN_YANG_EYE_BOX_MAXNUM> box_flag;
	BitMap<YIN_YANG_EYE_NPC_MAXNUM> npc_flag;
};

typedef char YinYangEyeParamHex[sizeof(YinYangEyeParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(YinYangEyeParamHex) < 512);

#pragma pack(pop)
#endif
