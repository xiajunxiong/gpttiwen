#ifndef __HONGMENG_TIANJIE_DEF_HPP__
#define __HONGMENG_TIANJIE_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"

//鸿蒙天界
namespace HongMengTianJie
{
	static const int MAX_HONGMENG_TIANJIE_BOSS_NUM = 3;				// 副本内BOSS 数量
	static const int MAX_LIMIT_SEQ_NUM = 10;						// 限购物品种类上限
	static const int MAX_LIMIT_ITEM_NUM = 100;						// 限购物品的购买上限
	static const int MAX_FB_INDEX_NUM = 64;							// 副本上限

	struct WorldFBData
	{
		unsigned int first_pass_time;
		int pass_uid[MAX_TEAM_MEMBER_NUM];
	};
};
#pragma pack(push, 4)

struct RoleHongMengTianJieParam
{
	RoleHongMengTianJieParam()
	{
		this->Reset();
	}
	void Reset()
	{
		next_help_refresh_time = 0;
		next_reward_refresh_time = 0;

		memset(reward_times, 0, sizeof(reward_times));
		help_times = 0;

		memset(limit_buy_times, 0, sizeof(limit_buy_times));
		memset(re_ch, 0, sizeof(re_ch));

		fb_first_pass_mark.Reset();
	}

	unsigned int next_help_refresh_time;										// 下一次助战奖励刷新时间
	unsigned int next_reward_refresh_time;										// 下一次 BOSS 奖励刷新时间

	char help_times;															// 助战次数
	char reward_times[HongMengTianJie::MAX_HONGMENG_TIANJIE_BOSS_NUM];			// boss奖励次数
	char limit_buy_times[HongMengTianJie::MAX_LIMIT_SEQ_NUM];					// 限购物品购买次数

	char re_ch[30];

	BitMap<HongMengTianJie::MAX_FB_INDEX_NUM> fb_first_pass_mark;				// 副本首通标记 fb_id
};

typedef char RoleHongMengTianJieParamHex[sizeof(RoleHongMengTianJieParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleHongMengTianJieParamHex) < 128);

#pragma pack(pop)

#endif