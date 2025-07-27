#ifndef __RA_MAZE_SUMMER_PARAM_HPP__
#define __RA_MAZE_SUMMER_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

/*
		27		28		29
		24		25		26
		21		22		23
		18		19		20
		15		16		17
		12		13		14
		9		10		11
		6		7		8
		3		4		5
		0		1		2
*/

const static int MAX_MAZE_SUMMER_POS_X = 3;		//横向
const static int MAX_MAZE_SUMMER_POS_Y = 10;	//竖向
//设定房间位置
const static int MAX_MAZE_SUMMER_HOUSE_LIST[MAX_MAZE_SUMMER_POS_X * MAX_MAZE_SUMMER_POS_Y / 2] = { 3,4,5,9,10,11,15,16,17,21,22,23,27,28,29 };
//设定道路位置
const static int MAX_MAZE_SUMMER_ROSE_LIST[MAX_MAZE_SUMMER_POS_X * MAX_MAZE_SUMMER_POS_Y / 2] = { 0,1,2,6,7,8,12,13,14,18,19,20,24,25,26 };

const static int MAX_MAZE_SUMMER_SHOP_NUM = 6;	//最多6件商品

enum RA_MAZE_SUMMER_MAP_TYPE
{
	RA_MAZE_SUMMER_MAP_TYPE_ROSE = 0,		//道路
	RA_MAZE_SUMMER_MAP_TYPE_BOSS = 1,		//boss
	RA_MAZE_SUMMER_MAP_TYPE_MONSTER = 2,	//怪物
	RA_MAZE_SUMMER_MAP_TYPE_SHOP = 3,		//商店
	RA_MAZE_SUMMER_MAP_TYPE_QUESTION = 4,	//答题
};

enum RA_MAZE_SUMMER_MAP_STATUS
{
	RA_MAZE_SUMMER_MAP_STATUS_NORMAL = 0,		//正常
	RA_MAZE_SUMMER_MAP_STATUS_DOING = 1,		//进行中
	RA_MAZE_SUMMER_MAP_STATUS_FINISH = 2,		//完成
};

enum RA_MAZE_SUMMER_MAP_LOCK
{
	RA_MAZE_SUMMER_MAP_HAS_LOCK = 0,		//锁定中
	RA_MAZE_SUMMER_MAP_HAS_UNLOCK = 1,		//解锁完成
};

struct RAMazeSummerMap
{
	RAMazeSummerMap() :map_type(RA_MAZE_SUMMER_MAP_TYPE_ROSE), map_lock(RA_MAZE_SUMMER_MAP_HAS_LOCK),map_status(RA_MAZE_SUMMER_MAP_STATUS_NORMAL), param1(0), param2(0), param3(0){}
	void Reset()
	{
		map_type = RA_MAZE_SUMMER_MAP_TYPE_ROSE;
		map_lock = RA_MAZE_SUMMER_MAP_HAS_LOCK;
		map_status = RA_MAZE_SUMMER_MAP_STATUS_NORMAL;
		param1 = 0;
		param2 = 0;
		param3 = 0;
	}
	short map_type;
	short map_lock;
	short map_status;
	unsigned short param1;
	unsigned short param2;
	unsigned short param3;
};

struct RAMazeSummerShop
{
	RAMazeSummerShop() :shop_id(0), seq(0), buy_times(0){}
	void Reset()
	{
		shop_id = 0;
		seq = 0;
		buy_times = 0;
	}
	char shop_id;
	char seq;//下标
	short buy_times;//购买次数
};

struct RAMazeSummerTaskInfo
{
	RAMazeSummerTaskInfo() : is_get_reward(0), reserve_ch(0), task_id(0) {}
	void Reset()
	{
		is_get_reward = 0;
		reserve_ch = 0;
		task_id = 0;
	}
	char is_get_reward;//奖励是否领取过了
	char reserve_ch;
	unsigned short task_id;//任务id
};

enum RAMAZE_SUMMER_TASK_TYPE
{
	RAMAZE_SUMMER_TASK_TYPE_0 = 0,
	RAMAZE_SUMMER_TASK_TYPE_1 = 1,//通关层数
	RAMAZE_SUMMER_TASK_TYPE_2 = 2,//通关格子数
	RAMAZE_SUMMER_TASK_TYPE_3 = 3,//击杀怪物组数
	RAMAZE_SUMMER_TASK_TYPE_4 = 4,//答对题目数
	RAMAZE_SUMMER_TASK_TYPE_MAX,
};

const static int MAX_MAZE_SUMMER_TASK_NUM = 10;
UNSTD_STATIC_CHECK(RAMAZE_SUMMER_TASK_TYPE_MAX <= MAX_MAZE_SUMMER_TASK_NUM);//最大拓展上线，超过需要移动数据
struct RAMazeSummerParam
{
	RAMazeSummerParam() { this->Reset(); }

	void Reset()
	{
		begin_tamp = 0;
		play_times = 0;
		reserve_ch = 0;
		has_shop = 0;
		shop_num = 0;
		for (int i = 0; i < MAX_MAZE_SUMMER_POS_X; i++)
		{
			for (int j = 0; j < MAX_MAZE_SUMMER_POS_Y; j++)
			{
				m_map[i][j].Reset();
			}
		}
		m_pos.x = 0;
		m_pos.y = 0;
		for (int i = 0; i < MAX_MAZE_SUMMER_SHOP_NUM; i++)
		{
			m_shop[i].Reset();
		}

		memset(play_info, 0, sizeof(play_info));
		task_num = 0;
		for (int i = 0; i < MAX_MAZE_SUMMER_TASK_NUM; i++)
		{
			task_info[i].Reset();
		}
	}

	void ResetGame()
	{
		has_shop = 0;
		shop_num = 0;
		for (int i = 0; i < MAX_MAZE_SUMMER_POS_X; i++)
		{
			for (int j = 0; j < MAX_MAZE_SUMMER_POS_Y; j++)
			{
				m_map[i][j].Reset();
			}
		}
		m_pos.x = 0;
		m_pos.y = 0;
		for (int i = 0; i < MAX_MAZE_SUMMER_SHOP_NUM; i++)
		{
			m_shop[i].Reset();
		}
	}

	unsigned int begin_tamp;
	short play_times;
	char reserve_ch;
	char has_shop;

	int shop_num;
	RAMazeSummerShop m_shop[MAX_MAZE_SUMMER_SHOP_NUM];

	Posi m_pos;
	RAMazeSummerMap m_map[MAX_MAZE_SUMMER_POS_X][MAX_MAZE_SUMMER_POS_Y];

	int play_info[MAX_MAZE_SUMMER_TASK_NUM];
	int task_num;
	RAMazeSummerTaskInfo task_info[MAX_MAZE_SUMMER_TASK_NUM];
};

UNSTD_STATIC_CHECK(sizeof(RAMazeSummerParam) <= sizeof(RoleActivityRoleData));

#pragma pack(pop)


#endif	//__RA_DISCOUNT_STORE_PARAM_HPP__
