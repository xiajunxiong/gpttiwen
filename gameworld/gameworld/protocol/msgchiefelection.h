#ifndef __MSG_CHIEF_ELECTION_HPP__
#define __MSG_CHIEF_ELECTION_HPP__

#include "servercommon/activitydef.hpp"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum CHIEF_ELECTION_REQ_TYPE
	{
		CHIEF_ELECTION_REQ_TYPE_ROLE_INFO,		//角色信息	（没用）
		CHIEF_ELECTION_REQ_TYPE_MATCH_INFO,		//比赛信息
		CHIEF_ELECTION_REQ_TYPE_JOIN,		//报名 （没用）
		CHIEF_ELECTION_REQ_TYPE_READY,		//准备
		CHIEF_ELECTION_REQ_JOIN_TOP_INFO,		//报名的前几名信息 （没用）
		CHIEF_ELECTION_REQ_TYPE_GOTO_SEA,		//进入混战海选场景
		CHIEF_ELECTION_REQ_TYPE_OP_LOG,		//客户端log，用来排查问题
		
		CHIEF_ELECTION_REQ_GET_INFO = 1000,		//获取回合信息， 服务端自动发去隐藏服 (服务端用)
	};

	class CSChiefElectionReq			//!< 2650
	{
	public:
		CSChiefElectionReq();
		MessageHeader header;

		int req_type;
		int p1;
		int p2;
	};

	class SCChiefElectionNotice		//!< 		2651
	{
	public:
		SCChiefElectionNotice();
		MessageHeader header;

		int curr_round;				//当前回合
		int round_state;				//0是回合准备状态，1是开始
		unsigned int end_time;		//回合结束时间
	};

	class SCChiefElectionMatchInfo		//!< 		2652
	{
	public:
		SCChiefElectionMatchInfo();
		MessageHeader header;

		struct InfoItem
		{
			InfoItem()
			{
				prof = 0;
				role_id = 0;
				level = 0;
				avatar_type = 0;
				headshot_id = 0;
				memset(role_name, 0, sizeof(role_name));
				match_round = 0;
				state = 0;
			}

			int prof;
			int role_id;					//0说明该项无效
			int level;
			short avatar_type;
			short headshot_id;
			GameName role_name;
			int match_round;		//到达的回合
			int state;				//1：输了，0：	match_round>curr_round 赢了 或 match_round==curr_round  未分出胜负
		};

		int curr_round;				//当前回合	
		int round_state;				//0是回合准备状态，1是开始
		unsigned int end_time;		//回合结束时间
		int is_finish;				//是否完成
		int  match_info_count;
		InfoItem match_info[ACTIVITY_CHIEF_ELECTION_FIGTHER_NUM];	
	};

	class SCChiefElectionRoleInfo		//!< 		2653
	{
	public:
		SCChiefElectionRoleInfo();
		MessageHeader header;

		int is_join;			//是否报名了
	};

	class SCChiefElectionWinRoleInfo		//!< 		2654  胜出人的roleid，可以参加巅峰的人
	{
	public:
		SCChiefElectionWinRoleInfo();
		MessageHeader header;
		
		ChiefElectionWinnerInfo winner_info[ACTIVITY_CHIEF_ELECTION_JOIN_PROF_NUM];
	};

	class SCChiefElectionJoinTopRoleInfo	//!< 		2655
	{
	public:
		SCChiefElectionJoinTopRoleInfo();
		MessageHeader header;

		struct InfoItem
		{
			InfoItem()
			{
				role_id = 0;
				capability = 0;
				avatar_type = 0;
				level = 0;
				headshot_id = 0;
				memset(role_name, 0, sizeof(role_name));
			}

			int role_id;					//0说明该项无效
			int capability;	
			short avatar_type;				
			short headshot_id;				//头像ID
			int level;
			GameName role_name;
		};

		InfoItem match_info[ACTIVITY_CHIEF_ELECTION_FIGTHER_NUM];	//大小为
	};

	class SCChiefElectionFighterFinishNotice		//!< 		2656  通知哪个人打完了,有人分出胜负
	{
	public:
		SCChiefElectionFighterFinishNotice();
		MessageHeader header;

		int role_id;								
		int is_win;
	};

	class SCChiefElectionChampion		//!< 		2657  谁成为冠军
	{
	public:
		SCChiefElectionChampion();
		MessageHeader header;
	};

	class SCChiefElectionSeaRoleInfo	//!< 		2658 混战角色信息
	{
	public:
		SCChiefElectionSeaRoleInfo();
		MessageHeader header;

		int socre;
		int win_times;
		int lose_times;
		int total_times;
	};

	class SCChiefElectionSeaSceneInfo	//!< 		2659 混战场景信息
	{
	public:
		SCChiefElectionSeaSceneInfo();
		MessageHeader header;

		int role_num;					//当前场景人数
	};

	class SCChiefElectionStateInfo	//!< 		2660 首席活动状态，登录或者改变了发
	{
	public:
		SCChiefElectionStateInfo();
		MessageHeader header;
			
		int which_prof_match;
		int match_state;				//活动状态，收到 ACTIVITY_CHIEF_ELECTION_STATE_SEA_ELECTION_FINISH ，混战结束
		unsigned int next_match_state_time;		//切换活动状态时间戳
	};

	class SCChiefElectionSeaRank	//!< 		2661 混战排行榜
	{
	public:
		SCChiefElectionSeaRank();			//刷新后自动下发
		MessageHeader header;

		struct Item
		{
			int role_id;
			GameName role_name;
			int socre;
			int win_times;
			int lose_times;
			int total_times;
		};

		int count;
		Item rank_list[100];
	};
}

#pragma pack(pop)

#endif