#ifndef __MSG_PLATFORM_BATTLE_HPP__
#define __MSG_PLATFORM_BATTLE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/platformbattledef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum PLATFORM_BATTLE_REQ_TYPE
	{
		PLATFORM_BATTLE_REQ_TYPE_ROLE_INFO,		//角色信息
		PLATFORM_BATTLE_REQ_TYPE_RANK_INFO,		//擂台信息 p1 擂台编号
		PLATFORM_BATTLE_REQ_TYPE_CHALLENGE,			//战斗	 p1 擂台编号	,p2 rank ,  pet_index要上场的宠物，partner_id要上场的伙伴
		PLATFORM_BATTLE_REQ_TYPE_AJUST,			//调整	 p1 擂台编号,			pet_index要上场的宠物，partner_id要上场的伙伴
		PLATFORM_BATTLE_REQ_TYPE_REMOVE,			//撤下某个擂台上的伙伴和宠物  p1 擂台编号,
		PLATFORM_BATTLE_REQ_TYPE_RECORD,			//请求战报
		PLATFORM_BATTLE_REQ_TYPE_SET_NOTICE,		// 设置客户端存放红点信息 p1 1是isfirst
		PLATFORM_BATTLE_REQ_TYPE_SET_NOTICE_2,		// 设置客户端存放红点信息 p1（0开始）是lose_record_flag第几个置0
		PLATFORM_BATTLE_REQ_TYPE_LAST_RANK_INFO,		// 上次排名信息 ，下发（2791）
	};

	class CSPlatformBattleReq			//!< 2780
	{
	public:
		CSPlatformBattleReq();
		MessageHeader header;

		int req_type;
		short p1;
		short p2;

		MsgPlatformBattlePlayItem pet_index[PLATFORM_BATTLE_PET_PARTNER_MAX];		//5		p1 index p2 pos
		MsgPlatformBattlePlayItem partner_id[PLATFORM_BATTLE_PET_PARTNER_MAX];		//5		p1 partner_id p2 pos
	};

	class SCPlatformBattleRoleInfo		//!< 	2781 玩家自己守擂信息
	{
	public:
		SCPlatformBattleRoleInfo();		
		MessageHeader header;

		int count;
		MsgPlatformBattleRoleInfoItem role_info[PLATFORM_BATTLE_MAX_JOIN_NUM];			//10
	};

	struct MsgPlatformBattleRankItemPetInfo
	{
		MsgPlatformBattleRankItemPetInfo()
		{
			pos = 0;
			pet_id = 0;
			pet_capability = 0;
			pet_level = 0;
			pet_strengthen_level = 0;
			pet_skin_id = 0;
			memset(pet_name, 0, sizeof(pet_name));
		}

		int pos;
		int pet_id;				//0无效
		int pet_capability;		
		int pet_level;
		int pet_strengthen_level;
		int pet_skin_id;
		GameName pet_name;
	};

	struct MsgPlatformBattleRankItemPartnerInfo
	{
		MsgPlatformBattleRankItemPartnerInfo()
		{
			pos = 0;
			partner_id = 0;				//0无效			
			partner_capability = 0;
			partner_skin_id = 0;
			level = 0;
			str_level = 0;
		}

		int pos;
		int partner_id;
		int partner_capability;
		int partner_skin_id;	
		int level;
		int str_level;
	};


	class SCPlatformBattleRankInfo		//!< 		2782  一个擂台 所有玩家信息
	{
	public:
		SCPlatformBattleRankInfo();		
		MessageHeader header;

		struct PlatformBattleRankItem
		{
			int role_id;
			int role_level;
			int top_level;
			short avatar_type;
			short headshot_id;
			GameName role_name;

			MsgPlatformBattleRankItemPetInfo battling_pet[PLATFORM_BATTLE_PET_PARTNER_MAX];		//大小5
			MsgPlatformBattleRankItemPartnerInfo battling_partner[PLATFORM_BATTLE_PET_PARTNER_MAX];	//大小5
		};

		short platform_number;
		short count;
		PlatformBattleRankItem rank_item[PLATFORM_BATTLE_RANK_ITEM_MAX];
	};


	class CSPlatformBattleRankFirstInfoReq		//!< 		2783  
	{
	public:
		CSPlatformBattleRankFirstInfoReq();
		MessageHeader header;

		unsigned char platform_number_list[PLATFORM_BATTLE_PLATFORM_NUM];
	};

	class SCPlatformBattleRankFirstInfo		//!< 		2784  擂台第一名玩家信息
	{
	public:
		SCPlatformBattleRankFirstInfo();
		MessageHeader header;

		struct PlatformBattleRankItem
		{
			int platform_number;
			int role_id;
			int role_level;
			int top_level;
			short avatar_type;
			short headshot_id;
			GameName role_name;

			MsgPlatformBattleRankItemPetInfo battling_pet[PLATFORM_BATTLE_PET_PARTNER_MAX];		//大小5
			MsgPlatformBattleRankItemPartnerInfo battling_partner[PLATFORM_BATTLE_PET_PARTNER_MAX];	//大小5
		};

		int count;
		PlatformBattleRankItem first_item[PLATFORM_BATTLE_PLATFORM_NUM];
	};

	class CSPlatformBattlePlaformSwitch	//!< 2785 在擂台场景中切到其他擂台场景
	{
	public:
		CSPlatformBattlePlaformSwitch();
		MessageHeader header;

		int platform_seq;  //!< 1~5
	};

	class SCPlatformBattleRecord	//!< 2786 擂台战战报
	{
	public:
		SCPlatformBattleRecord();
		MessageHeader header;

		int record_num;
		CommonSavePlatformBattleRoleRecord records[PLATFORM_BATTLE_RECORD_ITEM_MAX_NUM];
	};

	class SCPlatformBattleRecordNotice	//!< 2787   通知显示红点 
	{
	public:
		SCPlatformBattleRecordNotice();
		MessageHeader header;

		short is_first_times;			//非0是没玩过
		short lose_record_flag;		//每次擂台排名降低，需要红点提示
	};

	enum PLATFORM_BATTLE_REQ2_TYPE
	{
		PLATFORM_BATTLE_REQ2_TYPE_ACHIEVEMENT,   //领取成就奖励,p1 type , p2 seq
	};

	class CSPlatformBattleReq2	//!< 2788   
	{
	public:
		CSPlatformBattleReq2();
		MessageHeader header;

		int req_type;
		int param1;
		int param2;
	};

	class SCPlatformBattleAchievementInfo	//!< 2789   成就信息
	{
	public:
		SCPlatformBattleAchievementInfo();
		MessageHeader header;

		CommonSavePlatformBattleAchievementItem achievement_info[PLATFORM_BATTLE_ACHIEVEMENT_MAX];
	};

	class SCPlatformBattleSettleAccountNotice	//!< 2790   擂台战结算
	{
	public:
		SCPlatformBattleSettleAccountNotice();
		MessageHeader header;
	};

	class SCPlatformBattleLastRankInfo 	//!< 2791   上次排名信息
	{
	public:
		SCPlatformBattleLastRankInfo();
		MessageHeader header;

		CommonSavePlatformBattleLastRankItem info[PLATFORM_BATTLE_MAX_RANK_REWARD];
	};
}

#pragma pack(pop)

#endif