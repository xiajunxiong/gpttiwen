#ifndef __RA_FORTUNE_MAZE_PARAM_HPP__
#define __RA_FORTUNE_MAZE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////

/*
		默认值为0       正确顺序
		X	X	X		1	2	3
		X	X	X		4	5	6
		X	X	X		7	8	9

*/
const static int PIN_TU_INDEX_NUM = 9;
const static int PIN_TU_TYPE_NUM = 7;
const static int PIN_TU_INDEX[PIN_TU_INDEX_NUM] = { 1,2,3,4,5,6,7,8,9 };
const static int MAX_FORTUNE_MAZE_TASK_NUM = 15;//幸运迷宫任务最大数量

enum FORTUNE_MAZE_TASK_TYPE
{
	FORTUNE_MAZE_TASK_TYPE_1 = 1,	//	宠物技能书
	FORTUNE_MAZE_TASK_TYPE_2 = 2,	//	限时活动
	FORTUNE_MAZE_TASK_TYPE_3 = 3,	//	铜币
	FORTUNE_MAZE_TASK_TYPE_4 = 4,	//	宠物封印
	FORTUNE_MAZE_TASK_TYPE_5 = 5,	//	秘境
	FORTUNE_MAZE_TASK_TYPE_6 = 6,	//	交易所
	FORTUNE_MAZE_TASK_TYPE_7 = 7,	//	声望
	FORTUNE_MAZE_TASK_TYPE_8 = 8,	//	限时寻宝
	FORTUNE_MAZE_TASK_TYPE_9 = 9,	//	野外首领
	FORTUNE_MAZE_TASK_TYPE_10 = 10,	//	宠物强化
	FORTUNE_MAZE_TASK_TYPE_11 = 11,	//	远征魔冢
	FORTUNE_MAZE_TASK_TYPE_12 = 12,	//	秘境体力
	FORTUNE_MAZE_TASK_TYPE_13 = 13,	//	登录
	FORTUNE_MAZE_TASK_TYPE_14 = 14,	//	宝石
	FORTUNE_MAZE_TASK_TYPE_MAX,
};
////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAFortuneMazeParam
{
	RAFortuneMazeParam() { this->Reset(); }

	void Reset()
	{
		open_day = 0;
		begin_time = 0;
		rand_pin_tu_times = 0;
		today_pin_tu_times = 0;
		can_enter_fb = 0;
		today_login_flag = 0;
		mark_index = 0;
		memset(m_pin_tu, 0, sizeof(m_pin_tu));
		memset(task_info, 0, sizeof(task_info));
		old_scene_id = 0;
		login_refresh_tamp = 0;
	}

	void DayReset()
	{
		today_pin_tu_times = 0;
		today_login_flag = 0;
		memset(task_info, 0, sizeof(task_info));
		login_refresh_tamp = 0;
	}

	bool HasPinTu(int pin_tu_id,int id)
	{
		if (pin_tu_id < 0 || pin_tu_id >= PIN_TU_TYPE_NUM)return false;
		for (int i = 0; i < PIN_TU_INDEX_NUM; i++)
		{
			if (id == m_pin_tu[pin_tu_id][i])
			{
				return true;
			}
		}
		return false;
	}

	bool PinTuHasOpen(int pin_tu_id, int index)
	{
		if (pin_tu_id < 0 || pin_tu_id >= PIN_TU_TYPE_NUM)return false;
		if (index < 0 || index >= PIN_TU_INDEX_NUM)return false;
		if (m_pin_tu[pin_tu_id][index] != 0)
		{
			return true;
		}
		return false;
	}

	bool HasOpenAll(int pin_tu_id)
	{
		if (pin_tu_id < 0 || pin_tu_id >= PIN_TU_TYPE_NUM)return false;
		for (int i = 0; i < PIN_TU_INDEX_NUM; i++)
		{
			if (m_pin_tu[pin_tu_id][i] <= 0)
			{
				return false;
			}
		}
		return true;
	}

	bool ExchangePosi(int pin_tu_id, int posi_1, int posi_2)
	{
		if (pin_tu_id < 0 || pin_tu_id >= PIN_TU_TYPE_NUM)return false;
		if (!this->HasOpenAll(pin_tu_id))return false;
		if (posi_1 < 0 || posi_1 >= PIN_TU_INDEX_NUM || posi_2 < 0 || posi_2 >= PIN_TU_INDEX_NUM)return false;
		if (posi_1 == posi_2)return false;
		int pin_tu = m_pin_tu[pin_tu_id][posi_1];
		m_pin_tu[pin_tu_id][posi_1] = m_pin_tu[pin_tu_id][posi_2];
		m_pin_tu[pin_tu_id][posi_2] = pin_tu;
		return true;
	}

	int open_day;
	unsigned int begin_time;
	int rand_pin_tu_times;
	int today_pin_tu_times;
	char can_enter_fb;
	char today_login_flag;
	short mark_index;

	int m_pin_tu[PIN_TU_TYPE_NUM][PIN_TU_INDEX_NUM];

	int task_info[MAX_FORTUNE_MAZE_TASK_NUM];

	int old_scene_id;//上一次未获取宝箱的记录

	unsigned int login_refresh_tamp;
};

UNSTD_STATIC_CHECK(sizeof(RAFortuneMazeParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)


#endif	//__RA_LITTLE_YEAR_GIFT_PARAM_HPP__
