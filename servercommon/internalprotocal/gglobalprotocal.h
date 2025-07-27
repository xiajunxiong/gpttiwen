#ifndef __GGLOBALPROTOCAL_H__
#define __GGLOBALPROTOCAL_H__

#include "servercommon/serverdef.h"
#include "servercommon/basedef.h"
#include "servercommon/rankdef.hpp"

#include "servercommon/struct/global/worldstatusparam.hpp"
#include "servercommon/struct/global/trademarketparam.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace gglobalprotocal
{
	enum _MsgType
	{
		MT_INVALID,

		///////////////////////////////////////////   基本协议   /////////////////////////////////////////
		MT_GGLOBAL_BEGIN = 21000,
		MT_USER_MSG,										// 用户在服务器内部消息 （ggprotocal协议中的内容）	

		MT_MSG_TO_CROSS_SERVER,								// 给cross发消息 (crossgameprotocal协议)
		MT_CROSS_GAME_MSG,									// cross发给gameworld的消息 (crossgameprotocal协议)		
		MT_MSG_TO_USER,										// 给用户发消息 （Protocal用户协议内容）

		///////////////////////////////////////////   交易行  ////////////////////////////////////////////
		MT_GG_ADD_ITEM_ON_SHELVE,							// 上架货品
		MT_GG_REMOVE_ITEM_ON_SHELVE,						// 下架货品
		MT_GG_TRADE_MARKET_CASH_OUT,						// 提现

		MT_SYNC_LAST_BATTLEFILED_RANK_INFO,					// gameworld->global 同步战场排行
		MT_SYNC_PERSON_RANK_INFO,							// 同步排行榜信息
		MT_SYNC_TEAM_RANK_INFO,								// 同步组队排行榜信息
		MT_SYNC_GUILD_RANK_INFO,							// 仙盟排行信息同步
		MT_SYNC_WORLD_LEVEL_INFO,							// 同步服务器等级信息
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct MessageHeader
	{
		MessageHeader() : msg_type(MT_INVALID) {}
		MessageHeader(int type) : msg_type(type) {}

		int	msg_type;
	};

	static const int MAX_REGISTER_SCENE_LIST_NUM = 1024;

	struct GGlobalUserMsg
	{
		GGlobalUserMsg() : header(MT_USER_MSG) {}

		MessageHeader header;
		int db_index;
		int role_id;
		// 后面紧跟着ggprotocal协议中的内容
	};

	struct GGlobalMsgToUser
	{
		GGlobalMsgToUser() : header(MT_MSG_TO_USER) {}

		MessageHeader header;
		int db_index;
		int role_id;
		// 后面紧跟着Protocal用户协议内容
	};

	struct GGlobalMsgToCrossServer
	{
		GGlobalMsgToCrossServer() : header(MT_MSG_TO_CROSS_SERVER) {}
		MessageHeader header;

		int cross_activity_type;
		// 后面紧跟着crossgameprotocal协议内容
	};

	struct GGlobalCrossGameMsg
	{
		GGlobalCrossGameMsg() : header(MT_CROSS_GAME_MSG) {}
		MessageHeader header;

		// 后面紧跟着crossgameprotocal协议内容
	};

	struct SyncWorldLevelInfo
	{
		SyncWorldLevelInfo() : header(MT_SYNC_WORLD_LEVEL_INFO) {}
		MessageHeader		header;

		int					world_level;				// 世界等级
		int					top_user_level;				// 最高角色等级
		int					capability_rank_mincap;		// 战力榜最低战力
	};

	struct GGlobalAddItemOnShelve
	{
		GGlobalAddItemOnShelve() : header(MT_GG_ADD_ITEM_ON_SHELVE) {}
		MessageHeader header;

		int role_id;
		GSNetID role_netid;
		GameName role_name;
		SaleItemParam param;
	};

	struct GGlobalRemoveItemOnShelve
	{
		GGlobalRemoveItemOnShelve() : header(MT_GG_REMOVE_ITEM_ON_SHELVE) {}
		MessageHeader header;
		int role_id;
		GSNetID role_netid;
		int sale_index;
	};

	struct GGlobalTradeMarketCashOut
	{
		GGlobalTradeMarketCashOut() : header(MT_GG_TRADE_MARKET_CASH_OUT) {}
		MessageHeader header;

		int role_id;
		GSNetID role_netid;
		int sale_index;
	};


	struct SyncLastBattleFieldRankItem
	{
		UserID user_id;
		char vip;
		char sex;
		char prof;
		char camp;
		int level;
		GameName user_name;
		GameName flexible_name;
		long long rank_value;
		long long flexible_ll;
		int flexible_int;
		long long reserved;
	};
	struct SyncLastBattleFieldRankInfo
	{
		SyncLastBattleFieldRankInfo() : header(MT_SYNC_LAST_BATTLEFILED_RANK_INFO) {}

		MessageHeader		header;

		int rank_type;
		int rank_count;
		SyncLastBattleFieldRankItem rank_list[PERSON_RANK_MAX_SIZE];
	};

	struct SyncPersonRankInfo
	{
		SyncPersonRankInfo() : header(MT_SYNC_PERSON_RANK_INFO) {}

		MessageHeader		header;

		void Reset()
		{
			db_index = -1;
			role_info.Reset();

			rank_type = -1;
			rank_value = 0;

			memset(flexible_name, 0, sizeof(flexible_name));
			flexible_int = 0;
			flexible_ll = 0;
		}

		int					db_index;
		RoleBaseInfo		role_info;

		int 			rank_type;
		long long		rank_value;

		GameName		flexible_name;
		int				flexible_int;
		long long		flexible_ll;
	};

	struct SyncGuildRankInfo
	{
	public:
		SyncGuildRankInfo() : header(MT_SYNC_GUILD_RANK_INFO) {}

		MessageHeader header;

		static const int MAX_GUILD_RANK_NUM = 100;

		struct GuildRank
		{
			GuildID guild_id;
			long long rank_value;
		};

		int guild_rank_type;
		int guild_rank_count;
		GuildRank guild_rank_list[MAX_GUILD_RANK_NUM];
	};

	struct SyncTeamRankInfo
	{
		SyncTeamRankInfo() : header(MT_SYNC_TEAM_RANK_INFO) {}

		MessageHeader		header;

		TeamRankMemberInfo member_list[MAX_TEAM_MEMBER_NUM];
		int rank_type;
		long long rank_value;
		int flexible_int;
		long long flexible_ll;
	};

}

#pragma pack(pop)

#endif  // __GGLOBALPROTOCAL_H__

