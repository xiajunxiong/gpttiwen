#ifndef __MSG_WORLD_BOSS_2_HPP__
#define __MSG_WORLD_BOSS_2_HPP__

#include "servercommon/activitydef.hpp"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum WORLD_BOSS_2_REQ_TYPE
	{
		WORLD_BOSS_2_REQ_TYPE_REQ_INFO,			//������Ϣ
		WORLD_BOSS_2_REQ_TYPE_CNACEL_INFO,		//ȡ����Ϣ
	};

	class CSWorldBoss2Req			//!< 3200
	{
	public:
		CSWorldBoss2Req();
		MessageHeader header;

		int req_type;
	};

	class SCWorldBoss2Info  //!< 3201
	{
	public:
		SCWorldBoss2Info();
		MessageHeader header;

		int boss_npc_id;
		int boss_max_hp;
		int boss_curr_hp;
	};

	class SCWorldBoss2Result  //!< 3202 ���������������Ϣ
	{
	public:
		SCWorldBoss2Result();
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

		RankRoleInfo hurt_rank[ACTIVITY_WORLD_BOSS_2_MAX_RANK_ROLE_NUM];
		RankRoleInfo heal_rank[ACTIVITY_WORLD_BOSS_2_MAX_RANK_ROLE_NUM];
		RankRoleInfo luck_rank[ACTIVITY_WORLD_BOSS_2_MAX_RANK_ROLE_NUM];
	};

	class SCWorldBoss2PosInfo  //!< 3203
	{
	public:
		SCWorldBoss2PosInfo();
		MessageHeader header;

		int boss_npc_id;
		int scene_id;
		int pos_x;
		int pos_y;
	};

	class SCWorldBoss2BattleInfo  //!< 3204 ����boss һЩս������ͳ��
	{
	public:
		SCWorldBoss2BattleInfo();
		MessageHeader header;

		long long team_total_damage;
		long long team_total_heal;
	};

	class SCWorldBoss2RoleInfo  //!< 3199 ����boss2 �����Ϣ
	{
	public:
		SCWorldBoss2RoleInfo();
		MessageHeader header;

		int play_times;
		int hurt;
		int heal;
	};

	class SCWorldBoss2RankInfo  //!< 3198 ���а���Ϣ  ����boss
	{
	public:
		SCWorldBoss2RankInfo();
		MessageHeader header;

		struct RankItemInfo
		{
			RankItemInfo()
			{
				role_id = 0;
				memset(role_name, 0, sizeof(role_name));
				level = 0;
				prof = 0;
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
		RankItemInfo rank_list[ACTIVITY_WORLD_BOSS_2_MAX_RANK_ROLE_NUM];
	};
}

#pragma pack(pop)

#endif