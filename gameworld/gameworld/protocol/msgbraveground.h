#ifndef __MSG_BRAVE_GROUND_HPP__
#define __MSG_BRAVE_GROUND_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/commonsavedef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum BRAVE_GROUND_REQ_TYPE
	{
		BRAVE_GROUND_REQ_TYPE_RANK_INFO,			//请求排行榜信息
		BRAVE_GROUND_REQ_TYPE_USE_NPC,			// 使用npc，p1 npc的objid
		BRAVE_GROUND_REQ_TYPE_INFO,			// 信息
		BRAVE_GROUND_REQ_TYPE_ROUTE,			// 操作转发
		BRAVE_GROUND_REQ_TYPE_SELECT_LEVEL,			// 跳关
		BRAVE_GROUND_REQ_TYPE_SAO_DANG,			// 扫荡
	};

	class CSBraveGroundReq			//!< 2520
	{
	public:
		CSBraveGroundReq();
		MessageHeader header;

		int req_type;
		int p1;
		int p2;
	};

	class SCBraveGroundRankInfo			//!< 2521
	{
	public:
		SCBraveGroundRankInfo();
		MessageHeader header;

		const static int MAX_RANK_ITEM = 100;

		int count;
		CommonSaveBraveGroundTeamReceordData rank_list[MAX_RANK_ITEM];
	};

	class SCBraveGroundSceneInfo			//!< 2522		
	{
	public:
		SCBraveGroundSceneInfo();
		MessageHeader header;

		int curr_level;
		int buff_id;
		int is_kill_boss;
	};

	class SCBraveGroundLevelRewardInfo			//!< 2523		
	{
	public:
		SCBraveGroundLevelRewardInfo();
		MessageHeader header;

		int start_level;
		int end_level;			//  [ start_level, end_level )
	};

	class SCBraveGroundInfo			//!< 2524		
	{
	public:
		SCBraveGroundInfo();
		MessageHeader header;

		int reached_level;			//!< 到达层数
	};

	class SCBraveGroundRoute			//!< 2525		
	{
	public:
		SCBraveGroundRoute();
		MessageHeader header;

		int param1;
		int param2;
	};

	class SCBraveGroundNotify			//!< 2526		勇闯地宫通知			
	{
	public:
		SCBraveGroundNotify();
		MessageHeader header;

		int p1;					//层数
	};

	class SCBraveGroundUseItemInfo						//!< 勇闯地宫场景玩家使用道具信息 2527
	{
	public:
		SCBraveGroundUseItemInfo();
		MessageHeader		header;

		struct UseItemInfo
		{
			UseItemInfo()
			{
				use_type = 0;
				use_times = 0;
			}

			int use_type;
			int use_times;
		};

		int count;
		UseItemInfo use_info[ACTIVITY_BRAVE_GROUND_LIMIT_ITEM_MAX];		//大小100
	};

	class SCBraveGroundTeamInfo						//!< 勇闯地宫队伍玩家信息 2528
	{
	public:
		SCBraveGroundTeamInfo();
		MessageHeader		header;

		struct Item
		{
			Item()
			{
				role_id = 0;
				reached_level = 0;
			}

			int role_id;
			int reached_level;
		};


		Item info[MAX_TEAM_MEMBER_NUM];
	};

	class SCBraveGroundRoleInfo			//!< 2529		
	{
	public:
		SCBraveGroundRoleInfo();
		MessageHeader header;

		int max_reached_level;			//!< 历史最高层数
		unsigned short sao_dang_reward_level;		//!<  这周领取层数
		unsigned short is_fetch_sao_dang_reward;		//!<  是否领取了
	};

	class SCBraveGroundCommonInfo			//!< 2530		勇闯地宫本身信息
	{
	public:
		SCBraveGroundCommonInfo();
		MessageHeader header;

		int brave_ground_world_level;
	};
}

#pragma pack(pop)

#endif