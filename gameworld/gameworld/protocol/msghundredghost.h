#ifndef __MSG_HUNDRED_GHOST_H__
#define __MSG_HUNDRED_GHOST_H__

#include "servercommon/activitydef.hpp"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/struct/global/drawdef.hpp"
#include "servercommon/struct/battle/battle_def.hpp"

static const int HUNDRED_GHOST_BOSS_MAXNUM = 50;

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	struct CSHundredGhostReq					// 3940 �ٹ�ҹ��npc��Ϣ����
	{
		CSHundredGhostReq();
		MessageHeader header;

		enum REQ_TYPE
		{
			REQ_TYPE_BOSS_INFO = 0,			//����BOSS��Ϣ
		};

		short req_type;
		short param;
	};

	class SCHundredGhostInfo					//!<3941 �ٹ�ҹ��npc��Ϣ
	{
	public:
		SCHundredGhostInfo();
		MessageHeader header;

		struct NpcInfo
		{
			int npc_id;
			int scene_id;
		};

		short surplus_num;			//ʣ��boss����
		short count;
		NpcInfo boss_info[HUNDRED_GHOST_BOSS_MAXNUM];
	};

	struct HundredGhostPoker
	{
		HundredGhostPoker() { this->Reset(); }

		void Reset()
		{
			index = -1;
			role_uid = 0;
			memset(role_name, '\0', sizeof(role_name));
			reward_info.Reset();
		}

		int index;							// �������Ƶ�index
		int role_uid;						// UID
		GameName role_name;					// ��ɫ����

		ItemDrawInfo reward_info;
	};
	struct SCHundredGhostPokerInfo			// 3942	�ٹ�ҹ�з��ƽ�����Ϣ
	{
		SCHundredGhostPokerInfo();
		MessageHeader header;

		int draw_group_id;
		unsigned timeout;
		HundredGhostPoker poker_data[SIDE_MAX_ROLE_NUM];		//SIDE_MAX_ROLE_NUM - 5
	};
}

#pragma pack(pop)
#endif


