#ifndef __CROSS_GAME_RA_MSG_H__
#define __CROSS_GAME_RA_MSG_H__

#include "servercommon/serverdef.h"
#include "servercommon/rankdef.hpp"
#include "servercommon/crossdef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace crossgameramsg
{
	// C - cross, G - gameworld
	enum _MsgType
	{
		MSG_INVALID = 0,

		MSG_COMMON_BEGIN = 1000,											// 基类使用的通讯号

		MSG_COMMON_ACTIVITY_STATUS_CG,										// 同步活动状态
		MSG_COMMON_FORCE_TO_NEXT_STATUS_GC,									// 强制切换到下一个活动状态
		MSG_COMMON_FORCE_SET_ACTIVITY_TIME_GC,								// 强制更改活动时间

		MSG_COMMON_END = 2000,

		MSG_ACT_BEGIN = 2001,												// 活动自定义通讯号

		MSG_CHONGZHI_RANK_SYNC_DATA_GC,										// 充值排行-同步服务器充值数据
		MSG_CHONGZHI_RANK_CHECK_REWARD_CG,									// 充值排行-通知原服结算

		MSG_CONSUME_RANK_SYNC_DATA_GC,										// 消费排行-同步服务器消费数据
		MSG_CONSUME_RANK_CHECK_REWARD_CG,									// 消费排行-通知原服结算

		MSG_SYNC_LUCKY_ROLL_GC,												// 同步转转信息到跨服
		MSG_SYNC_LUCKY_ROLL_CG,												// 跨服同步转转记录到原服

		MSG_ACT_END = 10000
 	};

	struct MessageHeader
	{
		MessageHeader(): msg_type(MSG_INVALID) {}
		MessageHeader(int _msg_type): msg_type(_msg_type) {}
		int	msg_type;
	};

	struct CGSyncActivityStatus
	{
		CGSyncActivityStatus() : header(MSG_COMMON_ACTIVITY_STATUS_CG) {}
		MessageHeader	header;

		short is_status_change;
		short activity_status;
		unsigned int begin_time;
		unsigned int end_time;
		unsigned int cross_cur_time;
	};

	struct GCForceToNextStatus
	{
		GCForceToNextStatus() : header(MSG_COMMON_FORCE_TO_NEXT_STATUS_GC) {}
		MessageHeader	header;
	};

	struct GCForceSetActivityTime
	{
		GCForceSetActivityTime() : header(MSG_COMMON_FORCE_SET_ACTIVITY_TIME_GC) {}
		MessageHeader	header;

		unsigned int begin_time;
		unsigned int end_time;
		unsigned int gameworld_cur_time;
	};


	// 以下是每个活动自定义通讯协议 =================================================================
	
	struct GCChongzhiRankSyncData
	{
		GCChongzhiRankSyncData() : header(MSG_CHONGZHI_RANK_SYNC_DATA_GC) {}
		MessageHeader	header;

		int plat_type;														// 平台类型
		int merge_server_id;												// 服务器id

		GameName mvp_name;													// 该服充值最高的玩家名字
		int mvp_server_id;													// 玩家原服务器id
		long long total_chongzhi;											// 全服总充值
	};

	struct CGChongzhiRankCheckReward
	{
		CGChongzhiRankCheckReward() : header(MSG_CHONGZHI_RANK_CHECK_REWARD_CG) {}
		MessageHeader	header;

		int rank;															// 服务器所在排名
	};

	//跨服消费排行榜活动
	struct GCConsumeRankSyncData
	{
		GCConsumeRankSyncData() : header(MSG_CONSUME_RANK_SYNC_DATA_GC) {}
		MessageHeader	header;

		int plat_type;														// 平台类型
		int merge_server_id;												// 服务器id

		GameName mvp_name;													// 该服充值最高的玩家名字
		int mvp_server_id;													// 玩家原服务器id
		long long total_consume;											// 全服总充值
	};

	struct CGConsumeRankCheckReward
	{
		CGConsumeRankCheckReward() : header(MSG_CONSUME_RANK_CHECK_REWARD_CG) {}
		MessageHeader	header;

		int rank;															// 服务器所在排名
	};

	struct CALuckyRollUserData
	{
		int plat_type;
		int merge_server_id;
		int origin_server_id;
		int role_id;
		int role_camp;
		GameName role_name;
		int gold_add_pool;            // 给奖池加的元宝数
		short roll_idx;               // 获得的奖励的序号
		short reward_type;            // 获得的奖励的类型
		short gold_reward_percentage; // 配置里的元宝奖励百分比
		short is_broadcast;			  // 是否加入抽奖结果并下发广播
		long long gold_reward_num;    // 请求时表示预计获得的元宝数， 返回时表示实际获得的元宝数
		unsigned int timestamp;		  // 请求时表示
	};

	static const int CA_MAX_SYNC_LUCKY_ROLL_RECORD_NUM = 10;

	//全服转转同步信息到跨服
	struct GCLuckyRollSyncData
	{
		GCLuckyRollSyncData() : header(MSG_SYNC_LUCKY_ROLL_GC) {}
		MessageHeader header;

		int can_get_max_gold;
		long long server_total_gold_poll;
		int user_count;
		CALuckyRollUserData user_record_list[CA_MAX_SYNC_LUCKY_ROLL_RECORD_NUM];
	};

	struct CGLuckyRollSyncData
	{
		CGLuckyRollSyncData() : header(MSG_SYNC_LUCKY_ROLL_CG) {}
		MessageHeader header;

		long long all_server_total_gold_poll;
		int user_count;
		CALuckyRollUserData user_record_list[CA_MAX_SYNC_LUCKY_ROLL_RECORD_NUM];
	};
}

#pragma pack(pop)

#endif	// __CROSS_GAME_RA_MSG_H__

