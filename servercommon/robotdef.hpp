#ifndef __ROBOT_DEF_HPP__
#define __ROBOT_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/struct/global/robotaiparam.hpp"

#pragma pack(push, 4)

enum ROBOT_TYPE
{
	ROBOT_TYPE_NORMAL = 0, // 没有特殊逻辑的机器人
	ROBOT_TYPE_TASK_ROBOT = 1,	// 任务机器人
	ROBOT_TYPE_MISSION_ROBOT = 2,	// 主线机器人
	ROBOT_TYPE_RANGER_ROBOT = 3,	// 巡逻机器人
	ROBOT_TYPE_STATIC_ROBOT = 4,	// 静态机器人

	ROBOT_TYPE_MAX,
};

enum ROBOT_CONFIG_USE_TYPE
{
	ROBOT_CONFIG_USE_TYPE_ZHUZHAN = 0,		// 助战机器人 
	ROBOT_CONFIG_USE_TYPE_JJC_AND_PLATFORM = 1,	// JJC和擂台战机器人
	ROBOT_CONFIG_USE_TYPE_ROLE_SHADOW = 2,		// 玩家外观拷贝机器人
	ROBOT_CONFIG_USE_TYPE_MISSION_ROBOT = 3,// 主线机器人
	ROBOT_CONFIG_USE_TYPE_RANGER_ROBOT = 4,  // 巡逻机器人
	ROBOT_CONFIG_USE_TYPE_STATIC_ROBOT = 5,  // 静态机器人
	ROBOT_CONFIG_USE_TYPE_MAX,
};

struct RobotParam
{
	RobotParam() { this->Reset(); }

	void Reset()
	{
		plat_type = 0;
		robot_id = 0;
		robot_type = 0;
		memset(robot_name, 0, sizeof(robot_name));

		level = 0;
		profession = 0;
		avatar_type = 0;
		role_battle_data.Reset();

		bound_team_type = 0;
		bound_battle_mode = 0;
		//team_index = 0;
		robot_ai_param.Reset();
		real_role_uid = 0;
	}

	int plat_type;
	int robot_id;
	int robot_type;
	GameName robot_name;

	int level;
	int profession;
	int avatar_type;

	int bound_team_type;
	int bound_battle_mode;
	int real_role_uid;
	//int team_index;

	RoleBattleData role_battle_data;

	RobotAIParam robot_ai_param;
};

#pragma pack(pop)

#endif