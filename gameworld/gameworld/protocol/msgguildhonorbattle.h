#ifndef __MSG_GUILD_HONOR_BATTLE_HPP__
#define __MSG_GUILD_HONOR_BATTLE_HPP__

#include "servercommon/activitydef.hpp"
#include "servercommon/guilddef.hpp"
#include "servercommon/guildhonorbattledef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum GUILD_HONOR_REQ_TYPE
	{
		GUILD_HONOR_REQ_TYPE_ASSIGN,			//p1 分配到区域（0,1,2,3);  P2 role_id
		GUILD_HONOR_REQ_TYPE_JOIN,			// 到活动场景
		GUILD_HONOR_REQ_TYPE_AUTO_ASSIGN,	// 自动分配
		GUILD_HONOR_REQ_TYPE_OB,				// 观战 p1 zond_id
	};

	class CSGuildHonorReq			//!< 2750
	{
	public:
		CSGuildHonorReq();
		MessageHeader header;

		int req_type;
		int p1;
		int p2;
	};

	struct MsgGuildHonorRoleItemInfo
	{
		int role_id;
		GameName role_name;
		int level;
		short prof;
		short zone_id;
	};

	class SCGuildHonorRoleInfo		//!< 		2751		
	{
	public:
		SCGuildHonorRoleInfo();
		MessageHeader header;

		int count;
		MsgGuildHonorRoleItemInfo role_info[GUILD_MAX_MEMBER_COUNT];	//256
	};

	class SCGuildHonorRoleChange		//!< 		2752		
	{
	public:
		SCGuildHonorRoleChange();
		MessageHeader header;

		short change_reson;		
		short zone_id;		
		int role_id;
	};

	class SCGuildHonorAddRoleInfo		//!< 		2754		
	{
	public:
		SCGuildHonorAddRoleInfo();
		MessageHeader header;

		MsgGuildHonorRoleItemInfo role_info;	
	};

	class SCGuildHonorZoneBattleResultInfo		//!< 		2755	  每次战斗结果产生时候发
	{
	public:
		SCGuildHonorZoneBattleResultInfo();		
		MessageHeader header;

		int zone_id;
		int win_guild_id;  //0是未分出胜负。-1是平手，不然就是胜利的guild_id
	};



	class SCGuildHonorAllZoneBattleResultInfo		//!< 2756	 进入场景发
	{
	public:
		SCGuildHonorAllZoneBattleResultInfo();
		MessageHeader header;

		struct GuildInfoItem
		{
			GuildInfoItem()
			{
				guild_id = 0;
				memset(guild_name, 0, sizeof(guild_name));
			}
			int guild_id;
			GuildName guild_name;
		};

		GuildInfoItem guild_info[2];
		int zone_battle_result[GUILD_HONOR_BATTLE_MAX_ZONE];  //0是未分出胜负。-1是平手，不然就是胜利的guild_id
	};


	class SCGuildHonorAllZoneRoleNumInfo		//!< 		2757	 战斗和观战界面，增加一个UI，显示“剩余未参战人数”。发生改变和进入场景发
	{
	public:
		SCGuildHonorAllZoneRoleNumInfo();
		MessageHeader header;

		int zone_role_num[GUILD_HONOR_BATTLE_MAX_ZONE]; 
	};

	enum GUILD_HONOR_REQ_TYPE_2
	{
		GUILD_HONOR_REQ_TYPE_2_SCORE_INFO,		// 请求某个家族的家族大乱斗一星期的分数  p1 guild_id
		GUILD_HONOR_REQ_TYPE_2_SCORE_RANK,		// 大乱斗星期的分数排行榜
		GUILD_HONOR_REQ_TYPE_2_GUESSING,			// 竞猜 p1 guild_id p2 win_rank
		GUILD_HONOR_REQ_TYPE_2_CANCEL_GUESSING,			// 取消竞猜 p1 guild_id p2 win_rank
		GUILD_HONOR_REQ_TYPE_2_MATCH_RECORD_INFO,			// 上次比赛信息
		GUILD_HONOR_REQ_TYPE_2_GUESSING_INFO,			// 请求竞猜类型
		GUILD_HONOR_REQ_TYPE_2_GUESSING_CHAMPION,			// 冠军竞猜类型p1 guild_id 
		GUILD_HONOR_REQ_TYPE_2_CANCEL_GUESSING_CHAMPION,			// 取消冠军竞猜 p1 guild_id 
	};

	class CSGuildHonorReq2			//!< 2758
	{
	public:
		CSGuildHonorReq2();
		MessageHeader header;

		int req_type;
		int p1;
		int p2;
	};

	class SCGuildHonorBattleProcessInfo		//!< 		2759  活动进程 ，跨服没连接不会下发
	{
	public:
		SCGuildHonorBattleProcessInfo();
		MessageHeader header;

		int process;			//GUILD_HONOR_BATTLE_PROCESS
		unsigned int settlement_list_time;		//名单时间
	};

	class SCGuildHonorBattleScoreInfo		//!< 		2760  某个家族的家族大乱斗一星期的分数
	{
	public:
		SCGuildHonorBattleScoreInfo();
		MessageHeader header;

		int guild_id;

		unsigned short join_guild_fight_day_flag;	//参加了大乱斗星期几的标志
		unsigned short score[7];				//整个星期每天的分数，index 0是星期天	
	};

	class SCGuildHonorScoreRankInfo		//!< 		2761 大乱斗每星期的分数排行榜
	{
	public:
		SCGuildHonorScoreRankInfo();
		MessageHeader header;

		struct ItemInfo
		{
			int server_id;
			int guild_id;
			GuildName name;
			GuildBanner banner;

			unsigned short join_guild_fight_day_flag;	//参加了大乱斗星期几的标志
			unsigned short score[7];				//整个星期每天的分数，index 0是星期天
		};


		int count;
		ItemInfo item[50];
	};

	struct GuildHonorBattleMatchInfoItem 
	{
		struct FigtherInfoItem
		{
			int number;
			int server_id;
			int guild_id;
			GuildName guild_name;
			GuildBanner banner;
			GameName role_name;	//族长名字
			int guild_fight_rank;				//家族大乱斗排名

			int match_round;		//到达的回合，和curr_round 不同
			char is_finish;			//match_round 回合是否完成战斗
			char is_lose;			//match_round 回合是不是输了
			unsigned short last_match_rank;				//0 没有，上次比赛排名

			int champion_guess_num;		//选冠军支持的人数
			int guess_num[GUILD_HONOR_BATTLE_MAX_ROUND];		//每轮支持的人数
			int battle_id[GUILD_HONOR_BATTLE_MAX_ZONE];		//每个区域观战id
			int battle_result[GUILD_HONOR_BATTLE_MAX_ZONE];  //每个区域的战斗结果，用来显示某个区域是否能观战
		};

		int start_first_round;			//这组开始时，第一组回合
		int win_rank;				//胜利排名
		int curr_round;				//当前回合	
		int round_state;				//0是回合准备状态，1是开始
		unsigned int next_round_state_time;		//回合结束时间
		int is_finish;				//是否完成
		int  fighter_num;
		FigtherInfoItem figther_info[GUILD_HONOR_BATTLE_FIGHTER_MAX_NUM];
	};

	class SCGuildHonorBattleMatchInfo		//!< 		2762  赛况信息
	{
	public:
		SCGuildHonorBattleMatchInfo();
		MessageHeader header;

		int match_info_count;			//比赛信息数量
		GuildHonorBattleMatchInfoItem match_info[16];		//比赛信息
	};

	class SCGuildHonorBattleGuessInfo		//!< 		2763  竞猜信息
	{
	public:
		SCGuildHonorBattleGuessInfo();
		MessageHeader header;

		struct Item
		{
			short win_rank;
			short round;
			int guild_id;
		};

		int champion_guild_id;
		int guess_item_count;
		Item guess_item[32];
	};

	class SCGuildHonorBattleMatchRecord		//!< 		2764  上个比赛赛况
	{
	public:
		SCGuildHonorBattleMatchRecord();
		MessageHeader header;

		struct InfoItem
		{
			short start_first_round;;
			short win_rank;
			int number;
			int server_id;
			int guild_id;
			GuildName guild_name;
			GuildBanner banner;
			GameName role_name;	//族长名字
			int guild_fight_rank;				//家族大乱斗排名
			int match_round;		//到达的回合，和curr_round 不同
			int champion_guess_num;		//选冠军支持的人数
			int guess_num[GUILD_HONOR_BATTLE_MAX_ROUND];		//每轮支持的人数
		};


		int  fighter_num;
		InfoItem figther_info[128];
	};

	class SCGuildHonorBattleTopInfo		//!< 		2765  buff
	{
	public:
		SCGuildHonorBattleTopInfo();
		MessageHeader header;
		const int static ITEM_NUM = 3;

		int guild_id[ITEM_NUM];
		GameName guild_name[ITEM_NUM];
	};

	class SCGuildHonorRoleBattleState			//2766  战斗状态
	{
	public:
		SCGuildHonorRoleBattleState();
		MessageHeader header;


		int state;
	};
}

#pragma pack(pop)

#endif