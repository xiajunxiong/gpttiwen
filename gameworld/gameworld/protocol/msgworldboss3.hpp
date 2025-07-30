#ifndef __MSG_WORLD_BOSS_3_HPP__
#define __MSG_WORLD_BOSS_3_HPP__

#include "servercommon/activitydef.hpp"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum WORLD_BOSS_3_REQ_TYPE
	{
		WORLD_BOSS_3_REQ_TYPE_REQ_INFO,			//������Ϣ
		WORLD_BOSS_3_REQ_TYPE_CNACEL_INFO,		//ȡ����Ϣ
	};

	class CSWorldBoss3Req			//!< 3210 ����BOSS3 ��������
	{
	public:
		CSWorldBoss3Req();
		MessageHeader header;

		int req_type;
	};

	class SCWorldBoss3Info  //!< 3211 ����BOSS3 ��Ϣ
	{
	public:
		SCWorldBoss3Info();
		MessageHeader header;

		int boss_npc_id;
		int boss_max_hp;
		int boss_curr_hp;
	};

	class SCWorldBoss3Result  //!< 3212 ���������������Ϣ
	{
	public:
		SCWorldBoss3Result();
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

		RankRoleInfo hurt_rank[ACTIVITY_WORLD_BOSS_3_MAX_RANK_ROLE_NUM];
		RankRoleInfo heal_rank[ACTIVITY_WORLD_BOSS_3_MAX_RANK_ROLE_NUM];
		RankRoleInfo luck_rank[ACTIVITY_WORLD_BOSS_3_MAX_RANK_ROLE_NUM];
	};

	class SCWorldBoss3PosInfo  //!< 3213 ����BOSS3 λ����Ϣ
	{
	public:
		SCWorldBoss3PosInfo();
		MessageHeader header;

		int boss_npc_id;
		int scene_id;
		int pos_x;
		int pos_y;
	};

	class SCWorldBoss3BattleInfo  //!< 3214 ����boss һЩս������ͳ��
	{
	public:
		SCWorldBoss3BattleInfo();
		MessageHeader header;

		long long team_total_damage;
		long long team_total_heal;
	};

	class SCWorldBoss3RoleInfo  //!< 3215 ����boss�����Ϣ
	{
	public:
		SCWorldBoss3RoleInfo();
		MessageHeader header;

		int play_times;
		int hurt;
		int heal;
	};

	class SCWorldBoss3RankInfo  //!< 3216 ���а���Ϣ
	{
	public:
		SCWorldBoss3RankInfo();
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
		RankItemInfo rank_list[ACTIVITY_WORLD_BOSS_3_MAX_RANK_ROLE_NUM];
	};

}

#pragma pack(pop)

#endif