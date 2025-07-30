#ifndef __MSG_CHIEF_PEAK_BATTLE_HPP__
#define __MSG_CHIEF_PEAK_BATTLE_HPP__

#include "servercommon/activitydef.hpp"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum CHIEF_PEAK_BATTLE_REQ_TYPE
	{
		CHIEF_PEAK_BATTLE_REQ_TYPE_ROLE_INFO,		//角色信息
		CHIEF_PEAK_BATTLE_REQ_TYPE_MATCH_INFO,		//比赛信息
		CHIEF_PEAK_BATTLE_REQ_TYPE_BET,			//下注 p1 seq，p2 figher_index
		CHIEF_PEAK_BATTLE_REQ_TYPE_READY,		//准备

		CHIEF_PEAK_BATTLE_REQ_GET_NOTICE = 1000,		//获取回合信息， 服务端自动发去隐藏服
	};

	class CSChiefPeakBattleReq			//!< 2700
	{
	public:
		CSChiefPeakBattleReq();
		MessageHeader header;

		int req_type;
		int p1;
		int p2;
	};

	class SCChiefPeakBattleNotice		//!< 		2701
	{
	public:
		SCChiefPeakBattleNotice();
		MessageHeader header;

		int curr_round;				//当前回合
		int round_state;				//0是回合准备状态，1是开始
		unsigned int end_time;		//回合结束时间
	};

	class SCChiefPeakBattleMatchInfo		//!< 		2702
	{
	public:
		SCChiefPeakBattleMatchInfo();
		MessageHeader header;

		struct InfoItem
		{
			InfoItem()
			{
				role_id = 0;
				level = 0;
				avatar_type = 0;
				memset(role_name, 0, sizeof(role_name));
				match_round = 0;
				state = 0;
				headshot_id = 0;
			}

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
		int is_finish;
		InfoItem match_info[ACTIVITY_CHIEF_PEAK_BATTLE_FIGTHER_NUM];	//大小为8
	};

	class SCChiefPeakBattleRoleInfo		//!< 		2703 角色信息返回
	{
	public:
		SCChiefPeakBattleRoleInfo();
		MessageHeader header;

		int bet_type;			//下注的类型
		int role_id;
	};

	class SCChiefPeakBattleInfo		//!< 		2704 
	{
	public:
		SCChiefPeakBattleInfo();
		MessageHeader header;

		int champion_role_id;			//上次冠军
	};

	class SCChiefPeakBattleFighterFinishNotice		//!< 		2705  通知哪个人打完了,有人分出胜负
	{
	public:
		SCChiefPeakBattleFighterFinishNotice();
		MessageHeader header;

		int role_id;
		int is_win;
	};

	class SCChiefPeakBattleChampion		//!< 		2706  谁成为冠军
	{
	public:
		SCChiefPeakBattleChampion();
		MessageHeader header;
	};
}

#pragma pack(pop)

#endif