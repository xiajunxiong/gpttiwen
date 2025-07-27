#ifndef __PET_GOD_FB_DEF_HPP__
#define __PET_GOD_FB_DEF_HPP__

#include "servercommon/servercommon.h"

//神印之地
static const int PET_GOD_REWARD_GROUP_BASE_RATE = 10000;		//奖励组基础概率(万分比)

static const int PET_GOD_FB_MAX_TYPE_NUM = 4;//副本类型


static const int PET_GOD_FB_DRAW_NUM = 5;	//翻牌数量


#pragma pack(push, 4)
//神印之地角色参数
struct PetGodFBParam
{
	PetGodFBParam()
	{
		this->Reset();
	}
	void Reset()
	{
		day_pass_times = 0;
		day_buy_times = 0;
		hold_ll = 0;
		memset(fb_info_list, 0, sizeof(fb_info_list));
	}

	struct FBInfo
	{
		FBInfo()
		{
			max_history_pass_level = 0;
			hold_i = 0;
		}
		int max_history_pass_level;		//历史最高通关层数
		int hold_i;
	};


	int day_pass_times;		//今日通关次数
	int day_buy_times;		//今日购买次数

	long long hold_ll;

	//副本信息[副本类型 - 1]
	FBInfo fb_info_list[PET_GOD_FB_MAX_TYPE_NUM];

};

typedef char PetGodFBParamHex[sizeof(PetGodFBParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(PetGodFBParamHex) < 128);


#pragma pack(pop)

#endif