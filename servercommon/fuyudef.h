#ifndef __FU_YU_DEF__
#define __FU_YU_DEF__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int WEAR_FU_YU_WEAR_NUM = 3;		// 单个伙伴最多装备符玉数
static const int FU_YU_STRENGTHEN_TOTAL_RATE = 10000;
static const int MAX_FU_YU_RANK_NUM = 15;		// 符玉最大阶数

static const int HUN_YUAN_FU_YU_EXTRA_SP_LEVEL = 10;	// 混元符玉额外特效等级
static const int HUA_SHEN_FU_YU_EXTRA_SP_LEVEL = 15;	// 化神符玉额外特效等级

enum FU_YU_SP_LIMIT_TYPE
{
	FU_YU_SP_NON_LIMIT = 0,			// 不限制, param1-无效
	FU_YU_SP_PROF_LIMIT = 1,		// 职业限制, param1-职业类型100-800
	FU_YU_SP_PARNTER_LIMIT = 2,		// 伙伴限定, param1-伙伴id

	FU_YU_SP_LIMIT_TYPE_MAX,
};

struct FuYuItemInfo
{
	FuYuItemInfo()
	{
		partner_id = 0;
		wear_idx = -1;
		item_id = 0;
		num = 0;
		is_bind = 0;
		has_param = 0;
		invalid_time = 0;
		item_unique_id = 0LL;
		param_length = 0;
		memset(param_data, 0, sizeof(param_data));
	}

	int partner_id;				//!< 伙伴id
	int wear_idx;				//!< 穿戴index(0-2)
	ItemID item_id;				//!< 物品id
	short num;					//!< 数量
	char is_bind;				//!< 是否绑定
	char has_param;				//!< 是否有净值参数
	UInt32 invalid_time;		//!< 过期时间
	long long item_unique_id;	//!< 道具唯一ID
	short param_length;			//!< 净值参数的数据长度 （当has_param为1时可读）
	NetValueItemParamData param_data;	//!< 净值参数的数据，根据param_length的值来读取 （当has_param为1时可读）
};

struct FuYuCommonParam
{
	FuYuCommonParam()
	{
		this->Reset();
	}

	void Reset()
	{
		reserve_sh = 0;
	}

	long long reserve_sh;		//保留字
};

typedef char FuYuCommonParamHex[sizeof(FuYuCommonParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(FuYuCommonParamHex) < 128);


#pragma pack(pop)
#endif 
