#ifndef __RA_ANIMAL_RACE_PARAM_HPP__
#define __RA_ANIMAL_RACE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2061 随机活动 - 神兽径赛  //////////////////////////////////////////

const static int MAX_ANIMAL_RACE_NUM = 4;						//神兽最大数量
const static int MAX_ANIMAL_RACE_SCENE_NUM = 19;				//活动场景中数量呈现保存最多为19,加玩家自己为20
const static int MAX_ANIMAL_RACE_SC_NUM = 20;					//协议发送
const static int MAX_ANIMAL_RACE_SECOND_NUM = 50;				//神兽移动的最大秒数
const static int MAX_ANIMAL_RACE_DISTANCE_NUM = 10000;			//这里设定跑道最长距离
const static int ANIMAL_RACE_AVERAGE_SPEED_NUM = MAX_ANIMAL_RACE_DISTANCE_NUM / MAX_ANIMAL_RACE_SECOND_NUM;	//根据最大秒数与跑道长度,获得平均速度
const static int MAX_ANIMAL_RACE_EVENTHAMDLER_SECOND_NUM = 5;	//一次事件最大的持续时间

#pragma pack(push, 4)
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////


////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAAnimalRaceParam
{
	RAAnimalRaceParam() { this->Reset(); }
	void Reset()
	{
		ra_animal_race_begin_timestamp = 0;
		next_refresh_game_info_stamp = 0;
		server_id = 0;
		date_id = 0;
		memset(ra_animal_race_bet_type_count, 0, sizeof(ra_animal_race_bet_type_count));
	}

	unsigned int ra_animal_race_begin_timestamp;
	time_t next_refresh_game_info_stamp;										//下次刷新游戏的时间戳,也是发放奖励的时候	
	int date_id;																//在哪一期押注
	int	server_id;																//押注时玩家所在的服务器
	short ra_animal_race_bet_type_count[MAX_ANIMAL_RACE_NUM];					//玩家个人每个神兽的押注情况
};

UNSTD_STATIC_CHECK(sizeof(RAAnimalRaceParam) <= sizeof(RoleActivityRoleData))


#pragma pack(pop)


#endif	//__RA_ANIMAL_RACE_PARAM_HPP__
