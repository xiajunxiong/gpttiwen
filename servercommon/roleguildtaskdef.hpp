#ifndef __ROLE_GUILD_TASK_DEF_HPP__
#define __ROLE_GUILD_TASK_DEF_HPP__
#include "servercommon/servercommon.h"

#pragma pack(push, 4)


//1 = 单人挑战
//2 = 组队挑战
//3 = 打造物品
//4 = 提交道具（购买）
//5 = 抓宠物
//6 = 对话
//7 = 运镖


enum ROLE_GUILD_TASK_TYPE
{
	ROLE_GUILD_TASK_TYPE_SINGLE_BATTLE = 1,
	ROLE_GUILD_TASK_TYPE_TEAM_BATTLE = 2,
	ROLE_GUILD_TASK_TYPE_MAKE = 3,
	ROLE_GUILD_TASK_TYPE_ITEM = 4,
	ROLE_GUILD_TASK_TYPE_CATCH_PET = 5,
	ROLE_GUILD_TASK_TYPE_TALK = 6,
	ROLE_GUILD_TASK_TYPE_ESCORT = 7,
};

const static int ROLE_GUILD_TASK_MAX_NUM = 6;

struct RoleGuildTaskItem
{
	RoleGuildTaskItem()
	{
		this->Reset();
	}

	void Reset()
	{
		is_get = 0;
		is_solicit = 0;
		reserve_sh = 0;
		task_id = 0;
		curr_task_id = 0;
		refresh_time = 0;
	}

	char is_get;			//是否已经刷新了任务
	char is_solicit;
	short reserve_sh;
	int task_id;
	int curr_task_id;
	unsigned int refresh_time;		//

};

struct RoleGuildTaskParam
{
	RoleGuildTaskParam()
	{
		this->Reset();
	}

	void Reset()
	{
		finis_times = 0;
		accept_task_index = -1;
		ArrayCallReset(task_list);
	}

	int finis_times;		//今天完成的次数
	int accept_task_index;
	RoleGuildTaskItem task_list[ROLE_GUILD_TASK_MAX_NUM];
};

typedef char RoleGuildTaskParamHex[sizeof(RoleGuildTaskParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleGuildTaskParamHex) < 1024);

#pragma pack(pop)

#endif 
