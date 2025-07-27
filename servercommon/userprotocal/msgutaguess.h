#ifndef __MSG_UTA_GUESS_H__
#define __MSG_UTA_GUESS_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/userprotocal/msgcode.h"
#include "servercommon/commondata.hpp"

#include "servercommon/struct/bigcross/utaguess/utaguessparam.hpp"


#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum UTA_GUESS_CLIENT_REQ_TYPE
	{
		UTA_GUESS_CLIENT_REQ_TYPE_GET_INFO = 0,		//!< 获取单场竞猜的信息 param1: match_idx比赛序号
		UTA_GUESS_CLIENT_REQ_TYPE_BET = 1,			//!< 竞猜下注 
													//!< param1: match_idx比赛序号 
													//!< param2: team_idx队伍序号
													//!< param3: 下注的金额

		UTA_GUESS_CLIENT_REQ_TYPE_GET_RECORD = 2,	//!< 获取自己下注过的比赛记录
		UTA_GUESS_CLIENT_REQ_TYPE_GET_BET_REWARD = 3,	//!< 获取竞猜奖励 
														//!< param1:[-1:一键领取 ,非-1为match_idx比赛序号 ]
														//!< param2:team_idx队伍序号[param1非-1时有用]
		
		UTA_GUESS_CLIENT_REQ_TYPE_GET_HISTORY_EARNING = 4, //!< 获取累计收益
	};

	struct CSUTAGuessClienReq				// 4968  天下第一比武[组队]竞猜 客户端操作
	{
		CSUTAGuessClienReq() :header(MT_UTA_GUESS_CLIENT_REQ_CS) {}
		MessageHeader header;

		int req_type;						// 操作类型
		long long param_1;
		long long param_2;
		int param_3;
	};

	struct SCUTAGuessMatchInfo			// 4969 某一个比赛的竞猜信息
	{
		struct TeamInfo
		{
			TeamInfo():team_seq(-1), plat_id(-1), server_id(-1), total_bet_num(0), guess_status(0), winning(0),
				win_rate(0), bet_coin_num(0)
			{
				memset(prof, 0, sizeof(prof));
			}

			long long team_seq;								//!< 下注的队伍序号
			int plat_id;									//!< 所属平台ID
			int server_id;									//!< 所属服务器ID
			int total_bet_num;								//!< 队伍的总的被下注的金额
			int guess_status;								//!< 竞猜状态  [0:可以下注 , 1:停止下注 , 2:开奖[获胜] , 3开奖[失败]]
			int prof[MAX_TEAM_MEMBER_NUM];					//!< 队伍的职业信息
			int winning;									//!< 连胜场次
			int win_rate;									//!< 胜率
			int bet_coin_num;								//!< 玩家对比赛双方的下注信息
		};

		SCUTAGuessMatchInfo() :header(MT_UTA_GUESS_MATCH_INFO_SC) {}
		MessageHeader header;

		long long match_idx;
		TeamInfo team_info[MAX_MATCH_TEAM_NUM];		// 比赛双方队伍的下注信息				
	};

	struct SCUTAGuessBetRecord				// 4970 玩家参与下注的列表
	{
		struct TeamInfo
		{
			TeamInfo() :team_idx(0), bet_coin_num(0), is_get(0), rate(0), leader_headshot_id(0), leader_avatar_type(0)
			{
				memset(team_name, 0, sizeof(team_name));
			}

			long long team_idx;								//!< 队伍ID
			GameName team_name;								//!< 队伍名
			int bet_coin_num;								//!< 下注的金额
			int is_get;										//!< 是否已领取
			int rate;										//!< 赔率
			short leader_headshot_id;						//!< 队长头像
			short leader_avatar_type;						//!< 队长模型类型
		};

		struct MatchInfo
		{
			MatchInfo():match_idx(0), type(0), fight_result(0), round_idx(0), is_win_group(0) {}

			long long match_idx;		//!< 比赛ID	
			char type;					//!< 比赛类型  0:资格赛  1:正赛
			char fight_result;			//!< 比赛结果 0:未赛  1:攻方胜   2:守方胜  3: 在打 , 4:平局
			char round_idx;				//!< 轮次 （与type一起确定开始时间）
			char is_win_group;			//!< 是否胜者组（当正赛时有效）
		
			TeamInfo team_info[MAX_MATCH_TEAM_NUM];
		};

		SCUTAGuessBetRecord() :header(MT_UTA_GUESS_BET_RECORD_SC) {}
		MessageHeader header;

		int is_add;				// 是否是添加[如果下注的比赛特别多,获取时可能会收到几条协议]
		int count;
		MatchInfo match_info[MAX_ONCE_UTA_GUESS_COUNT];
	};

	struct SCUTAGuessHistoryEarning	//!< 4971 竞猜累计收益
	{
		SCUTAGuessHistoryEarning() : header(MT_UTA_GUESS_HISTORY_EARNING_SC) {}
		MessageHeader header;

		int history_earning;	//!< 累计收益
	};

	struct SCUTAGuessHasBetRewardNotice //!< 4972 有可领取竞猜奖励通知
	{
		SCUTAGuessHasBetRewardNotice() : header(MT_UTA_GUESS_HAS_BET_REWARD_SC) {}
		MessageHeader header;
	};
}

#pragma pack(pop)

#endif // __MSG_UTA_GUESS_H__

