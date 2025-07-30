#ifndef __MSG_SC_RANK_HPP__
#define __MSG_SC_RANK_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/serverdef.h"
#include "servercommon/rankdef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCGetPersonRankListAck	// 10000 排行榜列表
	{
	public:
		SCGetPersonRankListAck();

		MessageHeader header;

		struct PersonRankListItem
		{
			RoleSimapleInfo role_info;

			int rank_index;
			long long rank_value;
			int flexible_int;
			long long flexible_ll;
		};

		int rank_type;
		int special_type;
		int my_rank_index;									 // -1 表示未进入排行榜
		int count;
		PersonRankListItem rank_list[PERSON_RANK_MAX_SIZE];
	};

	class SCGetPersonRankTopUser
	{
	public:
		SCGetPersonRankTopUser();

		MessageHeader header;

		int rank_type;

		int user_id;
		GameName user_name;
		char sex;
		char prof;
		char camp;
		char reserved;
		int level;
		long long rank_value;
	};

	class SCGetWorldLevelAck
	{
	public:
		SCGetWorldLevelAck();

		MessageHeader header;

		int world_level;
		int top_user_level;
	};

	class SCGetGuildRankListAck
	{
	public:
		SCGetGuildRankListAck();

		MessageHeader header;

		struct GuildRankListItem
		{
			GuildID guild_id;
			GuildName guild_name;
			int tuan_zhang_uid;
			GameName tuan_zhang_name;
			short guild_level;
			short max_member_count;
			short camp;
			short member_count;
			long long rank_value;
		};

		int rank_type;
		int start_rank_index;
		int rank_count;
		GuildRankListItem rank_list[GUILD_RANK_MAX_SIZE];
	};

	class SCGetTeamRankListAck
	{
	public:
		SCGetTeamRankListAck();

		MessageHeader header;

		struct TeamRankListItem
		{
			TeamRankMemberInfo member_list[MAX_TEAM_MEMBER_NUM];
			long long rank_value;
			int flexible_int;
			long long flexible_ll;
		};

		int rank_type;
		int rank_count;
		TeamRankListItem rank_list[TEAM_RANK_MAX_SIZE];
	};

	class SCGetPetRankListAck			// 10008
	{
	public:
		SCGetPetRankListAck();

		MessageHeader header;

		struct PetRankListItem
		{
		//	RoleBaseInfo role_info;
		//	PetPro pet_info;
		//	AttributeList pet_attrs;
			RoleSimapleInfo role_info;
			PetSimpleInfo pet_info;

			int rank_index;
			long long rank_value;
			int flexible_int;
			long long flexible_ll;
		};

		unsigned int last_snapshot_time;
		int rank_type;
		int my_rank_index;									 // -1 表示未进入排行榜 , 只表示最高排名的
		int count;
		PetRankListItem rank_list[PET_RANK_MAX_SIZE];
	};

	class SCRankRefreshTime					//!< 10058 下发排行榜刷新时间
	{
	public:
		SCRankRefreshTime();
		MessageHeader header;

		unsigned int rank_refresh_timestamp;
	};

}

#pragma pack(pop)

#endif // __MSG_SC_RANK_HPP__
