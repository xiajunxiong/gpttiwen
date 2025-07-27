#ifndef __GUILD_HONOR_BATTLE_DEF_H__
#define __GUILD_HONOR_BATTLE_DEF_H__

//-----------------------------������ҫս-----------------------------------
enum ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE
{
	ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE_INVAILD,
	ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE_PERFECT_WIN,	//��ʤ
	ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE_WIN,			//ʤ��
	ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE_LOSE,			//ʧ��
	ACTIVITY_GUILD_HONOR_BATTLE_REWARD_TYPE_DRAW,				//ƽ��
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

static const int GUILD_HONOR_BATTLE_MAX_ZONE = 3;	//1��2��3
static const int GUILD_HONOR_BATTLE_FIGHTER_MAX_NUM = 16;  //�μ���̭����������������õ�
static const int GUILD_HONOR_BATTLE_MAX_ROUND = 4;  //��̭���������������õ�
static const int GUILD_HONOR_BATTLE_END_EARLY_TIME = 10;  // ս����ǰ����ʱ��(��)




#endif 
