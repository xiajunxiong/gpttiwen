#ifndef MSGCOLOSSEUM_H
#define MSGCOLOSSEUM_H

#include "gamedef.h"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/logindef.h"
#include "servercommon/colosseumdef.hpp"
#include "servercommon/struct/global/offlineregisterparam.hpp"
#include "servercommon/commondata.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	class CSColosseumReq					//!< 请求2600
	{
	public:
		CSColosseumReq();
		MessageHeader	header;

		enum REQ_TYPE
		{
			REQ_TYPE_FIHGT = 0,				//!< 战斗请求	param 发[0,3]
			REQ_TYPE_REFRESH_TARGET = 1,	//!< 刷新对手
			REQ_TYPE_ROLE_INFO = 2,			//!< 个人信息
			REQ_TYPE_TARGET_INFO = 3,		//!< 对手信息
			REQ_TYPE_TOP_INFO = 4,			//!< 前4信息
			REQ_TYPE_RANK_INFO = 5,			//!< 排行榜信息  param:rank_count
			REQ_TYPE_REPORT_INFO = 6,		//!< 战报信息
			REQ_TYPE_FETCH_EXP = 7,			//!< 领取经验	param发[0,3]
			REQ_TYPE_GET_AWARD = 8,			//!< 领取每日排名奖励
			REQ_TYPE_ACHIEVEMENT_INFO = 9,	//!< 成就信息下发
			REQ_TYPE_GET_ACHIEVEMENT = 10,	//!< 领取成就奖励 param发对应sn
			REQ_TYPE_GET_RANK_TOP_INFO = 11,//!< 第一名信息下发
			REQ_TYPE_SWEEP = 12,			//!< 扫荡 param 发[0-3]
		};

		int req_type;
		int param;
	};

	class SCColosseumRoleInfo					//!< 个人信息2601
	{
	public:
		SCColosseumRoleInfo();
		MessageHeader header;

		int is_out_of_rank_list;		//!< 是否被清理出排名表
		int rank;
		int today_fight_count;
		int fetch_exp_flag;
		int player_num_in_pool;
		int can_get_coin;
		int get_colosseum_credits;
		int capability;
		int highest_rank;
	};

	class SCColosseumTopInfo					//!< 前4信息2602
	{
	public:
		SCColosseumTopInfo();
		MessageHeader	header;

		struct Top
		{
			GameName name;
			int profession;
			short avatar_type;
			short level;
			int worship_count;
			int top_level;
		};

		int count;
		Top top_list[CHALLENGE_FIELD_TOP_NUM];
	};

	class SCColosseumTargetInfo					//!< 对手信息2603
	{
	public:
		SCColosseumTargetInfo();
		MessageHeader	header;

		struct Target
		{
			int buff_id_list[COLOSSEUM_DEFENDER_BUFF_NUM];
			GameName name;
			short rank;
			short level;
			Appearance appearance;
			int capability;
			int top_level;
			short is_can_sweep;		// 是否可扫荡
			short reserve_sh;
		};

		int count;
		Target target_list[CHALLENGE_FIELD_OPPONENT_MAX];
	};

	static const int COLOSSEUM_RANK_NUM = 50;
	class SCColosseumRankInfo					//!< 排行榜信息2604
	{
	public:
		SCColosseumRankInfo();
		MessageHeader	header;

		struct Top
		{
			GameName name;
			short profession;
			short level;
			int top_level;
			int capability;
			short headshot_id;
			short avatar_type;
		};

		int count;
		Top top_list[COLOSSEUM_RANK_NUM];
	};

	static const int COLOSSEUM_REPORT_NUM = 20;
	class SCColosseumReportInfo					//!< 战报信息2605
	{
	public:
		SCColosseumReportInfo();
		MessageHeader	header;
		
		int count;
		Report report_list[COLOSSEUM_REPORT_NUM];
	};

	class SCColosseumRankRewardInfo
	{
	public:
		SCColosseumRankRewardInfo();		//!< 2606 - 排名奖励领取概况
		MessageHeader header;

		int history_highest_rank;			//!< 历史最高排名
		long long rank_reward_fetch_flag;	//!< 排名奖励领取标记
	};

	class SCColosseumPvPsResultInfo
	{
	public:
		SCColosseumPvPsResultInfo();		//!< 2607 - 竞技场胜利结果下发
		MessageHeader header;

		int up_rank;			//!< 排名上升数值
		int new_rank;			//!< 新排名
		int add_score;			//!< 加了多少积分
		int score;
		int is_sweep;			//!< 是否是扫荡,0--否, 1--是
	};

	class SCColosseumOpenStatusInfo
	{
	public:
		SCColosseumOpenStatusInfo();		//!< 2608 - 竞技场开启状态变更通知
		MessageHeader header;

		int open_status;					//!< 古神宝库 开启状态：0 - 关闭；1 - 开启；
		long long next_switch_timestamp;	//!< 古神宝库 下次结算时间戳
	};

	class SCColosseumInPoolListInfo					//!< 2609 - 入库玩家排行榜
	{
	public:
		SCColosseumInPoolListInfo();
		MessageHeader	header;

		struct Top
		{
			GameName name;
			short profession;
			short level;
			int top_level;
		};

		UInt16 reason;		// 发送原因 \see PtcSendReason
		UInt16 count;		// 列表总长
		Top top_list[COLOSSEUM_IN_POOL_RANK_LIST_NUM];
	};

	class SCColosseumNewReport		//!< 2610 新战报通知
	{
	public:
		SCColosseumNewReport();
		MessageHeader header;

		Report report;
	};

	class SCColosseumEveryDayRewardInfo		//!< 2611 新每日奖励通知
	{
	public:
		SCColosseumEveryDayRewardInfo();
		MessageHeader header;
		int count;
		int rank_pos_list[OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST];  //!< 玩家的结算当时排名 OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST = 3
	};

	class SCColosseumNewReportDetail	//!< 2612 新战报详情
	{
	public:
		SCColosseumNewReportDetail();
		MessageHeader header;

		Report report;
	};

	class SCColosseumClientRefreshTargetNotice	//!< 2613 通知客户端请求最新对手信息
	{
	public:
		SCColosseumClientRefreshTargetNotice();
		MessageHeader header;
	};

	class SCColosseumAchievementInfo	//!< 2614 竞技场成就下发
	{
	public:
		SCColosseumAchievementInfo();
		MessageHeader header;

		unsigned int next_refresh_tamp;
		BitMap<MAX_COLOSSEUM_TASK_NUM> colosseum_task_flag;
	};

	class SCColosseumDetalInfo	//!< 2616 竞技场详细记录下发
	{
	public:
		SCColosseumDetalInfo();
		MessageHeader header;

		ColosseumParam colosseum_task_flag;
	};

	class SCHistoryHighestRankChangeNotice	//!< 2617 竞技场历史排名变化通知
	{
	public:
		SCHistoryHighestRankChangeNotice();
		MessageHeader header;

		int cur_rank;						//!< 当前排名
		int change_rank_num;				//!< 历史排名变化数值
	};

	class SCSweepResultNotice	//!< 2618 竞技场扫荡结果通知
	{
	public:
		SCSweepResultNotice();
		MessageHeader header;

		int is_sweep_success;	//1-成功,0-失败
	};

	class SCBattleRewardNotice	//2619 竞技场挑战奖励组通知
	{
	public:
		SCBattleRewardNotice();
		MessageHeader header;

		int reward_group;
	};
}

#pragma pack(pop)
#endif