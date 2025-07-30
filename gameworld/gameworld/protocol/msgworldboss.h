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
		WORLD_BOSS_REQ_TYPE_REQ_INFO,			//������Ϣ
		WORLD_BOSS_REQ_TYPE_CNACEL_INFO,		//ȡ����Ϣ
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

	class SCWorldBossResult  //!< 2202 ���������������Ϣ
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

	class SCWorldBossBattleInfo  //!< 2204 ����boss һЩս������ͳ��
	{
	public:
		SCWorldBossBattleInfo();
		MessageHeader header;

		long long team_total_damage;
		long long team_total_heal;
	};

	class SCWorldBossRoleInfo  //!< 2205 ����boss�����Ϣ
	{
	public:
		SCWorldBossRoleInfo();
		MessageHeader header;
		
		int play_times;
		int hurt;
		int heal;
	};

	class SCWorldBossRankInfo  //!< 2206 ���а���Ϣ
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

		int type;   //0���˺���1�����ư�
		int rank_count;
		RankItemInfo rank_list[ACTIVITY_WORLD_BOSS_MAX_RANK_ROLE_NUM];
	};

}

#pragma pack(pop)

#endif