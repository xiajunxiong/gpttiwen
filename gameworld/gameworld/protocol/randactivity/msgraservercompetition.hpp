#ifndef __MSG_RA_SERVER_COMPETITION_HPP__
#define __MSG_RA_SERVER_COMPETITION_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raservercompetitionparam.hpp"

#pragma pack(push, 4)

static const int MAX_SERVER_COMPETITION_RANK_USER_COUNT = 10;	// 每个排行榜最大人数

namespace Protocol
{
	// 由协议3000
	enum RA_SERVER_COMPETITION_REQ_TYPE
	{
		RA_SERVER_COMPETITION_REQ_TYPE_ROLE_ALL_INFO = 0,	// 请求下发角色信息
		RA_SERVER_COMPETITION_REQ_TYPE_FETCH_REWARD = 1,	// 请求领取奖励 P1:rank_type P2:seq
		RA_SERVER_COMPETITION_REQ_TYPE_ACTIVITY_INFO = 2,	// 请求活动信息下发
		RA_SERVER_COMPETITION_REQ_TYPE_FETCH_DAY_GIFT = 3,	// 请求领取每日礼包
		RA_SERVER_COMPETITION_REQ_TYPE_ALL_RANK_INFO = 4,	// 请求排行榜信息
	};

	struct RAServerCompetitionSingleRoleInfo
	{
		RAServerCompetitionSingleRoleInfo()
		{}

		int cur_rank_type_value;													// 该榜单当前数值
		int max_rank_type_value;													// 活动期间该榜单达到了最佳数值
		BitMap<MAX_SERVER_COMPETITION_RANK_TYPE_SEQ_NUM> reward_fetch_flag;			// 奖励领取标识
	};

	class SCRAServerCompetitionRoleAllInfo		//!< 3107 随机活动-全服比拼角色所有信息下发
	{
	public:
		SCRAServerCompetitionRoleAllInfo();
		MessageHeader header;

		char is_fetch_day_gift_flag;		// 是否领取每日礼包 0:未领取 1:已领取
		char reserve_ch;
		short reserve_sh;
		int count;
		RAServerCompetitionSingleRoleInfo rank_info_list[MAX_SERVER_COMPETITION_RANK_TYPE_NUM];
	};

	class SCRAServerCompetitionSingleRoleInfo	//!< 3108 随机活动-全服比拼角色单条信息下发
	{
	public:
		SCRAServerCompetitionSingleRoleInfo();
		MessageHeader header;

		int rank_type;
		RAServerCompetitionSingleRoleInfo rank_info;
	};

	class SCRAServerCompetitionActivityInfo		//!< 3109 随机活动-全服比拼活动信息下发
	{
	public:
		SCRAServerCompetitionActivityInfo();
		MessageHeader header;

		int count;
		unsigned int rank_type_end_timestamp_list[MAX_SERVER_COMPETITION_RANK_TYPE_NUM];		//各个排行榜结束时间戳
	};

	class SCRAServerCompetitionOtherInfo	//!< 3137 随机活动-全服比拼其它信息下发
	{
	public:
		SCRAServerCompetitionOtherInfo();
		MessageHeader header;

		char is_fetch_day_gift_flag;		// 是否领取每日礼包 0:未领取 1:已领取
		char reserve_ch;
		short reserve_sh;
	};

	struct SCRAServerCompetitionRankType
	{
		SCRAServerCompetitionRankType():uid(0), param(0)
		{}

		struct Top
		{
			Top()
			{
				memset(name, 0, sizeof(GameName));
				profession = 0;
				level = 0;
				top_level = 0;
				capability = 0;
				headshot_id = 0;
				avatar_type = 0;
			}

			GameName name;
			short profession;
			short level;
			int top_level;
			int capability;
			short headshot_id;
			short avatar_type;
		};

		int uid;
		int param;			// 评分,箍魔之塔星星等
		Top appearance_info;
	};

	class SCRAServerCompetitionAllRankInfo		//!<5137 随机活动-新服比拼全部排行榜信息
	{
	public:
		SCRAServerCompetitionAllRankInfo();
		MessageHeader header;

		SCRAServerCompetitionRankType rank_info[SEVER_COMPETITION_RANK_TYPE_MAX][MAX_SERVER_COMPETITION_RANK_USER_COUNT];
	};

}

#pragma pack(pop)

#endif	//__MSG_RA_SERVER_COMPETITION_HPP__