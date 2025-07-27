#ifndef __GUILD_HONOR_BATTLE_DEF_H__
#define __GUILD_HONOR_BATTLE_DEF_H__

//-----------------------------家族荣耀战-----------------------------------
enum ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE
{
	ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE_INVAILD,
	ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE_PERFECT_WIN,	//完胜
	ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE_WIN,			//胜利
	ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE_LOSE,			//失败
	ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE_DRAW,				//平手
};

struct GuildHonorBattleAssignItem
{
	GuildHonorBattleAssignItem()
	{
		zone_id = 0;
		role_id = 0;
	}

	int zone_id;
	int role_id;
};

static const int GUILD_HONOR_BATTLE_MAX_ZONE = 3;	//1，2，3
static const int GUILD_HONOR_BATTLE_FIGHTER_MAX_NUM = 16;  //参加淘汰赛的最大个数，存库用到
static const int GUILD_HONOR_BATTLE_MAX_ROUND = 4;  //淘汰赛最大轮数，存库用到
static const int GUILD_HONOR_BATTLE_END_EARLY_TIME = 10;  // 战斗提前结束时间(秒)




#endif 
