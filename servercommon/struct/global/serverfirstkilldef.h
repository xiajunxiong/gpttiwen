#ifndef __SERVER_FIRST_KILL_DEF_H__
#define __SERVER_FIRST_KILL_DEF_H__

#include "common/tlvprotocol.h"
#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int MEMBER_NAME_MAX_NUM = 5;
static const int FIRST_KILL_BOSS_TYPE_MAX_NUM = 100;
static const int TYPE_LEVEL_GROUP_SEQ_MAX_NUM = 30;
static const int SERVER_FIRST_KILL_SAVE_TO_DB_TIME = 60;	//下次保存到数据库的间隔时间
static const int FIRST_KILL_TYPE_BASE_NUM = 10000;			//底数 id = type*该值 + type_seq

struct RewardItemData
{
	RewardItemData()
	{
		this->Reset();
	}
	void Reset()
	{
		coin = 0;
		data.Reset();
	}
	struct ItemConfigPack
	{
		ItemConfigPack()
		{
			this->Reset();
		}
		void Reset()
		{
			item_id = 0;
			is_bind = 0;
			item_num = 0;
		}
		ItemID item_id;
		short is_bind;
		int item_num;
	};

	int coin;							//奖励的铜币
	ItemConfigPack data;				//奖励的道具
};

typedef char RewardItemDataHex[sizeof(RewardItemData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RewardItemDataHex) < 64);

struct ServerFirstKillEntry
{
	ServerFirstKillEntry()
	{
		this->Reset();
	}
	void Reset(bool is_deletle_video_record = true)
	{
		if (is_deletle_video_record)
		{
			battle_video_record_file_id = 0;
		}
		server_id = 0;
		type = 0;
		level_group = 0;
		seq = 0;
		monster_group_id = 0;
		member_count = 0;
		kill_timestamp = 0;
		kill_game_timestamp = 0;
		round_num = 0;
		kill_flag = 0;
		memset(member_name, 0, sizeof(member_name));
		reward_data.Reset();
		for (int i = 0; i < MEMBER_NAME_MAX_NUM; i++)
		{
			uid[i] = 0;
		}
	}
	bool Invalid()
	{
		return kill_flag < 1;
	}

	long long battle_video_record_file_id;		//对应战斗录像文件
	int server_id;								//服务器ID	//注意现如今首杀合跨服的服务器ID会一样
	short type;									//玩法类型
	short level_group;							//等级组
	short seq;									//玩法类型其中一个的排序
	short member_count;							//参与击杀玩家的人数
	short round_num;							//玩家进行的回合数
	short kill_flag;							//击杀标识
	int monster_group_id;						//怪物组ID
	long long kill_timestamp;					//击杀的时间
	long long kill_game_timestamp;													
	int uid[MEMBER_NAME_MAX_NUM];				//玩家的uid
	GameName member_name[MEMBER_NAME_MAX_NUM];	//参与击杀的玩家的名字
	RewardItemData reward_data;					//奖励数据		
};

#pragma pack(pop)

enum ServerFirstKillBossType
{
	TYPE_MIYI_BOSS = 1,					//试炼之地首领
	TYPE_CHALLENGE = 2,					//挑战任务BOSS
	TYPE_WILD_BOSS = 3,					//野外BOSS
	TYPE_SHAN_HAI_BOSS = 4,				//山海降妖
	TYPE_BIG_DIPPER = 5,				//北斗七星
	TYPE_FALLEN_GOD = 6,				//殒神之地
	TYPE_HONGMENG_TIANJIE = 7,			//鸿蒙天界

	SERVER_FIRST_KILL_TYPE_MAX,
};

static const int FIRST_KILL_TYPE_MAX = SERVER_FIRST_KILL_TYPE_MAX - 1;

class ServerFirstKillParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;																																							
	bool Unserialize(TLVUnserializer &instream);

	struct ServerFirstKillSaveEntry
	{
		ServerFirstKillSaveEntry()
		{
			this->Reset();
		}
		void Reset(bool is_deletle_video_record = true)
		{
			first_kill_info.Reset(is_deletle_video_record);
		}
		char change_state;					//改变的状态
		int id;

		ServerFirstKillEntry first_kill_info;
	};

	ServerFirstKillSaveEntry data_list[FIRST_KILL_TYPE_MAX][FIRST_KILL_BOSS_TYPE_MAX_NUM];

};

#endif 