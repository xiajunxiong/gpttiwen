#ifndef __NETHER_WORLD_PURGATORY_DEF_H__
#define __NETHER_WORLD_PURGATORY_DEF_H__

#include "servercommon/bitmapfunction.hpp"

const static int MAX_NETHER_WORLD_GATORY_CHAPTER_NUM = 20;	//最大章节号
const static int MAX_NETHER_WORLD_GATORY_INDEX = 20;		//最大关卡序号 -- 检查NetherWorldGatoryChapters对齐
const static int MAX_NETHER_WORLD_GATORY_TASK_NUM = 4;		//每关卡最大任务数 -- 检查NetherWorldGatoryLevel对齐

#pragma pack(push, 4)

struct NetherWorldGatoryLevel
{
	void Reset()
	{
		round_num = 0;
		pass_map.Reset();
		reward_map.Reset();
	}

	int round_num;
	BitMap<MAX_NETHER_WORLD_GATORY_TASK_NUM> pass_map;//是否完成任务
	BitMap<MAX_NETHER_WORLD_GATORY_TASK_NUM> reward_map;//是否领取奖励
};

UNSTD_STATIC_CHECK(sizeof(NetherWorldGatoryLevel) % 4 == 0);

struct NetherWorldGatoryChapters
{
	NetherWorldGatoryChapters() { this->Reset(); }
	void Reset()
	{
		for (int i = 0; i < MAX_NETHER_WORLD_GATORY_INDEX; i++)
		{
			challenge_info[i].Reset();
		}
		silver_reward_flag.Reset();
		silver_reward_flag.Reset();
	}
	NetherWorldGatoryLevel challenge_info[MAX_NETHER_WORLD_GATORY_INDEX];
	BitMap<MAX_NETHER_WORLD_GATORY_INDEX> silver_reward_flag;
	BitMap<MAX_NETHER_WORLD_GATORY_INDEX> gold_reward_flag;
};
UNSTD_STATIC_CHECK(sizeof(NetherWorldGatoryChapters) % 4 == 0);

struct NetherWorldGatoryParam
{
	/*************** **********************/
	NetherWorldGatoryParam() { this->Reset(); }

	void Reset()
	{
		pass_challenge_chapters = 0;
		pass_challenge_id = 0;
		today_pass_times = 0;

		low_key = 0;
		mid_key = 0;
		high_key = 0;
		show_video = 0;
		next_day_buff_up = 0;
		buff_level = 0;
		for (int i = 0; i < MAX_NETHER_WORLD_GATORY_CHAPTER_NUM; i++)
		{
			chapter_list[i].Reset();
		}
	}

	int pass_challenge_chapters;//通关章节
	int pass_challenge_id;//通关id
	int today_pass_times;

	int low_key;
	int mid_key;
	int high_key;
	short show_video;
	short next_day_buff_up;
	int buff_level;
	NetherWorldGatoryChapters chapter_list[MAX_NETHER_WORLD_GATORY_CHAPTER_NUM];
};
UNSTD_STATIC_CHECK(sizeof(NetherWorldGatoryParam) % 4 == 0);//对齐
typedef char NetherWorldGatoryParamHex[sizeof(NetherWorldGatoryParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(NetherWorldGatoryParamHex) < 15360);


#pragma pack(pop)
#endif