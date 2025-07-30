#ifndef __MSG_SHAN_HAI_BOSS_HPP__
#define __MSG_SHAN_HAI_BOSS_HPP__

#include "servercommon/activitydef.hpp"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/shanhaibossdef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	struct CSShanHaiBossReq					// 3370 ɽ������npc��Ϣ����
	{
		CSShanHaiBossReq();
		MessageHeader header;

		enum REQ_TYPE
		{
			REQ_TYPE_BOSS_INFO = 0,			//����BOSS��Ϣ
			REQ_TYPE_ROLE_INFO = 1,			//���������Ϣ
		};

		short req_type;
		short param;
	};

	class SCShanHaiBossInfo					//!<3371 ɽ����Ϣ
	{
	public:
		SCShanHaiBossInfo();
		MessageHeader header;

		struct NpcInfo
		{
			int npc_id;
			int scene_id;
			int death_num;
		};

		unsigned int next_refresh_time;
		int count;
		NpcInfo boss_info[SHAN_HAI_BOSS_MAXNUM];
	};

	class SCShanHaiBossRoleInfo				//!<3372 �����Ϣ
	{
	public:
		SCShanHaiBossRoleInfo();
		MessageHeader header;

		unsigned int next_refresh_time;
		int today_times;				// ������ս����
	};


	/*struct SCShanHaiBossPokerInfo			// 3373	ɽ���������ƽ�����Ϣ
	{
		SCShanHaiBossPokerInfo();
		MessageHeader header;

		int m_draw_group_id;
		unsigned timeout;
		ShanHaiBossPoker poker_data[SIDE_MAX_ROLE_NUM];		//SIDE_MAX_ROLE_NUM - 5
	};*/

	class SCShanHaiBossSingleInfo			//!<3374 ����boss��Ϣ
	{
	public:
		SCShanHaiBossSingleInfo();
		MessageHeader header;

		struct NpcInfo
		{
			int npc_id;
			int scene_id;
			int death_num;
		};

		NpcInfo boss_info;
	};
}

#pragma pack(pop)

#endif
