#ifndef __MSG_SHAN_HAI_BOSS_HPP__
#define __MSG_SHAN_HAI_BOSS_HPP__

#include "servercommon/activitydef.hpp"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/shanhaibossdef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	struct CSShanHaiBossReq					// 3370 山海降妖npc信息请求
	{
		CSShanHaiBossReq();
		MessageHeader header;

		enum REQ_TYPE
		{
			REQ_TYPE_BOSS_INFO = 0,			//请求BOSS信息
			REQ_TYPE_ROLE_INFO = 1,			//请求玩家信息
		};

		short req_type;
		short param;
	};

	class SCShanHaiBossInfo					//!<3371 山海信息
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

	class SCShanHaiBossRoleInfo				//!<3372 玩家信息
	{
	public:
		SCShanHaiBossRoleInfo();
		MessageHeader header;

		unsigned int next_refresh_time;
		int today_times;				// 今天挑战次数
	};


	/*struct SCShanHaiBossPokerInfo			// 3373	山海降妖翻牌奖励信息
	{
		SCShanHaiBossPokerInfo();
		MessageHeader header;

		int m_draw_group_id;
		unsigned timeout;
		ShanHaiBossPoker poker_data[SIDE_MAX_ROLE_NUM];		//SIDE_MAX_ROLE_NUM - 5
	};*/

	class SCShanHaiBossSingleInfo			//!<3374 单个boss信息
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
