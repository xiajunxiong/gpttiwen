#ifndef __MSG_WORLD_BOSS_HPP__
#define __MSG_WORLD_BOSS_HPP__

#include "servercommon/activitydef.hpp"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum WORLD_BOSS_REQ_TYPE
	{
		WORLD_BOSS_REQ_TYPE_REQ_INFO,			//请求信息
		WORLD_BOSS_REQ_TYPE_CNACEL_INFO,		//取消信息
	};

	class CSWorldBossReq			//!< 2200
	{
	public:
		CSWorldBossReq();
		MessageHeader header;

		int req_type;
	};

	class SCWorldBossInfo  //!< 2201
	{
	public:
		SCWorldBossInfo();
		MessageHeader header;

		int boss_npc_id;
		int boss_max_hp;
		int boss_curr_hp;
	};

	class SCWorldBossResult  //!< 2202 活动结束奖励结算信息
	{
	public:
		SCWorldBossResult();
		MessageHeader header;
		
		struct RankRoleInfo
		{
			RankRoleInfo()
			{
				role_id = 0;
				memset(role_name, 0, sizeof(role_name));
			}
			int role_id;
			GameName role_name;
		};

		RankRoleInfo hurt_rank[ACTIVITY_WORLD_BOSS_MAX_RANK_ROLE_NUM];
		RankRoleInfo heal_rank[ACTIVITY_WORLD_BOSS_MAX_RANK_ROLE_NUM];
		RankRoleInfo luck_rank[ACTIVITY_WORLD_BOSS_MAX_RANK_ROLE_NUM];
	};

	class SCWorldBossPosInfo  //!< 2203
	{
	public:
		SCWorldBossPosInfo();
		MessageHeader header;

		int boss_npc_id;
		int scene_id;
		int pos_x;
		int pos_y;
	};

	class SCWorldBossBattleInfo  //!< 2204 世界boss 一些战斗数据统计
	{
	public:
		SCWorldBossBattleInfo();
		MessageHeader header;

		long long team_total_damage;
		long long team_total_heal;
	};

	class SCWorldBossRoleInfo  //!< 2205 世界boss玩家信息
	{
	public:
		SCWorldBossRoleInfo();
		MessageHeader header;
		
		int play_times;
		int hurt;
		int heal;
	};

	class SCWorldBossRankInfo  //!< 2206 排行榜信息
	{
	public:
		SCWorldBossRankInfo();
		MessageHeader header;

		struct RankItemInfo
		{
			RankItemInfo()
			{
				role_id = 0;
				memset(role_name, 0, sizeof(role_name));
				prof = 0;
				level = 0;
				parm1 = 0;
			}
			int role_id;
			GameName role_name;
			int level;
			int prof;
			long long parm1;
		};

		int type;   //0是伤害，1是治疗榜
		int rank_count;
		RankItemInfo rank_list[ACTIVITY_WORLD_BOSS_MAX_RANK_ROLE_NUM];
	};

}

#pragma pack(pop)

#endif